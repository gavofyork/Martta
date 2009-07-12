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

#include "BuiltinType.h"
#include "Namespace.h"
#include "TextLabel.h"

#include "GccXml.h"
#include "Timer.h"
#include "DeclarationFile.h"
#include "IncludeProject.h"
#include "Project.h"

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
	mInfo() << "Support path:" << m_supportPath.toLatin1().data();
	CFRelease(appUrlRef);
	CFRelease(macPath);
#endif
#ifdef Q_WS_WIN
	m_supportPath = QCoreApplication::applicationDirPath() + "/Support/";
#endif
#ifdef Q_WS_X11
	m_supportPath = QCoreApplication::applicationDirPath() + "/../support/";
#endif
	AuxilliaryRegistrar::get()->initialiseClasses();
	
	m_root = AuxilliaryRegistrar::get()->auxilliary("Martta::Root")->create();
				
	if (QFile::exists(m_filename))
		revert();
	else
		resetAsNew();

	connect(&m_cDepends, SIGNAL(modelReset()), SLOT(reloadHeadersAndUpdateSubject()));

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
	clear();
	m_root->killAndDelete();
	AuxilliaryRegistrar::get()->finaliseClasses();
}

void Project::resetAsNew()
{
	clear();
	m_filename = QString();
	
	m_namespace = Entity::evaluate("Namespace");
	m_root->back().place(m_namespace);

	IncludeProject* sc = new IncludeProject("Standard C");
#ifdef Q_WS_WIN
	// TODO: Use .bat file to output & read proper paths for chosen compiler.
//	sc->addInclude("C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/include/stdlib.h");
//	sc->addInclude("C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/include/stdio.h");
	sc->addInclude("stdlib.h");
	sc->addInclude("stdio.h");
#else
	sc->addInclude("/usr/include/stdlib.h");
	sc->addInclude("/usr/include/stdio.h");
#endif

	m_cDepends.append(sc);
	reloadHeaders();

	m_namespace->killAndDelete();
	m_namespace = Entity::evaluate(L"Namespace{TextLabel[text=project]}{Class{TextLabel[text=program]}{Method{TextLabel[text=main]}{BuiltinType[id=0]}}}");
	m_root->back().place(m_namespace);
	
	emit subjectInvalid();
	emit nameChanged();
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
		QString ic = qs(m_namespace->asKind<Declaration>()->interfaceCode());
		if (ic.isEmpty())
			return QString();
		ret += ic + "\n" + qs(m_namespace->asKind<Declaration>()->implementationCode()) + "\n";
		if (m_program && m_program->parentIs<Declaration>())
			ret += "int main(int, char**)\n{\n" + qs(m_program->parentAs<Declaration>()->reference()) + " p;\np." + qs(m_program->asKind<Declaration>()->codeName()) + "();\n}\n";
	}

	return ret;
}

void Project::clear()
{
	m_alteringDepends = true;
	m_program = 0;
	m_namespace = 0;
	m_root->clearEntities();
	while (m_cDepends.size()) delete m_cDepends.takeLast();
	m_cDepends.reset();
	m_alteringDepends = false;
}

void Project::build()
{
	AssertNR(!m_compiler);

	QString src = m_tempPath + "/" + name() + ".cpp";
	QString bin = m_tempPath + "/" + exeName();

	QStringList ccArgs;
	ccArgs << src;
#ifdef Q_WS_WIN
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
#ifdef _MSC_VER
		c.replace("/usr/include/", "");//"C:\\Program Files\\Microsoft Visual Studio .NET 2003\\VC\\include\\");
#endif
		f.write(c.toAscii());
	}
	m_compiler = new QProcess;
	connect(m_compiler, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(compileFinished()));

	mInfo() << "Compiling" << src.toLatin1().data();

#ifdef Q_WS_WIN
	QStringList env = QProcess::systemEnvironment();
	int index = env.lastIndexOf(QRegExp("VS..COMNTOOLS.*"));
	QRegExp r("^([A-Z0-9]+)=(.*)$");
	r.exactMatch(env[index]);
	QTemporaryFile tempBat(QDir::tempPath() + "\\XXXXXX.bat");
	tempBat.open();
	tempBat.setAutoRemove(false);
	QString tempBatName = tempBat.fileName();
	QTextStream s(&tempBat);
	s << "call \"%" << r.cap(1) << "%vsvars32.bat\"" << endl;
	s << "cl \"" << ccArgs.join(QString("\" \"")) << "\"" << endl;
	tempBat.close();

	QStringList batArgs;
	batArgs.clear();
	batArgs << "/C";
	batArgs << tempBatName;
	QDir::setCurrent(QDir::tempPath());
	m_compiler->start("cmd", batArgs, QIODevice::ReadOnly);
	QFile::remove(tempBatName);
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
	mInfo() << out.data();
	if (m_compiler->exitCode())
	{
		mInfo() << err.size();
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

void importDom(QDomElement const& _el, Entity* _p)
{
	Entity* e = _p->spawn(qs(_el.attribute("kind")));
	Assert(e, "Spawn doesn't know anything about this kind, yet it did when exported.");
	
	Hash<String, String> h;
	String index;
	for (uint i = 0; i < _el.attributes().length(); i++)
		if (_el.attributes().item(i).nodeName() == "index")
			index = qs(_el.attributes().item(i).nodeValue());
		else
			h.insert(qs(_el.attributes().item(i).nodeName()), qs(_el.attributes().item(i).nodeValue()));
	e->silentMove(_p->named(index));

	e->setProperties(h);
	for (QDomNode i = _el.firstChild(); !i.isNull(); i = i.nextSibling())
		if (i.isElement() && i.toElement().tagName() == "entity")
			importDom(i.toElement(), e);
}

void Project::deserialise(QDomDocument& _d)
{
	TIME_FUNCTION;

	AssertNR(!m_alteringDepends);
	AssertNR(_d.documentElement().tagName() == "project");

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
	TIME_STATEMENT("Main extraction") GccXml::extractHeaders(c, GccXml::declarationsHandler(m_root, &files));

	List<DeclarationFile*> orphans;
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
//	AssertNR(orphans.isEmpty());

	ChangeMan::get()->sleep();
	TIME_STATEMENT(importDom) importDom(_d.documentElement().namedItem("entity").toElement(), m_root);
	TIME_STATEMENT(restorePtrs) m_root->apresLoad();
	ChangeMan::get()->wake();
	
	m_namespace = m_root->childOf<Namespace>();
	AssertNR(m_namespace);

	// Load "program"
	QString k = _d.documentElement().namedItem("program").toElement().attribute("key");
	
	if (Identifiable* e = m_root->asKind<Identifiable>()->find(qs(k)))
		m_program = e->self();
	else
		m_program = 0;

	List<SafePointer<Entity> > uplist;
	uplist << m_namespace;
	TIME_STATEMENT(Depends)
	while (uplist.size())
	{
		Entity* e = uplist.takeLast();
		if (e)
		{
			foreach (Entity* i, e->children())
				uplist << i;
			e->apresLoad();
		}
	}
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

void exportDom(QDomElement& _pe, Entity* _e)
{
	QDomElement n = _pe.ownerDocument().createElement("entity");
	n.setAttribute("kind", qs(_e->kind().name()));
	Hash<String, String> h;
	_e->properties(h);
	for (Hash<String, String>::Iterator i = h.begin(); i != h.end(); ++i)
		n.setAttribute(qs(i.key()), qs(i.value()));
	String iId = _e->namedIndexId();
	if (!iId.isEmpty())
		n.setAttribute("index", qs(iId));
	_pe.appendChild(n);
	foreach (Entity* e, _e->children())
		exportDom(n, e);
}

void Project::serialise(QDomDocument& _d) const
{
	TIME_FUNCTION;

	QDomElement root = _d.createElement("project");
	_d.appendChild(root);

	// Save include projects.
	foreach (IncludeProject* i, cDepends())
		i->exportDom(root);
	
	TIME_STATEMENT(exportDom) exportDom(root, m_namespace);

	// Save program.
	if (m_program)
	{
		QDomElement p = _d.createElement("program");
		p.setAttribute("key", qs(m_program->asKind<Identifiable>()->key()));
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
	
#if defined(DEBUG)
	List<Entity*> es = List<Entity*>() << m_namespace;
	while (es.size())
	{
		Entity* e = es.takeLast();
		AssertNR(e->root() == m_root);
		es << e->children();
	}
#endif
}

////////////////////
// Project::CDepends

bool Project::CDepends::insertRows(int _r, int _c, QModelIndex const& _p)
{
	bool ret = true;
	AssertNR(_c == 1);
	beginInsertRows(_p, _r, _r + _c - 1);
	if (_p == QModelIndex())
	{
		AssertNR(_r == size());
		append(new IncludeProject);
	}
	else if (_p.data(Project::HeadingRole).toInt() == Project::Includes)
	{
		IncludeProject* p = reinterpret_cast<IncludeProject*>(_p.data(Project::OwnerRole).value<void*>());
		AssertNR(p && p->includes().size() == _r);
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
	AssertNR(_c == 1);
	if (_p == QModelIndex())
	{
		AssertNR(_r < size());
		beginRemoveRows(_p, _r, _r + _c - 1);
		delete at(_r);
		erase(begin() + _r);
		endRemoveRows();
	}
	else if (_p.data(Project::HeadingRole).toInt() == Project::Includes)
	{
		IncludeProject* p = reinterpret_cast<IncludeProject*>(_p.data(Project::OwnerRole).value<void*>());
		AssertNR(p && _r < p->includes().size());
		p->removeInclude(_r);
	}
	else
		return false;
	reset();
	return true;
}

IncludeProject* Project::CDepends::checkHeading(QModelIndex _i, int _k) const
{
	AssertNR(_k < Max);
	if (_i != QModelIndex() && _i.parent() != QModelIndex() && _i.parent().parent() == QModelIndex() && (_i.row() == _k || _k == All))
		return at(_i.parent().row());
	return 0;
}

IncludeProject* Project::CDepends::checkItem(QModelIndex _i, int _k) const
{
	AssertNR(_k < Max);
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
			if (checkItem(_i, Types)) return qs(checkItem(_i, All)->types()[_i.row()]->code());
			if (checkItem(_i, Functions)) return qs(checkItem(_i, All)->functions()[_i.row()]->basicCode(LambdaNamer::InsideScope));
			if (checkItem(_i, Variables)) return qs(checkItem(_i, All)->variables()[_i.row()]->basicCode());
			if (checkItem(_i, Includes)) return checkItem(_i, All)->includes()[_i.row()];
			AssertNR(false);
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
	(void)(_r);
	AssertNR(_r == Qt::EditRole);
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
	if (checkProject(_i))
		return (Qt::ItemFlag)(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	return (Qt::ItemFlag)(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
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
	AssertNR(false);
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

}
