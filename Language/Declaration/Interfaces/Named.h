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
using namespace MarttaSupport;

#include "Meta.h"

#ifndef M_API_Declaration
#define M_API_Declaration M_OUTAPI
#endif

namespace Martta
{

class M_API_Declaration Named
{
public:
	virtual String								nick() const { return String::null; }
	virtual Concept const*						associate() const { return 0; }

	virtual ~Named() {}
};

}

namespace MarttaSupport
{
template<>
class NameTrait< ::Martta::Named*>
{
public:
	static String name(::Martta::Named* _val) { return _val ? _val->nick() : String::null; }
};
}

namespace Martta
{

class M_API_Declaration SimpleNamed: public Named
{
public:
	SimpleNamed(String const& _nick): m_nick(_nick) {}

	virtual String								nick() const { return m_nick; }

private:
	String										m_nick;
};

}
