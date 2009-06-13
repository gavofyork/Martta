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

#include "DeclarationFile.h"

namespace Martta
{

class RootEntity;
class Enumeration;
class FunctionResolver;
class IncomingFunctionType;

class DeclarationEntity;
class Function;
class Variable;
class TopLevelType;

class ArrayType;
class CvQualifiedType;
class PointerType;
class Resolver;

class DeclarationsHandler: public QXmlContentHandler
{
public:
	DeclarationsHandler(RootEntity* _d, QList<DeclarationFile*>* _l): m_d(_d), m_l(_l) {}
	~DeclarationsHandler() { }

	TypeEntity* resolveType(QString const& _typeId);
	QString commitToFile(QString const& _fileId, Function* _f);
	QString commitToFile(QString const& _fileId, Variable* _f);
	QString commitToFile(QString const& _fileId, TopLevelType* _f);
	void removeFromFile(TopLevelType* _f);

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

	RootEntity*						m_d;
	QList<DeclarationFile*>* 		m_l;

	Enumeration*					m_lastEnum;
	FunctionResolver*				m_lastFunction;
	IncomingFunctionType*			m_lastIncomingFunctionType;

	QMap<QString, DeclarationEntity*>	m_contexts;
	QMap<QString, Function*>		m_functions;
	QMap<QString, Variable*>		m_variables;
	QMap<QString, TopLevelType*>		m_types;

	QMap<QString, int>			m_simples;
	QMap<QString, ArrayType*> 		m_arrays;
	QMap<QString, CvQualifiedType*>	m_cvQualifieds;
	QMap<QString, PointerType*>		m_pointers;
	QMap<QString, IncomingFunctionType*>	m_functionTypes;

	QMap<QString, DeclarationFile*>	m_files;

	QList<Resolver*>				m_resolvers;
};

}

#endif
