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
#include "ExplicitType.h"
#include "Const.h"
#include "Pointer.h"
#include "ConstLabel.h"
#include "Compound.h"
#include "Argument.h"
#include "MemberLambda.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(MemberLambda);
MARTTA_REGISTER_CSS(MemberLambda,
	WebViewable::cssBorder(L".MemberLambda-public-", AccessLabel::idColour(Public).interpolated(70, Rgb(0xffffff))) +
	WebViewable::cssBorder(L".MemberLambda-protected-", AccessLabel::idColour(Protected).interpolated(70, Rgb(0xffffff))) +
	WebViewable::cssBorder(L".MemberLambda-private-", AccessLabel::idColour(Private).interpolated(70, Rgb(0xffffff))));

Kinds MemberLambda::allowedKinds(int _i) const
{
	if (_i == Constness)
		return Kind::of<ConstLabel>();
	if (_i == Body)
		return Kind::of<HardCompound>();
	if (_i == Returned)
		return Kind::of<TypeEntity>();
	if (_i >= 0)
		return Kind::of<Argument>();
	return Super::allowedKinds(_i);
}

bool MemberLambda::isConst() const
{
	if (ConstLabel* c = tryChild<ConstLabel>(Constness))
		return c->isConst();
	return false;
}

String MemberLambda::basicCode(FunctionCodeScope _ref) const
{
	if (!isComplete())
		return String::null;
	return Martta::code(qualifiers() & FunctionMask) + returns()->code(" " + callingCode(_ref)) + (isConst() ? " const" : "");
}

List<Declaration*> MemberLambda::utilised() const
{
	List<Declaration*> l = Super::utilised();
	l.removeAll(ancestor<TypeDefinition>()->tryKind<Declaration>());
	return l;
}

Type MemberLambda::thisType() const
{
	Type t(typeDefinition());
	if (isConst())
		t.topWith(Const());
	return t.topWith(Pointer());
}

String MemberLambda::memberDefineHtml() const
{
	return defineLambdaHtml();
}

String MemberLambda::definePreHtml() const
{
	return String(L"<div class=\"ConstLabel-%1constblock\"\">").arg(isConst() ? L"" : L"non");
}

String MemberLambda::definePostHtml() const
{
	return L"</div>";
}

bool MemberLambda::keyPressed(KeyEvent const* _e)
{
	AssertNR(isComplete());
	if (LambdaNamer::keyPressed(_e))
		return true;
	else if ((_e->isFocused() || !childIs<Compound>(_e->focalIndex())) && child(Constness) && childAs<Label>(Constness)->keyPressed(_e))
		return true;
	return Super::keyPressed(_e);
}

}
