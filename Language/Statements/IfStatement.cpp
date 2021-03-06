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

MARTTA_PROPER_CPP(IfStatement);
MARTTA_NAMED_CPP(IfStatement, AltBody);
MARTTA_PROPER_CPP(UnlessStatement);

static SimpleIdentifierSet<IfStatement> s_ifSet(L"if");
static SimpleIdentifierSet<UnlessStatement> s_unlessSet(L"unless");

bool IfStatement::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<IfStatement>(_p, _e, "?");
}

String IfStatement::code() const
{
	String ret;
	ret += "if (" + (isStatement(Condition) ? asStatement(Condition)->code() : "");
	ret += ")\n" + (isStatement(Body) ? asStatement(Body)->codeAsStatement() : "{}");
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
		_e->codeScene()->navigateOnto(child(AltBody));
	}
	else
		return Super::keyPressed(_e);
	return true;
}

String IfStatement::defineHtml() const
{
	return L"<^>" + tagOf(L"keyword", L"if") + L" " + tagOf("minor symbol", L"(") + toHtml(child(Condition)) + tagOf("minor symbol", L")") + L"<br/>" + Corporal::defineHtml(true)
			+ (child(AltBody) ? L"<br/>" + tagOf(L"keyword", L"else") + L"<br/>" + toHtml(child(AltBody)) : String::null);
}

bool UnlessStatement::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<UnlessStatement>(_p, _e, "!?");
}

void UnlessStatement::compose()
{
	IfStatement* t = new IfStatement;
	child(Condition)->silentMove(t->middle(Condition));
	if (child(Body))
		child(Body)->silentMove(t->middle(AltBody));
	else
		(new Compound)->silentMove(t->middle(AltBody));
	if (child(AltBody))
		child(AltBody)->silentMove(t->middle(Body));
	else
		(new Compound)->silentMove(t->middle(Body));
	replace(t);
}

String UnlessStatement::defineHtml() const
{
	return L"<^><span class=\"keyword\">unless</span> <span class=\"minor symbol\">(</span>" + toHtml(child(Condition)) + L"<span class=\"minor symbol\">)</span>" + Corporal::defineHtml(true)
			+ (child(AltBody) ? L"<span class=\"keyword\">else</span>" + toHtml(child(AltBody)) : String::null);
}

}
