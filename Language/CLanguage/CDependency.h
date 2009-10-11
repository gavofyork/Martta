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

#include <msString.h>
#include <msStringList.h>
using namespace MarttaSupport;

#include "Concept.h"

#ifndef M_API_CLanguage
#define M_API_CLanguage M_OUTAPI
#endif

namespace Martta
{

class M_API_CLanguage CDependency: public_super Concept
{
	MARTTA_PROPER(Concept)

public:
	String const&					name() const { return m_name; }
	void							setName(String const& _n) { m_name = _n; }

	String							code() const;
	StringList						args() const;
	bool							ownsFile(String const& _f) const { return m_includes.contains(_f); }

	StringList const&				includes() const { return m_includes; }
	void							addInclude(String const& _f = "") { m_includes.append(_f); }
	void							setInclude(int _i, String const& _n) { m_includes[_i] = _n; }
	void							removeInclude(int _i) { m_includes.erase(m_includes.begin() + _i); }

	StringList const&				libs() const { return m_libs; }
	void							addLib(String const& _f = "") { m_libs.append(_f); }
	void							setLib(int _i, String const& _n) { m_libs[_i] = _n; }
	void							removeLib(int _i) { m_libs.erase(m_libs.begin() + _i); }

protected:
	virtual void					properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"includes"] = m_includes.join(L'*'); _p[L"libs"] = m_libs.join(L'*'); _p[L"name"] = m_name; }
	virtual void					setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_includes = _p[L"includes"].split(L'*'); m_libs = _p[L"libs"].split(L'*'); m_name = _p[L"name"]; }

private:
	String							m_name;
	StringList						m_libs;
	StringList						m_includes;
};

}
