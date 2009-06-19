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

#include "msSupport.h"
#include "CodeScene.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Memberify.h"
#include "ValueDefiner.h"
#include "Operation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Operation);

QMultiHash<Operator, ValueDefiner*> Operation::s_operatorCatalogue;

Type Operation::prototypeOf(Type const& _t, int _index)
{
	// Check if it's declared in a class (first param is encoded as this if so).
	if (_t->isType<FunctionType>())
	{
		if (_index == FirstOperand)
			return _t->asType<FunctionType>()->argumentType(0);
		else if (_index == SecondOperand)
			return _t->asType<FunctionType>()->argumentType(1);
		else if (_index == UndefinedIndex)
			return _t->asType<FunctionType>()->returnType();
	}
	else if (_t->isType<Memberify>() && _t->asType<Memberify>()->childType()->isKind<FunctionType>())
	{
		if (_index == FirstOperand)
			return Type(*_t->asType<Memberify>()->scope()).topWith(Reference());
		else if (_index == SecondOperand)
			return _t->asType<Memberify>()->childType()->asKind<FunctionType>()->argumentType(0);
		else if (_index == UndefinedIndex)
			return _t->asType<Memberify>()->childType()->asKind<FunctionType>()->returnType();
	}
	return Type();
}

bool Operation::prototypeHasArgumentAt(Type const& _t, int _cardinal)
{
	// Check if it's declared in a class (first param is encoded as this if so).
	if (_t->isType<FunctionType>())
		return _t->asType<FunctionType>()->hasArgumentAt(_cardinal);
	else if (_t->isType<Memberify>() && _t->asType<Memberify>()->childType()->isKind<FunctionType>())
		return _t->asType<Memberify>()->childType()->asKind<FunctionType>()->hasArgumentAt(_cardinal - 1);
	return false;
}

QList<ValueDefiner*> Operation::findBestOverload(Types const& _actual, QList<ValueDefiner*> const _candidates)
{
//	qDebug() << "Finding best overload for" << _actual;
	QList<ValueDefiner*> ret;
	// Go through each one and give a score, then return the best one(s)...
	int bestScore = 0;
	foreach (ValueDefiner* i, _candidates)
	{
		Type c = i->type();
		
//		qDebug() << "Checking" << c;
		
		if (!prototypeHasArgumentAt(c, _actual.count() - 1))
			continue;
			
		// Go through each type and give a score
		int score = 0;
		int j = FirstOperand;
		foreach (Type f, _actual)
		{
			// No point matching against a null value.
			if (f.isNull())
				continue;
			
			Type t = prototypeOf(c, j);
			// Compare f (actual parameter type) to t (the overload's parameter type)
			
			for (int k = TypeEntity::MostConvertible; k >= TypeEntity::Convertible; k -= 1 << MarttaSupport::floorLog2(k))
				if (f->isSimilarTo(&*t, k))
				{
					score += k;
					goto OK;
				}
			// exit early if it really doeesn't match
			score = -1;
			break;
			OK:
			j++;
//			qDebug() << "Similarity of parameter" << j << "(" << t << ") is" << score;
		}
		
		if (score > bestScore)
		{
			ret.clear();
			ret += i;
			bestScore = score;
		}
		else if (score >= bestScore)
			ret += i;
	}
	return ret;
}

bool Operation::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (_e->text() == "(" && _p->isPlaceholder())
	{
		_e->codeScene()->setBracketed(_p);
		_e->codeScene()->update();
		return true;
	}
	return false;
}

bool Operation::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == ")" && _e->codeScene()->isBracketed(over()))
	{
		_e->codeScene()->removeBracket(over());
		over()->setCurrent();
		repaint(_e->codeScene());
		return true;
	}
	else if(_e->text() == ")")
	{
		_e->codeScene()->setBracketed(over());
		over()->setCurrent();
		repaint(_e->codeScene());
		return true;
	}
	
	return Super::keyPressed(_e);
}

InsertionPoint Operation::slideOnPrecedence(InsertionPoint _p, Precedence _d, Associativity _a, InsertionPoint const& _block)
{
	InsertionPoint p = _p;
	while (_block != p && p->parentIs<Operation>() && !p->parentAs<Operation>()->isSlidable(p.index()) &&
		   (_d > p->parentAs<Operation>()->precedence() || _d == p->parentAs<Operation>()->precedence() && _a == LeftAssociativity))
		p = p.parent()->over();
	while (_block != p && p->isKind<Operation>() && !p->asKind<Operation>()->lastOperand()->isPlaceholder() && p->asKind<Operation>()->precedence() == _d && _a == RightAssociativity)
		p = p->asKind<Operation>()->lastOperand()->over();
	if (p->parentIs<Operation>() && p->parentAs<Operation>()->isSlidable(p.index()))
		return _p;
	return p;
}

QList<ValueDefiner*> Operation::allOperators(Operator _o)
{
	// TODO: Should use rootEntity() here to index, but doesn't yet; no a big issue until there are multiple RootEntities at once.
	QList<ValueDefiner*> ret = s_operatorCatalogue.values(_o);
	// TODO: Search classes etc.
	return ret;
}

}
