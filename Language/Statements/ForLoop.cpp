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

#include "IdentifierSet.h"
#include "Primary.h"
#include "BareTyped.h"
#include "Typed.h"
#include "Const.h"
#include "BuiltinType.h"
#include "ForLoop.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ForLoop);

static SimpleIdentifierSet<ForLoop> s_forLoopSet(L"for");

Kinds ForLoop::allowedKinds(int _index) const
{
	if (_index == Initialiser)
		return Kind::of<Primary>();
	if (_index == Condition)
		return Kind::of<BareTyped>();
	if (_index == Ticker)
		return Kind::of<Typed>();
	return Super::allowedKinds(_index);
}

Types ForLoop::allowedTypes(int _index) const
{
	if (_index == Condition)
		return Type(Void).topWith(Const());
	if (_index == Initialiser)
		return Type(Bool).topWith(Const());
	if (_index == Ticker)
		return Type();
	return Super::allowedTypes(_index);
}

bool ForLoop::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<ForLoop>(_p, _e, "F");
}

bool ForLoop::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == ";" && _e->focalIndex() == Initialiser)
		child(Condition)->setCurrent();
	else if (_e->text() == ";" && _e->focalIndex() == Condition)
		child(Ticker)->setCurrent();
	else
		return Super::keyPressed(_e);
	return true;
}

String ForLoop::defineLayout(ViewKeys const& _k) const
{
	return ("Hfull;ycode;^;'for (';%1;'\\; ';%2;'\\; ';%3;')'" + Corporal::defineLayout(_k, true)).arg(Initialiser).arg(Condition).arg(Ticker);
//	String((!child(3) || !child(3)->child(0) || child(3)->child(0)->isPlaceholder()) ? ";' ';(;3;)" : ";n;i;(;3;)");
}

String ForLoop::defineHtml() const
{
	return L"<span id=\"this\" class=\"keyword\">for</span> <span class=\"minor symbol\">(</span>" +
			toHtml(child(Initialiser)) + L"<span class=\"minor symbol\">;</span> " +
			toHtml(child(Condition)) + L"<span class=\"minor symbol\">;</span> " +
			toHtml(child(Ticker)) + L"<span class=\"minor symbol\">)</span>" + Corporal::defineHtml(true);
}

String ForLoop::code() const
{
	String ret;
	ret += "for (" + (isStatement(Initialiser) ? asStatement(Initialiser)->code() : "");
	ret += "; " + (isStatement(Condition) ? asStatement(Condition)->code() : "");
	ret += "; " + (isStatement(Ticker) ? asStatement(Ticker)->code() : "");
	ret += ")\n" + (isStatement(Body) ? asStatement(Body)->codeAsStatement() : "");
	return ret;
}

/*void ForLoop::decorate(DecorationContext const& _c) const
{
	_c->setPen(Qt::NoPen);
	_c->setBrush(QColor::fromHsv(120, 16, 236));
	QPainterPath p;
	p.addRect(_c.cap(0));
	p.addRect(_c.cap(1));
	_c->drawPath(p);
}*/

}
