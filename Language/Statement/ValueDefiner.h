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

#include "TypeNamer.h"
#include "WebViewable.h"
#include "Identifiable.h"

#ifndef M_API_Statement
#define M_API_Statement M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Statement ValueDefiner: public_super_interface Identifiable, public_interface WebViewable, public_interface TypeNamer
{
	MARTTA_NOTION(Identifiable)
	MARTTA_ALSO_INHERITS(WebViewable, 0)
	MARTTA_ALSO_INHERITS(TypeNamer, 1)

public:
	virtual String						identity() const { return type()->code(nick()).replace(" ", "").replace("::", ";;"); }
	virtual bool						isAccessibleAt(Position const&) const { return true; }
	virtual void						apresLoad() { Identifiable::apresLoad(); TypeNamer::apresLoad(); }
};

}

namespace MarttaSupport
{

template<>
class NameTrait< ::Martta::ValueDefiner*>
{
public:
	static String name(::Martta::ValueDefiner* _val) { return _val ? _val->nick() : String::null; }
};

}
