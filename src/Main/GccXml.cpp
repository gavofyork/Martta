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

#include "TypeEntity.h"
#include "DeclarationsHandler.h"
#include "FilesExtractorHandler.h"
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
		QTemporaryFile xml;
		xml.open();
		xmlfn = xml.fileName();
	}
	TIME_STATEMENT("GccXML")
	if (QFile::exists("/usr/bin/gccxml"))
		QProcess::execute("/usr/bin/gccxml", QStringList() << f.fileName() << "--gccxml-cxxflags" << "-x c" << ("-fxml=" + xmlfn));
	else if (QFile::exists("/usr/local/bin/gccxml"))
		QProcess::execute("/usr/local/bin/gccxml", QStringList() << f.fileName() << "--gccxml-cxxflags" << "-x c" << ("-fxml=" + xmlfn));
	else if (QFile::exists("C:\\Program Files\\gccxml\\bin\\gccxml.exe"))
		QProcess::execute("C:\\Program Files\\gccxml\\bin\\gccxml.exe", QStringList() << f.fileName() << "--gccxml-cxxflags" << "-xc" << ("-fxml=" + xmlfn));
	f.close();

	QXmlSimpleReader xmlReader;
	QFile xml(xmlfn);
	xml.open(QIODevice::ReadOnly);
	QXmlInputSource* source = new QXmlInputSource(&xml);
	xmlReader.setContentHandler(_h);
	TIME_STATEMENT("Parser") xmlReader.parse(source);
	delete source;
	QFile::remove(xmlfn);
	delete _h;
}

QXmlContentHandler* GccXml::declarationsHandler(RootEntity* _d, QList<DeclarationFile*>* _l)
{
	return new DeclarationsHandler(_d, _l);
}

QXmlContentHandler* GccXml::filesExtractorHandler(QStringList* _f)
{
	return new FilesExtractorHandler(_f);
}

}
