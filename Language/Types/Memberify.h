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

#include "Const.h"
#include "ExplicitType.h"
#include "ModifyingType.h"

namespace Martta
{

class Memberify: public ModifyingType
{
	MARTTA_OBJECT(ModifyingType)

public:
	enum { Scope = FirstNamed, EndOfNamed };

	inline Memberify() {}
	Memberify(TypeDefinition* _scope, bool _isConst = true);
	Memberify(Type const& _object);

	bool								isConst() const;
	void								setConst(bool _c);
	TypeEntity*							scope() const { return tryChild<TypeEntity>(Scope); }
	TypeEntity*							scopeType() const;
	template<class T> T*				scope(bool* _isConst = 0) const;
	void								setScope(Type const& _newScope);
	void								setScopeClass(TypeDefinition* _scope, bool _isConst = false);
	virtual String						code(String const& _middle) const;
	virtual bool						isWellDefined() const { return Super::isWellDefined() && scope(); }

protected:
	virtual Types						assignableTypes() const;
	virtual inline TypeEntity*			newClone() const { return new Memberify; }
	virtual String						modifierHtml() const;
	virtual bool						isSuperfluous() const { return !childIs<TypeEntity>(Scope) || Super::isSuperfluous(); }
	virtual bool						canStandAlone() const { return false; }
	virtual bool						defineSimilarityFrom(TypeEntity const* _f, Castability _c) const;
	virtual Kinds						allowedKinds(int _i) const;
};

}

template<class T>
T* Martta::Memberify::scope(bool* _isConst) const
{
	TypeEntity* te = scope();
	if (!te)
		return 0;
	if (ExplicitType* et = te->ignore<Const>()->tryType<ExplicitType>())
	{
		if (_isConst)
			*_isConst = te->isType<Const>();
		return et->subject()->tryKind<T>();
	}
	return 0;
}


