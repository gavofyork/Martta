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

#include <msString.h>
using MarttaSupport::Char;

#include "TypeDefinition.h"
#include "Const.h"
#include "Reference.h"
#include "FunctionType.h"
#include "MethodOperator.h"
#include "SimpleBinaryOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(SimpleBinaryOperation);	

bool SimpleBinaryOperation::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	Operator o(_e->text(), Operator::Binary);
	if (!_p.exists() || _p->isPlaceholder() || o.isNull())
		return false;
		
	bool ok = false;
	Position p = slideOnPrecedence(_p, o.precedence(), o.associativity(), _e->nearestBracket(_p));
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
		p->insert(n, FirstOperand);
		n->validifyChildren();
		n->dropCursor();
		return true;
	}
	return false;
}

List<ValueDefiner*> SimpleBinaryOperation::findOperators(Operator _o, Type const& _left, Type const& _right)
{
	return findBestOverload((_left, _right), allOperators(_o.symbol()));
}

String SimpleBinaryOperation::operatorLayout() const
{
	if (id().symbol() == Operator::BangEquals)
		return L"ycode;fs+1;'=';M-5;'/';M-6;'='";
	else if (id().symbol() == Operator::EqualsEquals)
		return L"ycode;fs+1;'=';M-3;'='";
	else if (id().symbol() == Operator::LessThanEquals)
		return String(L"ycode;'%1'").arg(MarttaSupport::Char(0x2264));
	else if (id().symbol() == Operator::GreaterThanEquals)
		return String(L"ycode;'%1'").arg(MarttaSupport::Char(0x2265));
	else if (id().symbol() == Operator::LeftShift)
		return L"ycode;'<';M-4;'<'";
	else if (id().symbol() == Operator::RightShift)
		return L"ycode;'>';M-4;'>'";
	else if (id().symbol() == Operator::LeftShiftEquals)
		return L"ycode;'<';M-4;'<';M-3;'='";
	else if (id().symbol() == Operator::RightShiftEquals)
		return L"ycode;'>';M-4;'>';M-3;'='";
	else if (id().symbol() == Operator::Star)
		return String(L"ycode;'%1'").arg(MarttaSupport::Char(0x00d7));
	else if (id().symbol() == Operator::Slash)
		return String(L"ycode;'%1'").arg(MarttaSupport::Char(0x00f7));
	else if (id().symbol() == Operator::BarBar)
		return String(L"ycode;'%1'").arg(MarttaSupport::Char(0x06f7));
	else if (id().symbol() == Operator::AmpersAmpers)
		return String(L"ycode;'%1'").arg(MarttaSupport::Char(0x06f8));
	else if (id().symbol() == Operator::PlusEquals)
		return L"ycode;'+';M-3;'='";
	else if (id().symbol() == Operator::MinusEquals)
		return L"ycode;'-';M-3;'='";
	else if (id().symbol() == Operator::StarEquals)
		return String(L"ycode;'%1';M-3;'='").arg(MarttaSupport::Char(0x00d7));
	else if (id().symbol() == Operator::SlashEquals)
		return String(L"ycode;'%1';M-3;'='").arg(MarttaSupport::Char(0x00f7));
	else
		return String(L"ycode;'%1'").arg(id().code());
}

void SimpleBinaryOperation::setOperation(Operator _o, Type const& _left, Type const& _right)
{
	List<ValueDefiner*> l = findOperators(_o, _left, _right);
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

String SimpleBinaryOperation::code() const
{
	if (!haveLeft() || !haveRight() || !m_symbolCache)
		return String();
	return parenthesise(left()->code() + " " + id().code() + " " + right()->code());
}

Types SimpleBinaryOperation::allowedTypes(int _index) const
{
	if (_index == FirstOperand || _index == SecondOperand)
	{
		if (!m_symbolCache)
			return Types();
		mDebug() << prototypeOf(FirstOperand)->code() << prototypeOf(SecondOperand)->code() << " " << _index << " " << typeOf(FirstOperand)->code();
		if (_index == SecondOperand && prototypeOf(SecondOperand).isUltimatelyNull() && !typeOf(FirstOperand).isNull())
			return typeOf(FirstOperand).strippedTo(prototypeOf(SecondOperand));
		if (_index == SecondOperand && prototypeOf(SecondOperand).isUltimatelyNull())
			return Types();
		M_ASSERT(!prototypeOf(_index).isNull());
		return prototypeOf(_index);
	}
	return Super::allowedTypes(_index);
}

Type SimpleBinaryOperation::type() const
{
	if (!m_symbolCache)
		return Type();
	if (protoReturn().isUltimatelyNull() && !typeOf(FirstOperand).isNull())
		return typeOf(FirstOperand).strippedTo(protoReturn());
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
