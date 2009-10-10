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

#ifndef MARTTADECLARATIONSHANDLER_H
#define MARTTADECLARATIONSHANDLER_H

#include <QtCore>
#include <QtXml>

#ifndef M_API_Projects
#define M_API_Projects M_OUTAPI
#endif

namespace Martta
{

class Concept;
class FunctionResolver;
class IncomingFunctionType;
class ArrayType;
class CvQualifiedType;
class PointerType;
class Resolver;

inline QString qs(String const& _s)
{
	return QString::fromWCharArray(_s.data(), _s.length());
}

class M_API_Projects DeclarationsHandler: public QXmlContentHandler
{
public:
	DeclarationsHandler(Concept* _d): m_d(_d) {}
	~DeclarationsHandler() { }

	Concept* resolveType(QString const& _typeId, Concept** _td = 0);
	QString nameOfType(Concept* _td) const { return m_typeNames.value(_td); }

private:
	virtual bool startDocument();
	virtual bool startElement(QString const&, QString const& _n, QString const&, QXmlAttributes const& _a);
	virtual bool endElement(QString const&, QString const& _n, QString const&);
	virtual bool endDocument();

	virtual bool characters(QString const&) { return true; }
	virtual bool endPrefixMapping(QString const&) { return true; }
	virtual QString errorString() const { return QString(); }
	virtual bool ignorableWhitespace(QString const&) { return true; }
	virtual bool processingInstruction(QString const&, QString const&) { return true; }
	virtual void setDocumentLocator(QXmlLocator*) {}
	virtual bool skippedEntity(QString const&) { return true; }
	virtual bool startPrefixMapping(QString const&, QString const&) { return true; }

	Concept*									m_d;

	Concept*									m_lastEnum;
	FunctionResolver*						m_lastFunction;
	IncomingFunctionType*					m_lastIncomingFunctionType;

	QHash<QString, Concept*>					m_contexts;
	QHash<QString, Concept*>					m_functions;
	QHash<QString, Concept*>					m_variables;
	QHash<QString, Concept*>					m_types;
	QHash<Concept*, QString>					m_typeNames;

	QHash<QString, int>						m_simples;
	QHash<QString, ArrayType*>				m_arrays;
	QHash<QString, CvQualifiedType*>		m_cvQualifieds;
	QHash<QString, PointerType*>			m_pointers;
	QHash<QString, IncomingFunctionType*>	m_functionTypes;

	QList<Resolver*>						m_resolvers;
};

}

#endif
