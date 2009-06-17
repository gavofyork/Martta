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

class BinaryOperation: public Operation
{
	MARTTA_PLACEHOLDER(Operation)

public:
	virtual int							minimumRequired() const { return 2; }
	virtual Kinds						allowedKinds(int _index) const { if (_index < 2) return Kind::of<Typed>(); else return Super::allowedKinds(_index); }

protected:
	bool								haveLeft() const { return isTyped(0); }
	bool								haveRight() const { return isTyped(1); }
	Typed*								left() const { return asTyped(0); }
	Typed*								right() const { return asTyped(1); }
	Type								leftType() const { return typeOf(0); }
	Type								rightType() const { return typeOf(1); }

	template<class T> static bool		simpleKeyPressedOnInsertionPointHandler(InsertionPoint const& _p, EntityKeyEvent const* _e, QString const& _t, Precedence _d, Associativity _a)
	{
		if (!_p.exists() || _p->isPlaceholder() || _e->text() != _t)
			return false;

		InsertionPoint p = slideOnPrecedence(_p, _d, _a, _e->nearestBracket(_p));
		if (!isTemporary(p.entity()))
		{
			Entity* n = new T;
			_e->noteStrobeCreation(n, &*p);
			p->insert(n);
			n->validifyChildren();
			n->entity(1)->navigateOnto(_e->codeScene());
			return true;
		}
		return false;
	}
	template<class T> static bool		simpleKeyPressedOnInsertionPointHandler(InsertionPoint const& _p, EntityKeyEvent const* _e, Operator _o)
	{
		return simpleKeyPressedOnInsertionPointHandler<T>(_p, _e, _o.code(), _o.precedence(), _o.associativity());
	}
	
	virtual QString						operatorLayout() const { return "ycode;'" + id().code() + "'"; }
	virtual QString						defineLayout(ViewKeys&) const;

	virtual int							familyDependencies() const { return DependsOnBoth; }
	virtual void						onDependencySwitched(Entity* _e, Entity* _o) { if (_e == context()) relayoutLater(); else Super::onDependencySwitched(_e, _o); }
};

}
