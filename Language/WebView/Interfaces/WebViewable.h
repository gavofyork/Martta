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

namespace Martta
{

class WebViewable
{
	MARTTA_INTERFACE

public:
	virtual ~WebViewable() {}

	virtual String							defineHtml() const { return String::null; }
	virtual String							defineEditHtml(CodeScene* _cs) const;

	static String							htmlEscape(String const& _s);

protected:
	static String							toHtml(Entity const* _e, String const& _tag = L"span");
	static String							toHtml(List<Entity const*> const& _es, String const& _delimiter = L" ", String const& _tag = L"span");
	inline String							toHtml(List<Entity*> const& _es, String const& _delimiter = L" ", String const& _tag = L"span") { return toHtml(list_const_cast<Entity const*>(_es), _delimiter, _tag); }
};

}
