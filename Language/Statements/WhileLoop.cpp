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
#include "Typed.h"
#include "BuiltinType.h"
#include "Const.h"
#include "Compound.h"
#include "BreakStatement.h"
#include "WhileLoop.h"

namespace Martta
{

MARTTA_OBJECT_CPP(WhileLoop);
MARTTA_OBJECT_CPP(UntilLoop);

static SimpleIdentifierSet<WhileLoop> s_whileLoopSet(L"while");
static SimpleIdentifierSet<UntilLoop> s_untilLoopSet(L"until");

Kinds WhileLoop::allowedKinds(int _index) const
{
	if (_index == Condition)
		return Kind::of<BareTyped>();
	return Super::allowedKinds(_index);
}

Types WhileLoop::allowedTypes(int _index) const
{
	if (_index == Condition)
		return Type(Bool).topWith(Const());
	return Super::allowedTypes(_index);
}

String WhileLoop::code() const
{
	String ret;
	ret += "while (" + (asStatement(Condition) ? asStatement(Condition)->code() : "");
	ret += ")\n" + (asStatement(Body) ? asStatement(Body)->codeAsStatement() : "");
	return ret;
}

String WhileLoop::defineLayout(ViewKeys const& _k) const
{
	return ("ycode;^;'while (';%1;')'" + Corporal::defineLayout(_k, true)).arg(Condition);
}

String WhileLoop::defineHtml() const
{
	return L" <span id=\"this\" class=\"keyword\">while</span> <span class=\"minor symbol\">(</span>" + toHtml(child(Condition)) + L"<span class=\"minor symbol\">)</span><br/>" + Corporal::defineHtml(true);
}

Kinds UntilLoop::allowedKinds(int _index) const
{
	if (_index == Condition)
		return Kind::of<Typed>();
	return Super::allowedKinds(_index);
}

String UntilLoop::code() const
{
	String ret;
	ret += "while (!(" + (asStatement(Condition) ? asStatement(Condition)->code() : "");
	ret += "))\n" + (asStatement(Body) ? asStatement(Body)->codeAsStatement() : "");
	return ret;
}

String UntilLoop::defineLayout(ViewKeys const& _k) const
{
	return ("ycode;^;'until (';%1;')'" + Corporal::defineLayout(_k, true)).arg(Condition);
}

String UntilLoop::defineHtml() const
{
	return L"<span id=\"this\" class=\"keyword\">until</span> <span class=\"minor symbol\">(</span>" + toHtml(child(Condition)) + L"<span class=\"minor symbol\">)</span><br/>" + Corporal::defineHtml(true);
}

}
