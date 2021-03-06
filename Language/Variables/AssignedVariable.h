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
#include "BareTyped.h"

#ifndef M_API_Variables
#define M_API_Variables M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Variables AssignedVariable: public_super BareTyped, public_interface VariableNamer
{
	MARTTA_PROPER(BareTyped)
	MARTTA_ALSO_INHERITS(VariableNamer, 0)

public:
	MARTTA_NAMED(AssignedValue)

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	// From Concept via BareTyped
	virtual int							minRequired(int _i) const { return _i == OurType || _i == AssignedValue || _i == Identity ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _index) const;
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual String						defineHtml() const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual void						onDependencyChanged(int _a, Concept* _e);
	virtual void						onDependencySwitched(Concept*, Concept*);
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); Identifiable::properties(_p); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); Identifiable::setProperties(_p); }
	virtual bool						isInValidState() const { return Super::isInValidState() && VariableNamer::isInValidState(); }
	virtual String						informationHtml() const { return Super::informationHtml(); }
	virtual Type						actualType() const;

	// From Statement via BareTyped
	virtual String						code() const;

	// From BareTyped
	virtual Types						allowedTypes(int _index) const;

	// From Identifiable via VariableNamer
	virtual Identifiable*				addressableContext() const { return 0; }
};

class M_CLASS M_API_Variables Declarable: public_super TypeConcept
{
	MARTTA_NOTION(Declarable)

public:
	virtual Type						declaredType(Type const&) const { return isNull() ? Type() : *this; }
};

}
