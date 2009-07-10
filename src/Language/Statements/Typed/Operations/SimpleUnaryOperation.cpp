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

#include "Const.h"
#include "Reference.h"
#include "TypeDefinition.h"
#include "ValueDefiner.h"
#include "SimpleUnaryOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(SimpleUnaryOperation);	

bool SimpleUnaryOperation::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (!_p.exists())
		return false;
	
	bool pre;
	if (!Operator(_e->text(), Operator::UnaryPrefix).isNull() && (_p->isPlaceholder()/* || _e->isInserting()*/))
		pre = true;
	else if (!Operator(_e->text(), Operator::UnaryPostfix).isNull() && (!Operator(_e->text(), Operator::UnaryPrefix).isNull() || !_p->isPlaceholder()))
		pre = false;
	else
		return false;
	
	Operator o(_e->text(), pre ? Operator::UnaryPrefix : Operator::UnaryPostfix);
	Position p = slideOnPrecedence(_p, o.precedence(), o.associativity(), _e->nearestBracket(_p));
	bool b = findOperators(o, p->isKind<Typed>() ? p->asKind<Typed>()->apparentType() : Type()).size();
	if (b && !isTemporary(p.entity()))
	{
		SimpleUnaryOperation* n = new SimpleUnaryOperation(o, p->isKind<Typed>() ? p->asKind<Typed>()->apparentType() : Type());
		_e->noteStrobeCreation(n, &*p);
		p->insert(n, TheOperand);
		n->validifyChildren();
		n->dropCursor();
		return true;
	}
	return false;
}

List<ValueDefiner*> SimpleUnaryOperation::findOperators(Operator _o, Type const& _type)
{
	return findBestOverload((_type), allOperators(_o.symbol()));
}

void SimpleUnaryOperation::setOperation(Operator _o, Type const& _type)
{
	List<ValueDefiner*> l = findOperators(_o, _type);
	if (l.size() && (m_symbolCache != l[0] || m_operator != _o))
	{
		m_operator = _o;
		setDependency(m_symbolCache, l[0]);
	}
}

Type SimpleUnaryOperation::type() const
{
	if (!m_symbolCache)
		return Type();
	if (protoReturn().isUltimatelyNull() && !typeOf(TheOperand).isNull())
	{
/*		mDebug() << typeOf(TheOperand)->code();
		typeOf(TheOperand)->debugTree();
		mDebug() << protoReturn()->code();
		protoReturn()->debugTree();
		mDebug() << typeOf(TheOperand).strippedTo(protoReturn())->code();
		typeOf(TheOperand).strippedTo(protoReturn())->debugTree();*/
		return typeOf(TheOperand).strippedTo(protoReturn());
	}
	return protoReturn();
}

String SimpleUnaryOperation::code() const
{
	if (!haveOperand() || !m_symbolCache)
		return String();
	if (isPostfix())
		return parenthesise(operand()->code() + id().code());
	else
		return parenthesise(id().code() + operand()->code());
}

Types SimpleUnaryOperation::allowedTypes(int _index) const
{
	if (_index == TheOperand)
		return prototypeOf(_index);
	return Super::allowedTypes(_index);
}

String SimpleUnaryOperation::operatorLayout() const
{
	if (id().symbol() == Operator::PlusPlusX || id().symbol() == Operator::XPlusPlus)
		return L"ycode;'+';M-3;'+'";
	else if (id().symbol() == Operator::MinusMinusX || id().symbol() == Operator::XMinusMinus)
		return L"ycode;'-';M-1;'-'";
	else
		return String("ycode;'%1'").arg(id().code());
}

}
