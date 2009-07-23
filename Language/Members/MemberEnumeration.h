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

#include "EnumerationNamer.h"
#include "Labelled.h"
#include "Member.h"

namespace Martta
{

class MemberEnumeration: public Member, public_interface EnumerationNamer, public_interface Labelled
{
	MARTTA_OBJECT(Member)
	MARTTA_INHERITS(EnumerationNamer, 0)
	MARTTA_INHERITS(Labelled, 1)
	
public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	// From Member
	virtual String						memberDefineLayout(ViewKeys const& _k) const { return EnumerationNamer::defineLayout(_k); }
	virtual String						memberInterfaceCode() const { return EnumerationNamer::interfaceCode(); }
	virtual String						memberImplementationCode() const { return String::null; }
	
	// From TypeDefinition/EnumerationNamer
	virtual bool						hasDefaultConstructor() const { return EnumerationNamer::hasDefaultConstructor(); }
	virtual Types						assignableTypes() const { return Type(const_cast<MemberEnumeration*>(this)); }
	virtual String						code() const { return codeName(); }
	virtual List<Declaration*>			utilisedInUse() const { return List<Declaration*>() << const_cast<MemberEnumeration*>(this); }
	
	// From Declaration
	virtual List<ValueDefiner*>			valuesAdded() const { return EnumerationNamer::valuesAdded(); }

	// From Entity
	virtual int							minRequired(int _i) const { return _i == Cardinals ? 1 : _i == Identity ? 0 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual int							familyDependencies() { return DependsOnChildren; }
	virtual void						onDependencyAdded(Entity* _e) { EnumerationNamer::onDependencyAdded(_e); }
	virtual void						onDependencyChanged(Entity* _e) { EnumerationNamer::onDependencyChanged(_e); }
	virtual void						onDependencyRemoved(Entity* _e, int) { EnumerationNamer::onDependencyRemoved(_e); }
	virtual bool						keyPressed(KeyEvent const* _e) { AssertNR(isComplete()); return EnumerationNamer::keyPressed(_e) ? true : Super::keyPressed(_e); }
	virtual Entity*						isExpander() const { return EnumerationNamer::isExpander(); }
	virtual inline String				defineLabelName(String const& _text) const { return camelCase(_text, true); }
	
};

}