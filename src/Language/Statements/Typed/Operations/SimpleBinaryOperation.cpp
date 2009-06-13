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

#include "TypeDefinition.h"
#include "Const.h"
#include "Reference.h"
#include "FundamentalOperator.h"
#include "BasicOperator.h"
#include "FunctionType.h"
#include "MethodOperator.h"
#include "SimpleBinaryOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(SimpleBinaryOperation);	

bool SimpleBinaryOperation::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	Operator o(_e->text(), Operator::Binary);
	if (!_p.exists() || _p->isPlaceholder() || o.isNull())
		return false;
		
	bool ok = false;
	InsertionPoint p = slideOnPrecedence(_p, o.precedence(), o.associativity(), _e->nearestBracket(_p));
	M_ASSERT(!p.entity()->isEditing());
	if (p->isKind<Typed>() && findOperators(o, p->asKind<Typed>()->type()).size())
		ok = true;
	if (!ok)
		p = _p;
	if (p->isKind<Typed>() && findOperators(o, p->asKind<Typed>()->type()).size())
		ok = true;
	if (ok)
	{
		SimpleBinaryOperation* n = new SimpleBinaryOperation(o, p->asKind<Typed>()->type());
		_e->noteStrobeCreation(n, &*p);
		p->insert(n);
		n->validifyChildren();
		n->dropCursor();
		return true;
	}
	return false;
}

QList<ValueDefiner*> SimpleBinaryOperation::findOperators(Operator _o, Type const& _left, Type const& _right)
{
	return findBestOverload((_left, _right), allOperators(_o.symbol()));
}

QString SimpleBinaryOperation::operatorLayout() const
{
	if (id().symbol() == Operator::BangEquals)
		return QString::fromWCharArray(L"ycode;fs+1;'=';M-5;'/';M-6;'='");
	else if (id().symbol() == Operator::EqualsEquals)
		return QString::fromWCharArray(L"ycode;fs+1;'=';M-3;'='");
	else if (id().symbol() == Operator::LessThanEquals)
		return QString("ycode;'").append(QChar(0x2264)) + "'";
	else if (id().symbol() == Operator::GreaterThanEquals)
		return QString("ycode;'").append(QChar(0x2265)) + "'";
	else if (id().symbol() == Operator::LeftShift)
		return QString::fromWCharArray(L"ycode;'<';M-4;'<'");
	else if (id().symbol() == Operator::RightShift)
		return QString::fromWCharArray(L"ycode;'>';M-4;'>'");
	else if (id().symbol() == Operator::LeftShiftEquals)
		return QString::fromWCharArray(L"ycode;'<';M-4;'<';M-3;'='");
	else if (id().symbol() == Operator::RightShiftEquals)
		return QString::fromWCharArray(L"ycode;'>';M-4;'>';M-3;'='");
	else if (id().symbol() == Operator::Star)
		return QString("ycode;'").append(QChar(0x00d7)) + "'";
	else if (id().symbol() == Operator::Slash)
		return QString("ycode;'").append(QChar(0x00f7)) + "'";
	else if (id().symbol() == Operator::BarBar)
		return QString("ycode;'").append(QChar(0x06f7)) + "'";
	else if (id().symbol() == Operator::AmpersAmpers)
		return QString("ycode;'").append(QChar(0x06f8)) + "'";
	else if (id().symbol() == Operator::PlusEquals)
		return QString::fromWCharArray(L"ycode;'+';M-3;'='");
	else if (id().symbol() == Operator::MinusEquals)
		return QString::fromWCharArray(L"ycode;'-';M-3;'='");
	else if (id().symbol() == Operator::StarEquals)
		return QString("ycode;'").append(QChar(0x00d7)) + "';M-3;'='";
	else if (id().symbol() == Operator::SlashEquals)
		return QString("ycode;'").append(QChar(0x00f7)) + "';M-3;'='";
	else
		return QString("ycode;'%1'").arg(id().code());
}

void SimpleBinaryOperation::setOperation(Operator _o, Type const& _left, Type const& _right)
{
	QList<ValueDefiner*> l = findOperators(_o, _left, _right);
	if (l.size() && (m_symbolCache != l[0] || m_operator != _o))
	{
		m_operator = _o;
		setDependency(m_symbolCache, l[0]);
	}
}

void SimpleBinaryOperation::onDependencyChanged(Entity*)
{
	refreshOperation();
}

QString SimpleBinaryOperation::code() const
{
	if (!haveLeft() || !haveRight() || !m_symbolCache.isUsable())
		return QString();
	return parenthesise(left()->code() + " " + id().code() + " " + right()->code());
}

Types SimpleBinaryOperation::allowedTypes(int _index) const
{
	if (m_symbolCache.isUsable() && (_index == 0 || _index == 1))
	{
		qDebug() << prototypeOf(0)->code() << prototypeOf(1)->code() << " " << _index << " " << typeOf(0)->code();
		if (_index == 1 && prototypeOf(1).isUltimatelyNull() && !typeOf(0).isNull())
			return typeOf(0).strippedTo(prototypeOf(1));
		if (_index == 1 && prototypeOf(1).isUltimatelyNull())
			return Types();
		M_ASSERT(!prototypeOf(_index).isNull());
		return prototypeOf(_index);
	}
	return Types();
}

Type SimpleBinaryOperation::type() const
{
	if (!m_symbolCache.isUsable())
		return Type();
	if (protoReturn().isUltimatelyNull() && !typeOf(0).isNull())
		return typeOf(0).strippedTo(protoReturn());
	else
		return protoReturn();
}

void SimpleBinaryOperation::importDom(QDomElement const& _element)
{
	Super::importDom(_element);
	m_operator = Operator((Operator::Symbol)_element.attribute("operator").toInt());
}

void SimpleBinaryOperation::exportDom(QDomElement& _element) const
{
	Super::exportDom(_element);
	_element.setAttribute("operator", (int)m_operator.symbol());
}

}
