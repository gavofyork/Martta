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

#include "Argument.h"
#include "Compound.h"
#include "Primary.h"
#include "FunctionType.h"
#include "Reference.h"
#include "IdLabel.h"
#include "KeyEvent.h"
#include "LambdaNamer.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(LambdaNamer);
MARTTA_NAMED_CPP(LambdaNamer, Body);
MARTTA_NAMED_CPP(LambdaNamer, Name);
MARTTA_NAMED_CPP(LambdaNamer, Returned);
MARTTA_REGISTER_CSS(LambdaNamer, WebViewable::cssBorder(L".LambdaNamer-", Rgb(0xdddddd)));

String LambdaNamer::defineReturnHtml() const
{
	return toHtml(self()->child(Returned)) + L" ";
}

String LambdaNamer::defineNameHtml() const
{
	if (self()->child(Identity))
		return FunctionType().typeHtml(toHtml(self()->child(Identity)));
	else
		return name();
}

String LambdaNamer::defineArgListHtml() const
{
	return "<span class=\"symbol\">(</span>" + toHtml(self()->cardinalChildren(), L"<span class=\"symbol\">, </span>") + L"<span class=\"symbol\">)</span>";
}

String LambdaNamer::defineBodyHtml() const
{
	if (!body())
		return String::null;
	return String(L"<div id=\"%1-body\" ondblclick=\"event.stopPropagation()\" style=\"display: none\">").arg((int)self()) + toHtml(body()) + L"</div>";
}

String LambdaNamer::defineMidHtml(String const& _middle) const
{
	String info;
	if (body())
	{
		if (int n = body()->cardinalChildrenOf<Primary>().count() + body()->cardinalChildrenOf<Untyped>().count())
			info = L" (" + String::number(n) + L" statement" + (n > 1 ? L"s" : L"") + L")";
		else
			info = " (empty)";
	}
	return String(_middle + L"<span id=\"%1-info\" class=\"minor\">" + info + L"</span>").arg((int)self());
}

String LambdaNamer::defineEnclosureHtml(String const& _part, String const& _middle) const
{
	return L"<div class=\"" + defineBorderClass() + L"-" + _part + "\">" + _middle + L"</div>";
}

String LambdaNamer::defineLambdaHtml(String const& _middle) const
{
	return String(L"<div onKeyPress=\"if (event=='{' ? set2('%1-info', '%1-body') : event=='}' ? set1('%1-info', '%1-body') : false) { CodeView.markDirty(%1); return true; } return false;\" onDblClick=\"toggle('%1-info', '%1-body'); CodeView.markDirty(%1); event.stopPropagation();\">").arg((int)self())
		+ definePreHtml()
		+ defineEnclosureHtml(L"head", defineReturnHtml() + defineNameHtml() + defineArgListHtml() + defineMidHtml(_middle))
		+ defineEnclosureHtml(L"body", defineBodyHtml()) + definePostHtml() + L"</div>";
}

bool LambdaNamer::keyPressed(KeyEvent const* _e)
{
	if (((_e->text() == L"(" && !argumentCount() && (_e->focalIndex() == Identity || _e->isFocused())) || (_e->text() == L"," && _e->focalIndex() >= 0)) && self()->back().allowedToBeKind<Argument>() && !self()->back().isFixed())
	{
		Argument* v = new Argument;
		((_e->focalIndex() < 0 || _e->focalIndex() == UndefinedIndex) ? self()->back() : self()->middle(_e->focalIndex() + (_e->isInserting() ? 0 : 1))).place(v);
		v->prepareChildren();
		_e->codeScene()->navigateInto(v);
	}
	else if (_e->text() == "(" && _e->focalIndex() == Identity && argument(0))
	{
		_e->codeScene()->navigateInto(argument(0));
	}
	else if (_e->text() == " " && _e->focalIndex() == Returned && self()->child(Identity))
	{
		_e->codeScene()->navigateOnto(self()->child(Identity));
	}
	else if (_e->text() == "{" && _e->focalIndex() != Body)
	{
		_e->codeScene()->navigateInto(self()->child(Body));
	}
	else
		return false;
	return true;
}

Type LambdaNamer::type() const
{
	Type ret = FunctionType(this->ellipsis());
	ret.place(returns(), FunctionType::Returned);
	for (int i = 0; i < argumentCount(); i++)
		ret.append(argumentType(i));
	ret.topWith(Reference());
	return ret;
}

String LambdaNamer::implementationCode() const
{
	return basicCode(LambdaNamer::OutsideScope) + "\n" + (body() ? body()->code() : String::null);
}

String LambdaNamer::callingCode(FunctionCodeScope _ref) const
{
	String ret = ((_ref == OutsideScope) ? reference() : codeName()) + "(";
	for (int i = 0; i < argumentCount(); i++)
	{
		if (i) ret += ", ";
			ret += argumentType(i)->code(" " + argumentCodeName(i));
	}
	if (ellipsis())// && _ref == OutsideScope)
		ret += argumentCount() ? ", ..." : "...";
	ret += ")";
	return ret;
}

String LambdaNamer::basicCode(FunctionCodeScope _ref) const
{
	return Martta::code(qualifiers() & FunctionMask) + returns()->code(" " + callingCode(_ref));
}

Compound* LambdaNamer::body() const
{
	return self()->tryChild<Compound>(Body);
}

int LambdaNamer::argumentCount() const
{
	return self()->cardinalChildCount();
}

Argument* LambdaNamer::argument(int _index) const
{
	if (_index < argumentCount())
		return self()->tryChild<Argument>(_index);
	return 0;
}

Type LambdaNamer::returns() const
{
	if (TypeEntity* t = self()->tryChild<TypeEntity>(Returned))
		return *t;
	return Type();
}

Type LambdaNamer::argumentType(int _index) const
{
	if (Argument* v = argument(_index))
		return *v->actualType();
	return Type();
}

String LambdaNamer::argumentName(int _index) const
{
	if (Argument* v = argument(_index))
		return v->name();
	return String::null;
}

String LambdaNamer::argumentCodeName(int _index) const
{
	if (Argument* v = argument(_index))
		return v->codeName();
	return String::null;
}

}
