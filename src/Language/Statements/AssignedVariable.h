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

namespace Martta
{

class AssignedVariable: public BareTyped, public_interface VariableNamer
{
	MARTTA_OBJECT(BareTyped)
	MARTTA_INHERITS(VariableNamer, 0)

public:
	enum { AssignedValue = FirstNamed, EndOfNamed };
	
	static bool							keyPressedOnInsertionPoint(Position const& _p, EntityKeyEvent const* _e);
	
protected:
	// From Entity via BareTyped
	virtual int							minRequired(int _i) const { return _i == OurType || _i == AssignedValue || _i == Identity ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _index) const;
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual QString						defineLayout(ViewKeys& _k) const { return (VariableNamer::defineLayout(_k) + ";Mi;^;ycode;':=';Mi;%1").arg(AssignedValue); }
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual void						onDependencyChanged(Entity*);
	virtual void						onDependencySwitched(Entity*, Entity*);
	virtual void						exportDom(QDomElement& _element) const { VariableNamer::exportDom(_element); Super::exportDom(_element); }
	virtual void						importDom(QDomElement const& _element) { VariableNamer::importDom(_element); Super::importDom(_element); }

	// From Statement via BareTyped
	virtual QString						code() const;
	
	// From BareTyped
	virtual Types						allowedTypes(int _index) const;

	// From Identifiable via VariableNamer
	virtual Identifiable*				addressableContext() const { return 0; }
};

}
