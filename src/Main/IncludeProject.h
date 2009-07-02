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

#pragma once

#include <QDataStream>
#include <QList>
#include <QStringList>

#include "Entity.h"
#include "Function.h"

class QDomElement;

namespace Martta
{

class DeclarationFile;

class IncludeProject
{
public:
	IncludeProject(QString const& _n = ""): m_name(_n) {}

	QString							name() const { return m_name; }
	void							setName(QString const& _n) { m_name = _n; }

	QString							code() const;
	QStringList						args() const;
	bool							ownsFile(QString const& _f) const { return m_includes.contains(_f); }
	void							reset() { m_types.clear(); m_functions.clear(); m_variables.clear(); }

	QStringList						includes() const { return m_includes; }
	void							addInclude(QString const& _f = "") { m_includes.append(_f); }
	void							setInclude(int _i, QString const& _n) { m_includes[_i] = _n; }
	void							removeInclude(int _i) { m_includes.erase(m_includes.begin() + _i); }

	QStringList						libs() const { return m_libs; }
	void							addLib(QString const& _f = "") { m_libs.append(_f); }
	void							setLib(int _i, QString const& _n) { m_libs[_i] = _n; }
	void							removeLib(int _i) { m_libs.erase(m_libs.begin() + _i); }

	void							assimilate(DeclarationFile* _f);
	QList<TypeDefinition*>			types() const { return m_types; }
	QList<Function*>				functions() const { return m_functions; }
	QList<Variable*>				variables() const { return m_variables; }

	// Save/load
	void							exportDom(QDomElement& _context) const;
	void							importDom(QDomElement const& _element);

private:
	QString							m_name;
	QStringList						m_includes;
	QStringList						m_libs;

	QList<TypeDefinition*>			m_types;
	QList<Function*>				m_functions;
	QList<Variable*>				m_variables;
};

}
