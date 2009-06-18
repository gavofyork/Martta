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

namespace Martta
{

class UnaryOperation: public Operation
{
	MARTTA_PLACEHOLDER(Operation)

public:
	virtual int							minimumRequired() const { return 1; }
	virtual Kinds						allowedKinds(int _index) const { if (_index == 0) return Kind::of<Typed>(); else return Super::allowedKinds(_index); }
	virtual bool						isPostfix() const { return id().isPostfix(); }

protected:
	inline bool							haveOperand() const { return isTyped(0); }
	inline Typed*						operand() const { return asTyped(0); }
	inline Type							operandType() const { return typeOf(0); }
	inline Type							effectiveOperandType() const { return effectiveType(0); }

	virtual QString						operatorLayout() const { return "ycode;'" + id().code() + "'"; }
	virtual QString						defineLayout(ViewKeys&) const;

	virtual int							familyDependencies() const { return DependsOnBoth; }
	virtual void						onDependencySwitched(Entity* _e, Entity* _o) { if (_e == parent()) relayoutLater(); else Super::onDependencySwitched(_e, _o); }
	
	template<class T> static bool		simpleKeyPressedOnInsertionPointHandler(InsertionPoint const& _p, EntityKeyEvent const* _e, QString const& _t, Precedence _d, Associativity _a, bool _pre = true, bool _confusable = false)
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
		
		InsertionPoint p = pre ? _p : slideOnPrecedence(_p, _d, _a, _e->nearestBracket(_p));
		
		if (isTemporary(p.childType()))
			return false;
		
		Entity* n = new T;
		_e->noteStrobeCreation(n, &*p);
		p->insert(n);
		n->validifyChildren();
		if (pre)
			n->navigateInto(_e->codeScene());
		else
			n->setCurrent();
		return true;
	}
	template<class T> static bool		simpleKeyPressedOnInsertionPointHandler(InsertionPoint const& _p, EntityKeyEvent const* _e, Operator _o)
	{
		return simpleKeyPressedOnInsertionPointHandler<T>(_p, _e, _o.code(), _o.precedence(), _o.associativity(), _o.isPrefix(), _o.isConfusablePostfix());
	}
};

}
