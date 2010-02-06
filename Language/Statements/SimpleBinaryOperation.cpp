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

#include "TypeDefinition.h"
#include "Const.h"
#include "Reference.h"
#include "FunctionType.h"
#include "WebStylist.h"
#include "ValueDefiner.h"
#include "SimpleBinaryOperation.h"

namespace Martta
{

MARTTA_PROPER_CPP(SimpleBinaryOperation);

bool SimpleBinaryOperation::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	Operator o(_e->text(), Operator::Binary);
	if (!_p.exists() || _p->isPlaceholder() || o.isNull())
		return false;

	Position p = slideOnPrecedence(_p, o.precedence(), o.associativity(), _e->nearestBracket(_p));
	AssertNR(!p.concept()->isEditing());

	// Try condition for p, then _p if p fails, then bail if _p fails. p is the one that succeeded first.
	for (; !(p->isKind<Typed>() && findOperators(o, p->asKind<Typed>()->apparentType()).size()); p = _p)
		if (p == _p)
			return false;

	SimpleBinaryOperation* n = new SimpleBinaryOperation(o, p->asKind<Typed>()->apparentType());
	_e->noteStrobeCreation(n, &*p);
	p->insert(n, FirstOperand);
	n->validifyChildren();
	_e->codeScene()->dropCursor(n);
	return true;
}

List<ValueDefiner*> SimpleBinaryOperation::findOperators(Operator _o, Type const& _left, Type const& _right)
{
	return findBestOverload((_left, _right), allOperators(_o.symbol()));
}

String SimpleBinaryOperation::operatorHtml() const
{
	bool s = !WebStylist::current()->property("CSS", "Simple").toBool();
	if (s && id().symbol() == Operator::BangEquals)
		return L"<span class=\"symbol\">=<span style=\"margin-left:-0.4em\">/</span><span style=\"margin-left:-0.4em\">=</span></span>";
	else if (s && id().symbol() == Operator::EqualsEquals)
		return L"<span class=\"symbol\">=<span style=\"margin-left:-0.27em\">=</span></span>";
	else if (s && id().symbol() == Operator::LessThanEquals)
		return L"<span class=\"symbol\">&le;</span>";
	else if (s && id().symbol() == Operator::GreaterThanEquals)
		return L"<span class=\"symbol\">&ge;</span>";
	else if (id().symbol() == Operator::LessThanEquals)
		return L"<span class=\"symbol\">&lt;=</span>";
	else if (id().symbol() == Operator::GreaterThanEquals)
		return L"<span class=\"symbol\">&gt;=</span>";
	else if (id().symbol() == Operator::LessThan)
		return L"<span class=\"symbol\">&lt;</span>";
	else if (id().symbol() == Operator::GreaterThan)
		return L"<span class=\"symbol\">&gt;</span>";
	else if (s && id().symbol() == Operator::LeftShift)
		return L"<span class=\"symbol\">&lt;&lt;</span>";
	else if (id().symbol() == Operator::LeftShift)
		return L"<span class=\"symbol\">&laquo;</span>";
	else if (s && id().symbol() == Operator::RightShift)
		return L"<span class=\"symbol\">&raquo;</span>";
	else if (id().symbol() == Operator::RightShift)
		return L"<span class=\"symbol\">&gt;&gt;</span>";
	else if (s && id().symbol() == Operator::LeftShiftEquals)
		return L"<span class=\"symbol\">&laquo;=</span>";
	else if (s && id().symbol() == Operator::RightShiftEquals)
		return L"<span class=\"symbol\">&raquo;=</span>";
	else if (id().symbol() == Operator::LeftShiftEquals)
		return L"<span class=\"symbol\">&lt;&lt;=</span>";
	else if (id().symbol() == Operator::RightShiftEquals)
		return L"<span class=\"symbol\">&gt;&gt;=</span>";
	else if (s && id().symbol() == Operator::Star)
		return L"<span class=\"symbol\">&times;</span>";
	else if (s && id().symbol() == Operator::Slash)
		return L"<span class=\"symbol\">&divide;</span>";
	else if (s && id().symbol() == Operator::BarBar)
		return L"<span class=\"symbol\">&or;</span>";
	else if (s && id().symbol() == Operator::AmpersAmpers)
		return L"<span class=\"symbol\">&and;</span>";
	else if (s && id().symbol() == Operator::PlusEquals)
		return L"<span class=\"symbol\">+=</span>";
	else if (s && id().symbol() == Operator::MinusEquals)
		return L"<span class=\"symbol\">-=</span>";
	else if (s && id().symbol() == Operator::StarEquals)
		return L"<span class=\"symbol\">&times;=</span>";
	else if (s && id().symbol() == Operator::SlashEquals)
		return L"<span class=\"symbol\">&divide;=</span>";
	else
		return String(L"<span class=\"symbol\">%1</span>").arg(id().code());
}

String SimpleBinaryOperation::defineHtml() const
{
	if (id().symbol() == Operator::Slash && WebStylist::current()->property("SimpleBinaryOperation", "ProperMaths").toBool())
	{
		String ret = displayParenthesise(L"<^><div style=\"display: inline-block; vertical-align: middle; text-align:center\">%1<hr style=\"margin:0; margin-top:1px; padding:0; border:0; border-bottom: 2px solid #666;\"/>%2</div>");
		return ret	.arg(toHtml(child(FirstOperand), childIs<Operation>(FirstOperand) ? L"class=\"Operation\"" : L""))
					.arg(toHtml(child(SecondOperand), childIs<Operation>(SecondOperand) ? L"class=\"Operation\"" : L""));
	}
	else
		return Super::defineHtml();
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

void SimpleBinaryOperation::onDependencyChanged(int, Concept*)
{
	refreshOperation();
}

String SimpleBinaryOperation::code() const
{
	if (!haveLeft() || !haveRight() || !m_symbolCache)
		return String::null;
	return parenthesise(left()->code() + " " + id().code() + " " + right()->code());
}

Types SimpleBinaryOperation::allowedTypes(int _index) const
{
	if (_index == FirstOperand || _index == SecondOperand)
	{
		if (!m_symbolCache)
			return Types();
//		mDebug() << prototypeOf(FirstOperand)->code() << prototypeOf(SecondOperand)->code() << " " << _index << " " << typeOf(FirstOperand)->code();
		if (_index == SecondOperand && prototypeOf(SecondOperand).isUltimatelyNull() && !typeOf(FirstOperand).isNull())
			return typeOf(FirstOperand).strippedTo(prototypeOf(SecondOperand));
		if (_index == SecondOperand && prototypeOf(SecondOperand).isUltimatelyNull())
			return Types();
		AssertNR(!prototypeOf(_index).isNull());
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

}
