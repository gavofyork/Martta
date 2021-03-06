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

#include "VariableNamer.h"
#include "MemberValue.h"

#ifndef M_API_Members
#define M_API_Members M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Members Field: public_super MemberValue, public_interface VariableNamer
{
	MARTTA_PROPER(MemberValue)
	MARTTA_ALSO_INHERITS(VariableNamer, 0)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual bool						keyPressed(KeyEvent const* _e) { return VariableNamer::keyPressed(_e) ? true : Super::keyPressed(_e); }
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(int _a, Concept* _e) { VariableNamer::onDependencyChanged(_a, _e); Super::onDependencyChanged(_a, _e); }
	virtual List<Declaration*>			utilised() const { return actualType()->utilised(); }
	virtual String						memberInterfaceCode() const { return VariableNamer::interfaceCode(); }
	virtual String						memberImplementationCode() const { return VariableNamer::implementationCode(); }
	virtual String						memberDefineHtml() const { return defineVariableHtml() + tagOf(L"minor symbol", L";"); }
	virtual int							minRequired(int _i) const { return _i == OurType ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual Type						type() const { return MemberValue::memberifiedType(VariableNamer::type()); }
	virtual String						defineLabelCode(String const& _text) const { return L"m_" + camelCase(_text); }
	virtual bool						isInValidState() const { return Super::isInValidState() && VariableNamer::isInValidState(); }
};

}
