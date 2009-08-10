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
#include "BareTyped.h"
#include "Const.h"
#include "Compound.h"
#include "BuiltinType.h"
#include "IfStatement.h"

namespace Martta
{

MARTTA_OBJECT_CPP(IfStatement);
MARTTA_OBJECT_CPP(UnlessStatement);

static SimpleIdentifierSet<IfStatement> s_ifSet(L"if");
static SimpleIdentifierSet<UnlessStatement> s_unlessSet(L"unless");

bool IfStatement::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<IfStatement>(_p, _e, "?");
}

String IfStatement::code() const
{
	String ret;
	ret += "if (" + (isStatement(Condition) ? asStatement(Condition)->code() : "");
	ret += ")\n" + (isStatement(Body) ? asStatement(Body)->codeAsStatement() : "");
	ret += "else\n" + (isStatement(AltBody) ? asStatement(AltBody)->codeAsStatement() : "{}");
	return ret;
}

Kinds IfStatement::allowedKinds(int _index) const
{
	if (_index == Condition)
		return Kind::of<BareTyped>();
	if (_index == Body || _index == AltBody)
		return Kind::of<Compound>();
	return Super::allowedKinds(_index);
}

Types IfStatement::allowedTypes(int _index) const
{
	if (_index == Condition)
		return Type(Bool).topWith(Const());
	return Super::allowedTypes(_index);
}

bool IfStatement::keyPressed(KeyEvent const* _e)
{
	if (Corporal::keyPressed(_e))
	{}
	else if (_e->text() == "E" && _e->focalIndex() != AltBody)
	{
		if (!child(AltBody))
			middle(AltBody).spawnPrepared();
		child(AltBody)->navigateOnto(_e->codeScene());
	}
	else
		return Super::keyPressed(_e);
	return true;
}

String IfStatement::defineLayout(ViewKeys const& _k) const
{
	return String("ycode;^;'if (';%1;')'").arg(Condition) + Corporal::defineLayout(_k, true) +
			(child(AltBody) ? ";n;'else'" + String(child(AltBody)->cardinalChildCount() ? ";n;i;%1" : ";%1").arg(AltBody) : String());
}

String IfStatement::defineHtml() const
{
	return L"<span id=\"this\" class=\"keyword\">if</span> <span class=\"minor symbol\">(</span>" + toHtml(child(Condition)) + L"<span class=\"minor symbol\">)</span>" + Corporal::defineHtml(true)
			+ (child(AltBody) ? L"<span class=\"keyword\">else</span>" + toHtml(child(AltBody)) : String::null);
}

bool UnlessStatement::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<UnlessStatement>(_p, _e, "!?");
}

String UnlessStatement::code() const
{
	String ret;
	ret += "if (" + (isStatement(Condition) ? asStatement(Condition)->code() : "");
	ret += ")\n" + (isStatement(AltBody) ? asStatement(AltBody)->codeAsStatement() : "{}");
	ret += "else\n" + (isStatement(Body) ? asStatement(Body)->codeAsStatement() : "");
	return ret;
}

String UnlessStatement::defineLayout(ViewKeys const& _k) const
{
	return String("ycode;^;'unless (';%1;')'").arg(Condition) + Corporal::defineLayout(_k, true) +
			(child(AltBody) ? ";n;'else'" + String(child(AltBody)->cardinalChildCount() ? ";n;i;%1" : ";%1").arg(AltBody) : String());
}

String UnlessStatement::defineHtml() const
{
	return L"<span id=\"this\" class=\"keyword\">unless</span> <span class=\"minor symbol\">(</span>" + toHtml(child(Condition)) + L"<span class=\"minor symbol\">)</span>" + Corporal::defineHtml(true)
			+ (child(AltBody) ? L"<span class=\"keyword\">else</span>" + toHtml(child(AltBody)) : String::null);
}

}
