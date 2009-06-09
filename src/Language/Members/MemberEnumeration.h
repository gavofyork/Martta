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

#include "Type.h"
#include "Enumeration.h"
#include "TypeDefinition.h"
#include "Member.h"

namespace Martta
{

class MemberEnumeration: public Member, public_interface TypeDefinition
{
	MARTTA_OBJECT(Member)
	MARTTA_INHERITS(TypeDefinition, 0)
	
public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);

protected:
	virtual bool						usurpsChild(Entity const* _e) const { return _e == local(0); }

	virtual Kinds						memberAllowedKinds(int _i) const { if (_i == 0) return Kind::of<Enumeration>(); else return Kinds(); }
	
	virtual QString						code() const { return localAs<TypeDefinition>(0)->code(); }
	virtual bool						hasDefaultConstructor() const { return localAs<TypeDefinition>(0)->hasDefaultConstructor(); }
	virtual Types						assignableTypes() const { return localAs<TypeDefinition>(0)->assignableTypes(); }
	virtual QList<DeclarationEntity*>	utilisedInUse() const { return localAs<TypeDefinition>(0)->utilisedInUse(); }
};

}
