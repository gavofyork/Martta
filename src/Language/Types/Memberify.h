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

#include "ModifyingType.h"
#include "Class.h"

namespace Martta
{

class Class;

class Memberify: public ModifyingType
{
	MARTTA_OBJECT(ModifyingType)

public:
	enum { Scoped = FirstNamed, EndOfNamed };

	Memberify(Class* _scope = 0, bool _isConst = true) { if (_scope) setScopeClass(_scope, _isConst); }
	Memberify(Type const& _object);
//	~Memberify() { if (child(1)) child(1)->killAndDelete(); }
	
	bool								isConst() const;
	void								setConst(bool _c);
	TypeEntity*							scope() const { return child(1)->isKind<TypeEntity>() ? child(1)->asKind<TypeEntity>() : 0; }
	TypeEntity*							scopeType() const;
	Class*								scopeClass(bool* _isConst = 0) const;
	void								setScope(Type const& _newScope);
	void								setScopeClass(Class* _scope, bool _isConst = false);
	virtual QString						code(QString const& _middle) const;
	virtual bool						isWellDefined() const { return Super::isWellDefined() && scope(); }
	
protected:
	virtual Types						assignableTypes() const;
	virtual TypeEntity*					newClone() const;
	virtual QString						modifierLayout() const;
	virtual bool						isSuperfluous() const { return cardinalChildCount() < 2 || Super::isSuperfluous(); }
	virtual bool						canStandAlone() const { return false; }
	virtual bool						defineSimilarityFrom(TypeEntity const* _f, Castability _c) const;
	virtual Kinds						allowedKinds(int _i) const;
};

}
