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

class M_API_Statements UnaryOperation: public_super Operation
{
	MARTTA_PLACEHOLDER(Operation)

public:
	virtual int							minRequired(int _i) const { return _i == TheOperand ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const { return _i == TheOperand ? Kinds() << Kind::of<Typed>() : Super::allowedKinds(_i); }
	virtual bool						isPostfix() const { return id().isPostfix(); }
	virtual bool						isSlidable(int _i) const { return _i != TheOperand; }

protected:
	inline bool							haveOperand() const { return isTyped(TheOperand); }
	inline Typed*						operand() const { return asTyped(TheOperand); }
	inline Type							operandType() const { return typeOf(TheOperand); }
	inline Type							effectiveOperandType() const { return effectiveType(TheOperand); }

	virtual String						operatorHtml() const { return L"<span class=\"symbol\">" + String(id().code()) + L"</span>"; }
	virtual String						defineHtml() const;

	virtual int							familyDependencies() const { return DependsOnBoth; }
//	virtual void						onDependencySwitched(Concept* _e, Concept* _o) { if (_e == parent()) markDirty(); else Super::onDependencySwitched(_e, _o); }

	template<class T> static bool		simpleKeyPressedOnPositionHandler(Position const& _p, KeyEvent const* _e, String const& _t, Precedence _d, Associativity _a, bool _pre = true, bool _confusable = false)
	{
		if (!_p.exists() || _e->text() != _t)
			return false;

		bool pre;
		if (_pre && (_p->isPlaceholder()/* || _e->isInserting()*/))
			pre = true;
		else if (!_pre && (!_confusable || !_p->isPlaceholder()))
			pre = false;
		else
			return false;

		Position p = pre ? _p : slideOnPrecedence(_p, _d, _a, _e->nearestBracket(_p));

		if (isTemporary(p.entity()))
			return false;

		Concept* n = new T;
		_e->noteStrobeCreation(n, &*p);
		p->insert(n, TheOperand);
		n->validifyChildren();
		if (pre)
			_e->codeScene()->navigateInto(n);
		else
			_e->codeScene()->setCurrent(n);
		return true;
	}
	template<class T> static bool		simpleKeyPressedOnPositionHandler(Position const& _p, KeyEvent const* _e, Operator _o)
	{
		return simpleKeyPressedOnPositionHandler<T>(_p, _e, _o.code(), _o.precedence(), _o.associativity(), _o.isPrefix(), _o.isConfusablePostfix());
	}
};

}
