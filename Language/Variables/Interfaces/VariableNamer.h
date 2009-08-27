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

#include "Labelled.h"
#include "WebViewable.h"
#include "ValueDefiner.h"

namespace Martta
{

class VariableNamer: public_interface ValueDefiner, public_interface Labelled, public_interface WebViewable
{
	MARTTA_INTERFACE
	MARTTA_INHERITS(ValueDefiner, 0)
	MARTTA_INHERITS(Labelled, 1)
	MARTTA_INHERITS(WebViewable, 2)

public:
	MARTTA_NAMED(OurType)

	TypeEntity*							actualType() const;
	virtual String						summary() const { return basicCode(); }
	String								basicCode() const;

protected:
	virtual Type						type() const;
	virtual inline String				defineLabelCode(String const& _text) const { return L"l_" + camelCase(_text); }	// Overridden in MemberVariable & Argument

	inline String						interfaceCode() const { return basicCode() + ";\n"; }
	inline String						implementationCode() const { return String::null; }
	virtual inline String				defineLabelHtml(String const& _middle) const { return L"<span class=\"VariableNamer-definition\">" + type()->typeHtml(_middle) + L"</span>"; }
	bool								keyPressed(KeyEvent const* _e);
	String								defineVariableHtml() const;
	bool								isInValidState() const { return actualType()->canStandAlone(); }
};

}
