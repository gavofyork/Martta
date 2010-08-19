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

#ifdef M_WIN
#include <iostream>
using namespace std;

#include <QStringList>
#include <QProcess>
#include <QRegExp>
#include <QTemporaryFile>
#include <QDir>
#include <QTextStream>
#endif

#include <msList.h>
#include <msString.h>
#include <msStringList.h>
using namespace MarttaSupport;

#ifdef M_WIN
inline String xqs(QString const& _qs)
{
	String ret;
	ret.resize(_qs.length());
	wchar_t* d = ret.data();
	_qs.toWCharArray(d);
	ret.dataChanged(d);
	return ret;
}
inline QString xqs(String const& _s)
{
	return QString::fromWCharArray(_s.data(), _s.length());
}
#endif

#include "Function.h"
#include "Composite.h"
#include "CDependency.h"
#include "CModule.h"

namespace Martta
{

MARTTA_PROPER_CPP(CModule);
MARTTA_NAMED_CPP(CModule, RequiredIncludes);
MARTTA_NAMED_CPP(CModule, MainFunction);
MARTTA_PROPER_CPP(CModuleDependency);

// TODO: need to get temppath from Qt-aware part of code.
CModule::CModule()
#ifndef M_WIN
:	m_tempPath		(L"/tmp")
#endif
{
#ifdef M_WIN
	{
	QTemporaryFile out;
	out.open();
	m_tempPath = qPrintable(out.fileName());
	}
	QFile::remove(m_tempPath.toCString());
	QDir(m_tempPath.toCString()).mkdir(m_tempPath.toCString());
#endif
}

CModule::~CModule()
{
#ifdef M_WIN
	// TODO: clean up temp path
	QDir dir(m_tempPath.toCString());
	dir.setFilter(QDir::Files | QDir::Hidden);
	foreach (QFileInfo i, dir.entryInfoList())
		QFile::remove(i.fileName());
	QDir().rmdir(m_tempPath.toCString());
#endif
	if (!m_tempBatName.isEmpty())
		remove(m_tempBatName.toCString());
}

Kinds CModule::allowedKinds(int _i) const
{
	if (_i == RequiredIncludes)
		return Kind::of<CDependency>();
	if (_i == 0)
		return Kind::of<Function>();
	return Super::allowedKinds(_i);
}

String CModule::includeCode() const
{
	String ret;
	foreach (CDependency* i, childrenAs<CDependency>(RequiredIncludes))
		ret += i->code();
	return ret;
}

StringList CModule::libs() const
{
	StringList ret;
	foreach (CDependency* i, childrenAs<CDependency>(RequiredIncludes))
		ret += i->libs();
	return ret;
}

String CModule::finalCode() const
{
	String ret;
	ret += "#include \"" + supportPath() + "msList.h\"\n";
	ret += "#include \"" + supportPath() + "msString.h\"\n";
	ret += "#include \"" + supportPath() + "msHash.h\"\n";
	ret += "#include \"" + supportPath() + "msSupport.h\"\n";
	ret += includeCode();
	{
		CModule* composed = Composite::composeTree(this)->asKind<CModule>();
		ret += composed->Super::finalCode();
		delete composed;
	}
//	ret += Super::finalCode();
	if (childIs<Function>(0))
		ret += "int main(int argc, char** argv)\n{\n\treturn " + childAs<Function>(0)->reference() + "(argc, argv);\n}\n";

	debugTree();
	return ret;
}

List<StringList> CModule::steps() const
{
	String src = m_tempPath + "/" + nick() + ".cpp";
	String bin = m_tempPath + "/" + targetName();

	StringList ccArgs;
#ifdef M_WIN
	ccArgs << src;
	ccArgs << L"/Fe" + bin;
	ccArgs << L"/nologo";
	// TODO: Win32 linking to support library.
#else
	ccArgs << L"g++";
	ccArgs << src;
	ccArgs << L"-o" << bin;
	ccArgs << L"-lsupport";
	ccArgs << L"-L" + supportPath();
	ccArgs << L"-Wl,-R" + supportPath();
#endif

	foreach (String i, libs())
		ccArgs << L"-l" + i;

	// output to file
	{
		String c = finalCode();
#ifdef M_WIN
		c.replace("/usr/include/", "");
#endif
		FILE* f = fopen(src.toCString(), "w");
		fwrite(c.toCString(), c.length(), 1, f);
		fclose(f);
	}

	mInfo() << "Compiling" << src;
	List<StringList> ret;

#ifdef M_WIN
	QStringList env = QProcess::systemEnvironment();
	int index = env.lastIndexOf(QRegExp("VS..COMNTOOLS.*"));
	QRegExp r("^([A-Z0-9]+)=(.*)$");
	r.exactMatch(env[index]);
	if (m_tempBatName.isEmpty())
	{
		QTemporaryFile tempBat(QDir::tempPath() + "\\XXXXXX.bat");
		tempBat.open();
		tempBat.setAutoRemove(false);
		m_tempBatName = xqs(tempBat.fileName());
	}
	QFile tempBat(xqs(m_tempBatName));
	tempBat.open(QIODevice::WriteOnly | QIODevice::Truncate);
	QTextStream s(&tempBat);
	s << "call \"%" << r.cap(1) << "%vsvars32.bat\"" << endl;
	s << "cl \"" << xqs(ccArgs.join(L"\" \"")) << "\"" << endl;
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

String CModule::targetName() const
{
#ifdef M_WIN
	return nick() + ".exe";
#else
	return nick();
#endif
}

}
