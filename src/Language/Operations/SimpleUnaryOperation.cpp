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

#include <QtXml>

#include "Const.h"
#include "Reference.h"
#include "TypeDefinition.h"
#include "BasicOperator.h"
#include "MethodOperator.h"
#include "FundamentalOperator.h"
#include "SimpleUnaryOperation.h"

#include "Referenced.h"

namespace Martta
{

MARTTA_OBJECT_CPP(SimpleUnaryOperation);	

bool SimpleUnaryOperation::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
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
	InsertionPoint p = slideOnPrecedence(_p, o.precedence(), o.associativity(), _e->nearestBracket(_p));
	if (findOperators(o, p->isKind<Typed>() ? p->asKind<Typed>()->type() : Type()).size() && !isTemporary(p.entity()))
	{
		SimpleUnaryOperation* n = new SimpleUnaryOperation(o, p->isKind<Typed>() ? p->asKind<Typed>()->type() : Type());
//		p->debugTree();
		_e->noteStrobeCreation(n, &*p);
		p->insert(n);
//		n->debugTree();
		n->validifyChildren();
//		n->debugTree();
		n->dropCursor();
		return true;
	}
	return false;
}

QList<ValueDefinition*> SimpleUnaryOperation::findOperators(Operator _o, Type const& _type)
{
	return findBestOverload((_type), allOperators(_o.symbol()));
}

void SimpleUnaryOperation::setOperation(Operator _o, Type const& _type)
{
	m_operator = _o;
	QList<ValueDefinition*> l = findOperators(_o, _type);
	if (l.size())
		SET_DEPENDENCY(m_symbolCache, l[0]);
}

Type SimpleUnaryOperation::type() const
{
	if (!m_symbolCache.isUsable())
		return Type();
	if (protoReturn().isUltimatelyNull() && !typeOf(0).isNull())
	{
/*		qDebug() << typeOf(0)->code();
		typeOf(0)->debugTree();
		qDebug() << protoReturn()->code();
		protoReturn()->debugTree();
		qDebug() << typeOf(0).strippedTo(protoReturn())->code();
		typeOf(0).strippedTo(protoReturn())->debugTree();*/
		return typeOf(0).strippedTo(protoReturn());
	}
	return protoReturn();
}

QString SimpleUnaryOperation::code() const
{
	if (!haveOperand() || !m_symbolCache.isUsable())
		return QString();
	if (isPostfix())
		return parenthesise(operand()->code() + id().code());
	else
		return parenthesise(id().code() + operand()->code());
}

Types SimpleUnaryOperation::allowedTypes(int _index) const
{
	return prototypeOf(_index);
}

QString SimpleUnaryOperation::operatorLayout() const
{
	if (id().symbol() == Operator::PlusPlusX || id().symbol() == Operator::XPlusPlus)
		return QString::fromWCharArray(L"ycode;'+';M-3;'+'");
	else if (id().symbol() == Operator::MinusMinusX || id().symbol() == Operator::XMinusMinus)
		return QString::fromWCharArray(L"ycode;'-';M-1;'-'");
	else
		return "ycode;'" + id().code() + "'";
}

void SimpleUnaryOperation::importDom(QDomElement const& _element)
{
	Super::importDom(_element);
	m_operator = Operator((Operator::Symbol)_element.attribute("operator").toInt());
}

void SimpleUnaryOperation::exportDom(QDomElement& _element) const
{
	Super::exportDom(_element);
	_element.setAttribute("operator", (int)m_operator.symbol());
}

}
