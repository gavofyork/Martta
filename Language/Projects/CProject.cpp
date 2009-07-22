/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version 1.0
 * (the "License"); you may not use this file except in compliance with the
 * License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations under
 * the License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include <cstdio>

#include <QtXml>

#include <msList.h>
#include <msStringList.h>
using namespace MarttaSupport;

#include "Function.h"
#include "CDependency.h"
#include "CProject.h"
#undef inline

namespace Martta
{

MARTTA_OBJECT_CPP(CProject);
MARTTA_OBJECT_CPP(CProjectDependency);

static inline String qs(QString const& _qs)
{
	String ret;
	ret.resize(_qs.length());
	wchar_t* d = ret.data();
	_qs.toWCharArray(d);
	ret.dataChanged(d);
	return ret;
}

static inline QString qs(String const& _s)
{
	return QString::fromWCharArray(_s.data(), _s.length());
}

CProject::CProject():
	m_tempPath		(L"/tmp")
{
	/*
	// TODO: make a temp path
	{
	QTemporaryFile out;
	out.open();
	m_tempPath = out.fileName();
	}
	QFile::remove(m_tempPath);
	QDir(m_tempPath).mkdir(m_tempPath);
	*/
}

CProject::~CProject()
{
	/*
	// TODO: clean up temp path
	QDir dir(m_tempPath);
	dir.setFilter(QDir::Files | QDir::Hidden);
	foreach (QFileInfo i, dir.entryInfoList())
		QFile::remove(i.fileName());
	QDir().rmdir(m_tempPath);
	*/
	if (!m_tempBatName.isEmpty())
		remove(m_tempBatName.toCString());
}

QDomElement exportDom(QDomDocument& _doc, Entity const* _e)
{
	QDomElement n = _doc.createElement("entity");
	n.setAttribute("kind", qs(_e->kind().name()));
	Hash<String, String> h;
	_e->properties(h);
	for (Hash<String, String>::Iterator i = h.begin(); i != h.end(); ++i)
		n.setAttribute(qs(i.key()), qs(i.value()));
	String iId = _e->namedIndexId();
	if (!iId.isEmpty())
		n.setAttribute("index", qs(iId));
	foreach (Entity* e, _e->children())
		n.appendChild(exportDom(_doc, e));
	return n;
}

void CProject::save() const
{
	QDomDocument doc("MarttaProject");
	doc.appendChild(exportDom(doc, this));
	QFile f(qs(filename()));
	f.open(QFile::WriteOnly);
	QTextStream fs(&f);
	doc.save(fs, 4);
}

Entity* importDom(QDomElement const& _el, Entity* _p)
{
	Entity* e = Entity::spawn(qs(_el.attribute("kind")));
	Assert(e, "Spawn doesn't know anything about this kind, yet it did when exported.");

	Hash<String, String> h;
	String index;
	for (uint i = 0; i < _el.attributes().length(); i++)
		if (_el.attributes().item(i).nodeName() == "index")
			index = qs(_el.attributes().item(i).nodeValue());
		else
			h.insert(qs(_el.attributes().item(i).nodeName()), qs(_el.attributes().item(i).nodeValue()));

	if (_p)
		e->silentMove(_p->named(index));

	e->setProperties(h);
	for (QDomNode i = _el.firstChild(); !i.isNull(); i = i.nextSibling())
		if (i.isElement() && i.toElement().tagName() == "entity")
			importDom(i.toElement(), e);
	return e;
}

Project* CProject::load(String const& _filename)
{
	QFile f(qs(_filename));
	if (!f.open(QFile::ReadOnly | QFile::Text))
	{
//		QMessageBox::critical(0, tr("Load Project"), tr("File does not exist! (name is %1)").arg(f.fileName()));
		return 0;
	}

	QString errorStr;
	int errorLine;
	int errorColumn;

	QDomDocument doc;
	if (!doc.setContent(&f, true, &errorStr, &errorLine, &errorColumn))
	{
//		QMessageBox::critical(0, tr("Load Project"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
		return 0;
	}
	if (doc.doctype().name() != "MarttaProject")
	{
//		QMessageBox::critical(0, tr("Load Project"), tr("Invalid file format (%1)").arg(doc.documentType().name()));
		return 0;
	}
	return importDom(doc.documentElement(), 0)->asKind<Project>();
}

Kinds CProject::allowedKinds(int _i) const
{
	if (_i == RequiredIncludes)
		return Kind::of<CDependency>();
	if (_i == 0)
		return Kind::of<Function>();
	return Super::allowedKinds(_i);
}

String CProject::includeCode() const
{
	String ret;
	foreach (CDependency* i, childrenAs<CDependency>(RequiredIncludes))
		ret += i->code();
	return ret;
}

StringList CProject::libs() const
{
	StringList ret;
	foreach (CDependency* i, childrenAs<CDependency>(RequiredIncludes))
		ret += i->libs();
	return ret;
}

// TODO: remove once Qt is exorcised.
/*static inline QStringList qs(StringList const& _s)
{
	QStringList r;
	foreach (String i, _s)
		r.append(qs(i));
	return r;
}*/

String CProject::finalCode() const
{
	String ret;
	ret += "#include \"" + supportPath() + "msList.h\"\n";
	ret += "#include \"" + supportPath() + "msString.h\"\n";
	ret += "#include \"" + supportPath() + "msHash.h\"\n";
	ret += "#include \"" + supportPath() + "msSupport.h\"\n";
	ret += includeCode();
	ret += Super::finalCode();
	if (childIs<Function>(0))
		ret += "int main(int, char**)\n{\n" + childAs<Function>(0)->reference() + "();\n}\n";
	return ret;
}

List<StringList> CProject::steps() const
{
	String src = m_tempPath + "/" + name() + ".cpp";
	String bin = m_tempPath + "/" + targetName();

	StringList ccArgs;
#ifdef _MSC_VER
	ccArgs << L"/Fe" + bin;
	ccArgs << L"/nologo";
	// TODO: Win32 linking to support library.
#else
	ccArgs << L"g++";
	ccArgs << L"-o" << bin;
	ccArgs << supportPath() + L"/libsupport.a";
#endif
	ccArgs << src;

	foreach (String i, libs())
		ccArgs << L"-l" + i;

	// output to file
	{
		String c = finalCode();
#ifdef _MSC_VER
		c.replace("/usr/include/", "");
#endif
		FILE* f = fopen(src.toCString(), "w");
		fwrite(c.toCString(), c.length(), 1, f);
		fclose(f);
	}

	mInfo() << "Compiling" << src;
	List<StringList> ret;

#ifdef WIN32
	QStringList env = QProcess::systemEnvironment();
	int index = env.lastIndexOf(QRegExp("VS..COMNTOOLS.*"));
	QRegExp r("^([A-Z0-9]+)=(.*)$");
	r.exactMatch(env[index]);
	if (m_tempBatName.isEmpty())
	{
		QTemporaryFile tempBat(QDir::tempPath() + "\\XXXXXX.bat");
		tempBat.open();
		tempBat.setAutoRemove(false);
		m_tempBatName = qs(tempBat.fileName());
	}
	QTextStream s(&tempBat);
	s << "call \"%" << r.cap(1) << "%vsvars32.bat\"" << endl;
	s << "cl \"" << qs(ccArgs.join(L"\" \"")) << "\"" << endl;
	tempBat.close();

	StringList batArgs;
	batArgs << L"/C";
	batArgs << m_tempBatName;
	QDir::setCurrent(QDir::tempPath());
	ret << (StringList(L"cmd") << batArgs);
#else
	ret << ccArgs;
#endif

	ret << StringList(bin);
	return ret;
}

String CProject::targetName() const
{
#ifdef WIN32
	return name() + ".exe";
#else
	return name();
#endif
}

}
