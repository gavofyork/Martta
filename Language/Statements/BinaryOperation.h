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

#include "Operation.h"

#ifndef M_API_Statements
#define M_API_Statements M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Statements BinaryOperation: public_super Operation
{
	MARTTA_PLACEHOLDER(Operation)

public:
	virtual int							minRequired(int _i) const { return _i == FirstOperand || _i == SecondOperand ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const { if (_i == FirstOperand || _i == SecondOperand) return Kind::of<Typed>(); else return Super::allowedKinds(_i); }
	virtual bool						isSlidable(int _i) const { return _i != SecondOperand; }

protected:
	bool								haveLeft() const { return isTyped(FirstOperand); }
	bool								haveRight() const { return isTyped(SecondOperand); }
	Typed*								left() const { return asTyped(FirstOperand); }
	Typed*								right() const { return asTyped(SecondOperand); }
	Type								leftType() const { return typeOf(FirstOperand); }
	Type								rightType() const { return typeOf(SecondOperand); }

	template<class T> static bool		simpleKeyPressedOnPositionHandler(Position const& _p, KeyEvent const* _e, String const& _t, Precedence _d, Associativity _a)
	{
		if (!_p.exists() || _p->isPlaceholder() || _e->text() != _t)
			return false;
		Position p = slideOnPrecedence(_p, _d, _a, _e->nearestBracket(_p));

		if (isTemporary(p.concept()) || !p->isKind<Typed>())
			return false;

		Concept* n = new T;
		_e->noteStrobeCreation(n, &*p);
		p->insert(n, FirstOperand);
		n->validifyChildren();
		_e->codeScene()->dropCursor(n);
		return true;
	}
	template<class T> static bool		simpleKeyPressedOnPositionHandler(Position const& _p, KeyEvent const* _e, Operator _o)
	{
		return simpleKeyPressedOnPositionHandler<T>(_p, _e, _o.code(), _o.precedence(), _o.associativity());
	}

	virtual String						operatorHtml() const { return tagOf(L"symbol", id().code()); }
	virtual String						defineHtml() const;

	virtual bool						isSuperfluous() const { return left()->isPlaceholder() || right()->isPlaceholder(); }
	virtual Concept*					onDecay() const { return left()->isPlaceholder() ? right()->isPlaceholder() ? 0 : right() : left(); }

	virtual int							familyDependencies() const { return DependsOnBoth; }
//	virtual void						onDependencySwitched(Concept* _e, Concept* _o) { if (_e == parent()) markDirty(); else Super::onDependencySwitched(_e, _o); }
};

}
