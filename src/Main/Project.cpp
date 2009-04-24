/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include <QtCore>
#include <QtGui>
#include <QtXml>

#ifdef Q_WS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

#include "AccessLabel.h"
#include "ConstLabel.h"
#include "Typed.h"
#include "Method.h"
#include "NamespaceEntity.h"
#include "Literal.h"
#include "Referenced.h"
#include "SimpleUnaryOperation.h"
#include "SimpleBinaryOperation.h"
#include "GccXml.h"
#include "IncludeProject.h"
#include "Method.h"
#include "TextLabel.h"
#include "Project.h"
#include "DeclarationFile.h"

namespace Martta
{

Project::Project(QString const& _load):
	m_filename			(_load),
	m_alteringDepends	(false),
	m_compiler			(0)
{
#ifdef Q_WS_MAC
	m_supportPath = QCoreApplication::applicationDirPath() + "/../Resources/Support/";
	CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
	m_supportPath = QString(CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding())) + "/Support/";
	qInformation() << "Support path:" << m_supportPath;
	CFRelease(appUrlRef);
	CFRelease(macPath);
#endif
#if defined(Q_WS_X11) || defined(Q_WS_WIN)
	m_supportPath = QCoreApplication::applicationDirPath() + "/../support/";
#endif

	AuxilliaryRegistrar::get()->initialiseAll();
		
	if (QFile::exists(m_filename))
		revert();
	else
		resetAsNew();

	m_classes.setParent(this);

	connect(&m_cDepends, SIGNAL(modelReset()), SLOT(reloadHeaders()));
	connect(&m_declarations, SIGNAL(modelChanged()), SIGNAL(changed()));

	{
	QTemporaryFile out;
	out.open();
	m_tempPath = out.fileName();
	}
	QFile::remove(m_tempPath);
	QDir(m_tempPath).mkdir(m_tempPath);
}

Project::~Project()
{
	QDir dir(m_tempPath);
	dir.setFilter(QDir::Files | QDir::Hidden);
	foreach (QFileInfo i, dir.entryInfoList())
		QFile::remove(i.fileName());
	QDir().rmdir(m_tempPath);
}

void Project::resetAsNew()
{
	clear();
	m_filename = QString();
	m_namespace = new NamespaceEntity;
	m_declarations.back().place(m_namespace);
	m_namespace->back().place(new TextLabel("Project"));

	IncludeProject* sc = new IncludeProject("Standard C");
	sc->addInclude("/usr/include/stdlib.h");
	sc->addInclude("/usr/include/stdio.h");
	m_cDepends.append(sc);
	reloadHeaders();

	m_namespace->killAndDelete();
	m_namespace = new NamespaceEntity;
	m_declarations.back().place(m_namespace);
	m_namespace->back().place(new TextLabel("Project"));
	Class* c = new Class;
	m_namespace->back().place(c);
	c->back().place(new TextLabel("Program"));
	c->asKind<Entity>()->apresLoad();
	m_classes << c;

	c->back().place(m_program = new Method);
	m_program->back().place(new TextLabel("main"));
	Compound* o = new Compound;
	o->prepareChildren();
	m_program->back().place(o);
	m_program->back().place(new SimpleType(Void));
	m_program->back().place(new AccessLabel);
	m_program->back().place(new ConstLabel(false));
	
	nameChanged();
	changed();
}

QString Project::code() const
{
	QString ret;

	ret += "#include \"" + m_supportPath + "msList.h\"\n";
	ret += "#include \"" + m_supportPath + "msString.h\"\n";
	ret += "#include \"" + m_supportPath + "msHash.h\"\n";
	ret += "#include \"" + m_supportPath + "msSupport.h\"\n";
	
	foreach (IncludeProject* i, cDepends())
		ret += i->code() + "\n";

	if (m_namespace)
	{
		QString ic = m_namespace->interfaceCode();
		if (ic.isEmpty())
			return QString();
		ret += ic + "\n" + m_namespace->implementationCode() + "\n";
		if (m_program && m_program->contextIs<DeclarationEntity>())
			ret += "int main(int, char**)\n{\n" + m_program->contextAs<DeclarationEntity>()->reference() + " p;\np." + m_program->entityAs<Label>(0)->code() + "();\n}\n";
	}

	return ret;
}

void Project::clear()
{
	m_alteringDepends = true;
	m_program = 0;
	m_namespace = 0;
	while (m_classes.size()) m_classes.takeLast()->killAndDelete();
	m_classes.reset();
	while (m_cDepends.size()) delete m_cDepends.takeLast();
	m_cDepends.reset();
	m_alteringDepends = false;
	m_declarations.clearEntities();
}

void Project::build()
{
	M_ASSERT(!m_compiler);

	QString src = m_tempPath + "/" + name() + ".cpp";
	QString bin = m_tempPath + "/" + exeName();

	QStringList ccArgs;
	ccArgs << src;
#if WIN32
	ccArgs << "/Fe" + bin;
	ccArgs << "/nologo";
	// TODO: Win32 linking to support library.
#else
	ccArgs << "-o" << bin;
	ccArgs << m_supportPath + "/libsupport.a";
#endif
	foreach (IncludeProject* i, cDepends())
		ccArgs << i->args();

	// output to file
	{
		QFile f(src);
		f.open(QFile::WriteOnly | QFile::Text);
		QString c = code();
#if WIN32
		c.replace("/usr/include/", "C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\include\\");
#endif
		f.write(c.toAscii());
	}
	m_compiler = new QProcess;
	connect(m_compiler, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(compileFinished()));

	qInformation() << "Compiling" << src;

#if WIN32
	QStringList env = QProcess::systemEnvironment();
	env.replaceInStrings(QRegExp("^PATH=(.*)", Qt::CaseInsensitive), "PATH=\\1;C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\BIN;C:\\Program Files\\Microsoft Visual Studio 9.0\\Common7\\IDE");
	env << "LIB=C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\LIB;C:\\Program Files\\Microsoft Visual Studio 9.0\\Common7\\IDE;C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Lib";
	env << "INCLUDE=C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\INCLUDE";
	env << "LIBPATH=C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\LIB;C:\\Program Files\\Microsoft Visual Studio 9.0\\Common7\\IDE;C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Lib";
	qInformation() << env;
	m_compiler->setEnvironment(env);
	qInformation() << ccArgs;
	m_compiler->start("cl.exe", ccArgs, QIODevice::ReadOnly);
#else
	m_compiler->start("g++", ccArgs, QIODevice::ReadOnly);
#endif
}

void Project::stopCompile()
{
	delete m_compiler;
	m_compiler = 0;
}

void Project::compileFinished()
{
	QByteArray err = m_compiler->readAllStandardError();
	QByteArray out = m_compiler->readAllStandardOutput();
	qInformation() << out;
	if (m_compiler->exitCode())
	{
		qInformation() << err.size();
		QFile::remove(m_tempPath + "/" + exeName());
		m_lastCompileError = err;
	}
	m_compiler->deleteLater();
	m_compiler = 0;
}

QString Project::executable()
{
	if (!m_compiler) build();
	// TODO: Poll, service events and show progress bar or something...
	m_compiler->waitForFinished();
	if (!QFile::exists(m_tempPath + "/" + exeName()))
		return QString();
	return m_tempPath + "/" + exeName();
}

void Project::deserialise(QDomDocument& _d)
{
	TIME_FUNCTION;

	M_ASSERT(!m_alteringDepends);
	M_ASSERT(_d.documentElement().tagName() == "project");

	m_alteringDepends = true;
	clear();
	m_alteringDepends = false;

	// Load include projects.
	QString c;
	for (QDomNode e = _d.documentElement().firstChild(); !e.isNull(); e = e.nextSibling())
		if (e.isElement() && e.nodeName() == "includeproject")
		{
			IncludeProject* i = new IncludeProject;
			i->importDom(e.toElement());
			m_cDepends << i;
			c += i->code();
		}

	QList<DeclarationFile*> files;
	TIME_STATEMENT("Main extraction") GccXml::extractHeaders(c, GccXml::declarationsHandler(&m_declarations, &files));

	QList<DeclarationFile*> orphans;
	foreach (DeclarationFile* i, files)
	{
		foreach (IncludeProject* j, cDepends())
			if (j->ownsFile(i->m_filename))
		{
			j->assimilate(i);
			goto OK;
		}
		orphans << i;
		OK:;
	}

	// Work out which orphans belong to which project.
	QMap<IncludeProject*, QStringList> smatrix;
	foreach (IncludeProject* i, cDepends())
		GccXml::extractHeaders(i->code(), GccXml::filesExtractorHandler(&smatrix[i]));

	QMap<IncludeProject*, IncludeProject*> matrix;
	foreach (IncludeProject* i, smatrix.keys())
		foreach (QString s, smatrix[i])
			foreach (IncludeProject* j, cDepends())
				if (i != j && j->code().contains(s) && !matrix.values(i).contains(j))
					matrix.insertMulti(i, j);

	foreach (IncludeProject* p, cDepends())
	{
		QList<IncludeProject*> deps = matrix.values(p);
		for(int i = 0; i < deps.size(); i++)
			foreach (IncludeProject* q, matrix.values(deps[i]))
				if (!deps.contains(q))
					deps << q;
		// deps now has total dependencies for project p
		foreach (DeclarationFile* f, orphans)		// for all orphans
			if (smatrix[p].contains(f->m_filename))	// that we include
		{
				// if none of our dependencies include it
			foreach (IncludeProject* d, deps)
				if (smatrix[d].contains(f->m_filename))
					goto OK2;
				// then adopt it and exit.
			p->assimilate(f);
			OK2:;
		}
	}

	while (files.size()) delete files.takeLast();
//	M_ASSERT(orphans.isEmpty());

	TIME_STATEMENT(importDom) m_declarations.importDom(_d.documentElement());
	TIME_STATEMENT(restorePtrs) m_declarations.restorePtrs();

	m_namespace = m_declarations.entitiesOf<NamespaceEntity>()[0];
	M_ASSERT(m_namespace);

	m_classes << m_namespace->entitiesOf<Class>();

	// Load "program"
	Entity* e = m_namespace->findEntity(_d.documentElement().namedItem("program").toElement().attribute("key"));
	m_program = e->isKind<Method>() ? e->asKind<Method>() : 0;

	QList<SafePointer<Entity> > uplist;
	uplist << m_namespace;
	TIME_STATEMENT(Depends)
	while (uplist.size())
	{
		Entity* e = uplist.takeLast();
		if (e)
		{
			foreach (Entity* i, e->entities())
				uplist << i;
			e->apresLoad();
		}
	}
	
	changed();
}

void Project::revert()
{
	QFile f(m_filename);
	f.open(QFile::ReadOnly | QFile::Text);
	QString errorStr;
	int errorLine;
	int errorColumn;

	QDomDocument doc;
	if (!doc.setContent(&f, true, &errorStr, &errorLine, &errorColumn))
		QMessageBox::critical(0, tr("Load Project"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
	else
		deserialise(doc);
}

void Project::serialise(QDomDocument& _d) const
{
	TIME_FUNCTION;

	TIME_STATEMENT(archivePtrs) m_declarations.archivePtrs();

	QDomElement root = _d.createElement("project");
	_d.appendChild(root);

	// Save include projects.
	foreach (IncludeProject* i, cDepends())
		i->exportDom(root);

	QDomElement n = _d.createElement("entity");
	n.setAttribute("kind", m_namespace->kind().name());
	TIME_STATEMENT(exportDom) m_namespace->exportDom(n);
	root.appendChild(n);

	// Save program.
	if (m_program)
	{
		QDomElement p = _d.createElement("program");
		p.setAttribute("key", m_program->key());
		root.appendChild(p);
	}
}

void Project::save() const
{
	QTemporaryFile f;
	f.open();
	QTextStream out(&f);
	QDomDocument doc("MarttaProject");
	serialise(doc);
	doc.save(out, 4);
	QFile::remove(m_filename);
	f.rename(m_filename);
}

void Project::reloadHeaders()
{
	if (m_alteringDepends) return;

	QDomDocument doc("MarttaProject");
	serialise(doc);
	clear();
	QString s;
	QTextStream t(&s);
	doc.save(t, 4);
	deserialise(doc);

	emit changed();
}

////////////////////
// Project::CDepends

bool Project::CDepends::insertRows(int _r, int _c, QModelIndex const& _p)
{
	bool ret = true;
	M_ASSERT(_c == 1);
	beginInsertRows(_p, _r, _r + _c - 1);
	if (_p == QModelIndex())
	{
		M_ASSERT(_r == size());
		append(new IncludeProject);
	}
	else if (_p.data(Project::HeadingRole).toInt() == Project::Includes)
	{
		IncludeProject* p = reinterpret_cast<IncludeProject*>(_p.data(Project::OwnerRole).value<void*>());
		M_ASSERT(p && p->includes().size() == _r);
		p->addInclude();
	}
	else
		ret = false;
	endInsertRows();
	// No need to reset here since insertion on its own will not provide a valid model for updating.
	return ret;
}

bool Project::CDepends::removeRows(int _r, int _c, QModelIndex const& _p)
{
	M_ASSERT(_c == 1);
	if (_p == QModelIndex())
	{
		M_ASSERT(_r < size());
		beginRemoveRows(_p, _r, _r + _c - 1);
		delete at(_r);
		erase(begin() + _r);
		endRemoveRows();
	}
	else if (_p.data(Project::HeadingRole).toInt() == Project::Includes)
	{
		IncludeProject* p = reinterpret_cast<IncludeProject*>(_p.data(Project::OwnerRole).value<void*>());
		M_ASSERT(p && _r < p->includes().size());
		p->removeInclude(_r);
	}
	else
		return false;
	reset();
	return true;
}

IncludeProject* Project::CDepends::checkHeading(QModelIndex _i, int _k) const
{
	M_ASSERT(_k < Max);
	if (_i != QModelIndex() && _i.parent() != QModelIndex() && _i.parent().parent() == QModelIndex() && (_i.row() == _k || _k == All))
		return at(_i.parent().row());
	return 0;
}

IncludeProject* Project::CDepends::checkItem(QModelIndex _i, int _k) const
{
	M_ASSERT(_k < Max);
	if (_i != QModelIndex() && _i.parent() != QModelIndex() && _i.parent().parent() != QModelIndex() && _i.parent().parent().parent() == QModelIndex() && (_i.parent().row() == _k || _k == All))
		return at(_i.parent().parent().row());
	return 0;
}

IncludeProject* Project::CDepends::checkProject(QModelIndex _i) const
{
	if (_i != QModelIndex() && _i.parent() == QModelIndex())
		return at(_i.row());
	return 0;
}

QVariant Project::CDepends::data(QModelIndex const& _i, int _r) const
{
	switch (_r)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			if (checkProject(_i)) return checkProject(_i)->name();
			if (checkHeading(_i, Types)) return "Types";
			if (checkHeading(_i, Functions)) return "Functions";
			if (checkHeading(_i, Variables)) return "Variables";
			if (checkHeading(_i, Includes)) return "Includes";
			if (checkItem(_i, Types)) return checkItem(_i, All)->types()[_i.row()]->code();
			if (checkItem(_i, Functions)) return checkItem(_i, All)->functions()[_i.row()]->code(Callable::InsideScope);
			if (checkItem(_i, Variables)) return checkItem(_i, All)->variables()[_i.row()]->code();
			if (checkItem(_i, Includes)) return checkItem(_i, All)->includes()[_i.row()];
			M_ASSERT(false);
		case HeadingRole:
			for (int i = 0; i < All; i++)
				if (checkHeading(_i, i)) return QVariant(i);
			if (checkRoot(_i)) return QVariant(All);
			return QVariant();
		case ItemRole:
			for (int i = 0; i < All; i++)
				if (checkItem(_i, i)) return QVariant(i);
			if (checkProject(_i)) return QVariant(All);
			return QVariant();
		case OwnerRole:
			if (checkProject(_i)) return QVariant::fromValue<void*>(checkProject(_i));
			if (checkHeading(_i, All)) return QVariant::fromValue<void*>(checkHeading(_i, All));
			if (checkItem(_i, All)) return QVariant::fromValue<void*>(checkItem(_i, All));
			return QVariant();
		default: return QVariant();
	}
}

bool Project::CDepends::setData(QModelIndex const& _i, QVariant const& _v, int _r)
{
	M_ASSERT(_r == Qt::EditRole);
	bool ret = true;
	if (checkProject(_i))
		checkProject(_i)->setName(_v.toString());
	else if (checkItem(_i, Includes))
	{
		checkItem(_i, Includes)->setInclude(_i.row(), _v.toString());
		reset();
	}
	else
		ret = false;
	return ret;
}

Qt::ItemFlags Project::CDepends::flags(QModelIndex const& _i) const
{
	if (checkProject(_i)) return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int Project::CDepends::rowCount(QModelIndex const& _i) const
{
	if (_i.column() > 0) return 0;
	if (checkRoot(_i)) return size();
	if (checkProject(_i)) return All;
	if (checkHeading(_i, Types)) return checkHeading(_i, All)->types().size();
	if (checkHeading(_i, Functions)) return checkHeading(_i, All)->functions().size();
	if (checkHeading(_i, Variables)) return checkHeading(_i, All)->variables().size();
	if (checkHeading(_i, Includes)) return checkHeading(_i, All)->includes().size();
	if (checkItem(_i, All)) return 0;
	M_ASSERT(false);
	return 0;
}

QModelIndex Project::CDepends::index(int _r, int _c, QModelIndex const& _p) const
{
	uint i = 0;
	if (_c || _p == QModelIndex())					// Top-level (a project) or not first column
		i = 0;
	else if (_p.parent() == QModelIndex())			// Second-level; V/T/F.
		i = _p.row() * Max + All + 1;
	else if (_p.parent().parent() == QModelIndex())	// Third-level; descriptor.
		i = _p.internalId() - All + _p.row();
	return createIndex(_r, _c, i);
}

QModelIndex Project::CDepends::parent(QModelIndex const& _i) const
{
	if (_i == QModelIndex() || !_i.internalId())	// Top-level (a file); no parent.
		return QModelIndex();
	uint id = _i.internalId() - 1;
	if (id % Max == All)							// Second-level; (V/T/F). parent is a file.
		return index(id / Max, 0);
	else											// Third-level; descriptor. parent is (V/T/F).
		return index(id % Max, 0, index(id / Max, 0));
}

////////////////////
// Project::Classes

bool Project::Classes::insertRows(int _r, int _c, QModelIndex const& _p)
{
	bool ret = true;
	M_ASSERT(_c == 1);
	beginInsertRows(_p, _r, _r + _c - 1);
	if (_p == QModelIndex())
	{
		M_ASSERT(_r == size());
//		append(*qobject_cast<Project*>(QAbstractItemModel::parent())->m_namespace <<= new Class);
	}
	else
		ret = false;
	endInsertRows();
	// No need to reset here since insertion on its own will not provide a valid model for updating.
	return ret;
}

bool Project::Classes::removeRows(int _r, int _c, QModelIndex const& _p)
{
	M_ASSERT(_c == 1);
	if (_p == QModelIndex())
	{
		M_ASSERT(_r < size());
		beginRemoveRows(_p, _r, _r + _c - 1);
		at(_r)->killAndDelete();
		erase(begin() + _r);
		endRemoveRows();
	}
	else
		return false;
	return true;
}

QVariant Project::Classes::data(QModelIndex const& _i, int _r) const
{
	switch (_r)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			if (checkRoot(_i))
				return QVariant();
			else
				return at(_i.row())->name();
		case OwnerRole:
			if (!checkRoot(_i)) return QVariant::fromValue<void*>(at(_i.row()));
			return QVariant();
		default: return QVariant();
	}
}

bool Project::Classes::setData(QModelIndex const& _i, QVariant const&, int _r)
{
	M_ASSERT(_r == Qt::EditRole);
	bool ret = true;
	if (checkRoot(_i))
		ret = false;
	else
	{
//		at(_i.row())->setName(_v.toString());
		emit dataChanged(_i, _i);
	}
	return ret;
}

Qt::ItemFlags Project::Classes::flags(QModelIndex const& _i) const
{
	if (!checkRoot(_i))
		return /*Qt::ItemIsEditable | */Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int Project::Classes::rowCount(QModelIndex const& _i) const
{
	if (checkRoot(_i)) return size();
	else return 0;
	M_ASSERT(false);
	return 0;
}

QModelIndex Project::Classes::index(int _r, int _c, QModelIndex const& _p) const
{
	uint i = 0;
	if (_c || _p == QModelIndex())					// Top-level (a project) or not first column
		i = 0;
	return createIndex(_r, _c, i);
}

QModelIndex Project::Classes::parent(QModelIndex const& _i) const
{
	if (_i == QModelIndex() || !_i.internalId())	// Top-level (a file); no parent.
		return QModelIndex();
	M_ASSERT(0);
	return QModelIndex();
}

}
