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

#ifndef M_API_Variables
#define M_API_Variables M_OUTAPI
#endif

namespace Martta
{

class M_API_Variables VariableNamer: public_interface ValueDefiner, public_interface Labelled, public_interface WebViewable
{
	MARTTA_NOTION(ValueDefiner)
	MARTTA_ALSO_INHERITS(Labelled, 0)
	MARTTA_ALSO_INHERITS(WebViewable, 1)

public:
	MARTTA_NAMED(OurType)

	TypeConcept*						actualType() const;
	virtual String						summary() const { return basicCode(); }
	String								basicCode() const;

protected:
	virtual Type						type() const;
	virtual inline String				defineLabelCode(String const& _text) const { return L"l_" + camelCase(_text); }	// Overridden in Field & Argument

	inline String						interfaceCode() const { return basicCode() + ";\n"; }
	inline String						implementationCode() const { return String::null; }
	inline void							onDependencyChanged(int, Concept* _e) { if (_e == child(OurType)) changed(Dependee::Logically | Dependee::Visually); if (_e == child(Identity)) changed(Dependee::Visually); }
	virtual inline String				defineLabelHtml(String const& _text) const { return L"<span class=\"VariableNamer-definition\">" + type()->typeHtml(composeName(_text, StringList("Variable"))) + L"</span>"; }
	bool								keyPressed(KeyEvent const* _e);
	String								defineVariableHtml() const;
	bool								isInValidState() const { return actualType()->canStandAlone(); }
};

}
