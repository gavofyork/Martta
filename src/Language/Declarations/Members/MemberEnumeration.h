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
#include "Member.h"

namespace Martta
{

class MemberEnumeration: public Member, public_interface EnumerationNamer
{
	MARTTA_OBJECT(Member)
	MARTTA_INHERITS(EnumerationNamer, 0)
	
public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);

protected:
	// From Member
	virtual Kinds						memberAllowedKinds(int _i) const;
	virtual int							memberMinimumRequired() const { return 2; }
	virtual QString						memberDefineLayout(const ViewKeys& _k) const { return EnumerationNamer::defineLayout(_k); }
	virtual QString						memberInterfaceCode() const { return EnumerationNamer::interfaceCode(); }
	virtual QString						memberImplementationCode() const { return QString::null; }
	
	// From TypeDefinition/EnumerationNamer
	virtual bool						hasDefaultConstructor() const { return EnumerationNamer::hasDefaultConstructor(); }
	virtual Types						assignableTypes() const { return Type(const_cast<MemberEnumeration*>(this)); }
	virtual QString						code() const { return codeName(); }
	virtual QList<DeclarationEntity*>	utilisedInUse() const { return QList<DeclarationEntity*>() << const_cast<MemberEnumeration*>(this); }
	
	// From DeclarationEntity
	virtual QList<ValueDefiner*>		valuesAdded() const { return EnumerationNamer::valuesAdded(); }

	// From Entity
	virtual int							familyDependencies() { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity* _e);
	virtual bool						keyPressed(EntityKeyEvent const* _e) { M_ASSERT(isComplete()); return EnumerationNamer::keyPressed(_e) ? true : Super::keyPressed(_e); }
	virtual Entity*						isExpander() const { return entity(1); }
};

}