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

#include "Subscriptable.h"
#include "ModifyingType.h"

namespace Martta
{

class BuiltinMethod;
class BuiltinOperator;

class HashType: public ModifyingType, public_interface Subscriptable
{
	MARTTA_INITIALISED_OBJECT(ModifyingType)
	MARTTA_INHERITS(Subscriptable, 0)

	friend class BuiltinMethod;
	friend class BuiltinOperator;

public:
	enum { KeyType = FirstNamed, EndOfNamed };
	enum { ValueType = Original };

	static void							initialiseClass();
	static void							finaliseClass();
	TypeEntity const*					key() const { return childIs<TypeEntity>(KeyType) ? childAs<TypeEntity>(KeyType) : TypeEntity::null; }
	TypeEntity const*					value() const { return childIs<TypeEntity>(ValueType) ? childAs<TypeEntity>(ValueType) : TypeEntity::null; }
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);
	virtual bool						isWellDefined() const { return key() && value() && key()->isWellDefined() && value()->isWellDefined(); }
	
protected:
	virtual bool						hasDefaultConstructor() const { return true; }
	virtual Types						assignableTypes() const;
	virtual List<ValueDefiner*>			applicableMembers(Entity const* _s = 0, bool _isConst = false) const;
	
	virtual int							minRequired(int _i) const { return _i == KeyType ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual TypeEntity*					newClone() const { return new HashType; }
	virtual String						code(String const& _middle) const;
	virtual String						defineLayout(ViewKeys const&) const;
	virtual bool						defineSimilarityTo(TypeEntity const* _t, Castability _c) const;
	virtual Rgb							idColour() const { return 0xbbff77; }
	virtual Types						subscriptTypes() const;
	virtual Type						subscriptsTo(Type const&) const;
	
	static List<BuiltinMethod*>			s_members;	
	static List<BuiltinOperator*>		s_nonMembers;	
};

}
