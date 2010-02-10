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

#include "Virtual.h"
#include "Method.h"

#ifndef M_API_Class
#define M_API_Class M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Class VirtualMethod: public_super Method, public_interface Virtual
{
	MARTTA_PROPER(Method)
	MARTTA_ALSO_INHERITS(Virtual, 0)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<VirtualMethod>(_p, _e, "V"); }

protected:
	virtual String						memberInterfaceCode() const;
	virtual String						defineEnclosureHtml(String const& _part, String const& _middle) const { return Super::defineEnclosureHtml(_part, (_part == L"head" ? L"<span class=\"minor\">VIRTUAL</span> " : L"") + _middle); }
};

}

namespace MarttaSupport
{
template<>
class NameTrait< ::Martta::VirtualMethod*>
{
public:
	static String name(::Martta::VirtualMethod* _val) { return _val ? _val->nick() : String::null; }
};
}
