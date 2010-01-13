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

#include "DeclarationsHandler.h"
#include "GccXml.h"

namespace Martta
{

void GccXml::extractHeaders(QString const& _c, QXmlContentHandler* _h)
{
	QTemporaryFile f;
	f.open();
	f.write(_c.toAscii());
	f.flush();

	QString xmlfn;
	{
                QTemporaryFile xml("XXXXXX.c"); // Would be .cpp except we can only handle reading in C files at the moment.
		xml.open();
                xmlfn = xml.fileName();
	}

	QStringList searchPaths;
	QString executable;
#ifdef Q_WS_WIN
	searchPaths << (QCoreApplication::applicationDirPath() + "/../gccxml/bin/") << "C:\\Program Files\\gccxml\\bin\\";
	executable = "gccxml.exe";
#else
	searchPaths << "/usr/local/bin/" << "/usr/bin/";
	executable = "gccxml";
#endif
	foreach (QString s, searchPaths)
		if (QFile::exists(s))
		{
#ifdef M_WIN
			QStringList env = QProcess::systemEnvironment();
			int numVSInstalled = (env.filter(QRegExp("VS..COMNTOOLS.*"))).size();
			QDir g(s + "../share/gccxml-0.9");
			if(g.exists())
			{			
				int numGxVcDirs = ((g.entryList(QDir::Dirs)).filter(QRegExp("Vc.*"))).size();
				if(numVSInstalled >= numGxVcDirs)
				{
					QDir::setCurrent(s);
					QProcess::execute("cmd", QStringList() << "/C" << "gccxml_vcconfig.bat");
				}
			}				
#endif
			QProcess::execute(s + executable, QStringList() << f.fileName() << /*"--gccxml-cxxflags" << "-xc" <<*/ ("-fxml=" + xmlfn));
		}
	f.close();

	QXmlSimpleReader xmlReader;
	QFile xml(xmlfn);
	xml.open(QIODevice::ReadOnly);
	QXmlInputSource* source = new QXmlInputSource(&xml);
	xmlReader.setContentHandler(_h);
	xmlReader.parse(source);
	delete source;
	QFile::remove(xmlfn);
	delete _h;
}

QXmlContentHandler* GccXml::declarationsHandler(Concept* _d)
{
	return new DeclarationsHandler(_d);
}

}
