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

#include "KeyEvent.h"
#include "IdLabel.h"
#include "Primary.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Argument.h"
#include "Compound.h"
#include "LambdaNamer.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(LambdaNamer);
MARTTA_NAMED_CPP(LambdaNamer, Body)
MARTTA_NAMED_CPP(LambdaNamer, Name)
MARTTA_NAMED_CPP(LambdaNamer, Returned)

String LambdaNamer::defineReturnLayout(ViewKeys const&) const
{
	return String("%1;Mo").arg(Returned);
}

String LambdaNamer::defineNameLayout(ViewKeys const&) const
{
	if (self()->child(Identity))
		return String("ynormal;s%1;!%2").arg(FunctionType().idColour().name()).arg(Identity);
	else
		return "ycode;'" + name().replace(";", "\\;").replace("'", "\\'") + "'";
}

String LambdaNamer::defineArgListLayout(ViewKeys const&) const
{
	return "ycode;'(';" + times(0, self()->cardinalChildCount(), ";', ';") + ";')'";
}

String LambdaNamer::defineBodyLayout(ViewKeys const& _viewKeys) const
{
	String ret;
	if (body())
		if (_viewKeys["expanded"].toBool())
			ret += (body()->cardinalChildCount() ? "n;i;" : "") + String::number(Body);
		else
		{
			ret += "yminor;' (";
			if (int n = body()->cardinalChildrenOf<Primary>().count() + body()->cardinalChildrenOf<Untyped>().count())
				ret += String::number(n) + " statement" + (n > 1 ? "s, " : ", ");
			if (ret.endsWith(", "))
				ret.chop(2);
			ret += ")";
			if (ret.endsWith(" ()"))
				ret.replace(" ()", " (empty)");
			ret += "'";
		}
	else
		ret += "' = 0'";
	return ret;
}

String LambdaNamer::defineLayout(ViewKeys const& _k, String const& _middle) const
{
	// TODO handle ellipsis here so we can put one in/take one out
	return definePreLayout(_k) + ";" + defineReturnLayout(_k) + ";>name;" + defineNameLayout(_k) + ";" + defineArgListLayout(_k) + ";" + defineMidLayout(_k, _middle) + ";" + defineBodyLayout(_k) + ";" + definePostLayout(_k);
}

bool LambdaNamer::keyPressed(KeyEvent const* _e)
{
	if ((_e->text() == "(" && !argumentCount() && (_e->focalIndex() == Identity || _e->isFocused()) || _e->text() == "," && _e->focalIndex() >= 0) && self()->back().allowedToBeKind<Argument>())
	{
		Argument* v = new Argument;
		self()->back().place(v);
		v->prepareChildren();
		v->navigateInto(_e->codeScene());
	}
	else if (_e->text() == "(" && _e->focalIndex() == Identity && argument(0))
	{
		argument(0)->navigateInto(_e->codeScene());
	}
	else if (_e->text() == " " && _e->focalIndex() == Returned && self()->child(Identity))
	{
		self()->child(Identity)->navigateOnto(_e->codeScene());
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
	return basicCode(LambdaNamer::OutsideScope) + "\n" + (body() ? body()->code() : String());
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
