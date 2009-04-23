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

#ifndef MARTTAFILESEXTRACTORHANDLER_H
#define MARTTAFILESEXTRACTORHANDLER_H

#include <QStringList>
#include <QXmlContentHandler>

namespace Martta
{

class FilesExtractorHandler: public QXmlContentHandler
{
public:
	FilesExtractorHandler(QStringList* _f): m_files(_f) {}

private:
	virtual bool startDocument() { return true; }
	virtual bool startElement(QString const&, QString const& _n, QString const&, QXmlAttributes const& _a)
	{
		if (_n == "File")
			(*m_files) << _a.value("name");
		return true;
	}
	virtual bool endElement(QString const&, QString const&, QString const&) { return true; }
	virtual bool endDocument() { return true; }
	virtual bool characters(QString const&) { return true; }
	virtual bool endPrefixMapping(QString const&) { return true; }
	virtual QString errorString() const { return QString(); }
	virtual bool ignorableWhitespace(QString const&) { return true; }
	virtual bool processingInstruction(QString const&, QString const&) { return true; }
	virtual void setDocumentLocator(QXmlLocator*) {}
	virtual bool skippedEntity(QString const&) { return true; }
	virtual bool startPrefixMapping(QString const&, QString const&) { return true; }

	QStringList* m_files;
};

}

#endif
