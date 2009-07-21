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

#include <msList.h>
#include <msStringList.h>
using namespace MarttaSupport;

#include "Function.h"
#include "CDependency.h"
#include "CProject.h"

namespace Martta
{

MARTTA_OBJECT_CPP(CProject);
MARTTA_OBJECT_CPP(CProjectDependency);

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

#if 0
void compile()
{
	AssertNR(!m_compiler);
	StringList buildLine;
	m_compiler = new QProcess;
	connect(m_compiler, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(compileFinished()));
	m_compiler->start(5qs(buildLine[0]), qs(buildLine.mid(1)), QIODevice::ReadOnly);
}
#endif

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
		ret += "int main(int, char**)\n{\n" + childAs<Function>(0)->codeName() + "();\n}\n";
	return ret;
}

List<StringList> CProject::buildCommands() const
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

#ifdef Q_WS_WIN
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
	return StringList() << L"cmd" << batArgs;
#else
	return List<StringList>() << ccArgs;
#endif
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
