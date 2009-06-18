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

#include "EntityKeyEvent.h"
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

QString LambdaNamer::defineReturnLayout(ViewKeys&) const
{
	int sReturn = self()->childIndexOf<TypeEntity>();
	return QString::number(sReturn) + ";Mo";
}

QString LambdaNamer::defineNameLayout(ViewKeys&) const
{
	int sName = self()->childIndexOf<IdLabel>();
	if (sName != UndefinedIndex)
		return "ynormal;s" + FunctionType().idColour() + ";!" + QString::number(sName);
	else
		return "ycode;'" + name().replace(";", "\\;").replace("'", "\\'") + "'";
}

QString LambdaNamer::defineArgListLayout(ViewKeys&) const
{
	int sFirstArg = self()->childIndexOf<Argument>();
	int sArgCount = self()->childCountOf<Argument>();
	return "ycode;'(';" + times(sFirstArg, sFirstArg + sArgCount, ";', ';") + ";')'";
}

QString LambdaNamer::defineBodyLayout(ViewKeys& _viewKeys) const
{
	int sBody = self()->childIndexOf<Compound>();
	QString ret;
	if (body() && sBody != UndefinedIndex)
		if (_viewKeys["expanded"].toBool())
			ret += (body()->cardinalChildCount() ? "n;i;" : "") + QString::number(sBody);
		else
		{
			ret += "yminor;' (";
			if (int n = body()->childrenOf<Primary>().count() + body()->childrenOf<Untyped>().count())
				ret += QString::number(n) + " statement" + (n > 1 ? "s, " : ", ");
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

QString LambdaNamer::defineLayout(ViewKeys& _k, QString _middle) const
{
	// TODO handle ellipsis here so we can put one in/take one out
	return definePreLayout(_k) + ";" + defineReturnLayout(_k) + ";>name;" + defineNameLayout(_k) + ";" + defineArgListLayout(_k) + ";" + defineMidLayout(_k, _middle) + ";" + defineBodyLayout(_k) + ";" + definePostLayout(_k);
}

bool LambdaNamer::keyPressed(EntityKeyEvent const* _e)
{
	int sFirstArg = self()->childIndexOf<Argument>();
	int sName = self()->childIndexOf<IdLabel>();
	if ((_e->text() == "(" && !argumentCount() && (_e->focalIndex() == sName || _e->isFocused()) || _e->text() == "," && _e->focalIndex() >= sFirstArg) && self()->back().allowedToBeKind<Argument>())
	{
		Argument* v = new Argument;
		self()->back().place(v);
		v->prepareChildren();
		v->navigateInto(_e->codeScene());
	}
	else if (_e->text() == "(" && _e->focalIndex() == 0 && argument(0))
	{
		argument(0)->navigateInto(_e->codeScene());
	}
	else if (_e->text() == " " && self()->childIs<TypeEntity>(_e->focalIndex()) && self()->childCountOf<IdLabel>())
	{
		self()->childrenOf<IdLabel>()[0]->navigateOnto(_e->codeScene());
	}
	else
		return false;
	return true;
}

Type LambdaNamer::type() const
{
	Type ret = FunctionType(this->ellipsis());
	ret.append(returns());
	for (int i = 0; i < argumentCount(); i++)
		ret.append(argumentType(i));

	ret.topWith(Reference());
	return ret;
}

QString LambdaNamer::implementationCode() const
{
	return basicCode(LambdaNamer::OutsideScope) + "\n" + (body() ? body()->code() : QString());
}

QString LambdaNamer::callingCode(FunctionCodeScope _ref) const
{
	QString ret = ((_ref == OutsideScope) ? reference() : codeName()) + "(";
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

QString LambdaNamer::basicCode(FunctionCodeScope _ref) const
{
	return Martta::code(qualifiers() & FunctionMask) + returns()->code(" " + callingCode(_ref));
}

Compound* LambdaNamer::body() const
{
	if (self()->childrenOf<Compound>().size())
		return self()->childrenOf<Compound>()[0];
	return 0;
}

int LambdaNamer::argumentCount() const
{
	return self()->cardinalChildrenOf<Argument>().size();
}

Argument* LambdaNamer::argument(int _index) const
{
	if (_index < self()->cardinalChildrenOf<Argument>().size())
		return self()->cardinalChildrenOf<Argument>()[_index];
	return 0;
}

Type LambdaNamer::returns() const
{
	if (self()->childrenOf<TypeEntity>().size())
		return *self()->childrenOf<TypeEntity>()[0];
	return 0;
}

Type LambdaNamer::argumentType(int _index) const
{
	if (Argument* v = argument(_index))
		return *v->actualType();
	return 0;
}

QString LambdaNamer::argumentName(int _index) const
{
	if (Argument* v = argument(_index))
		return v->name();
	return 0;
}

QString LambdaNamer::argumentCodeName(int _index) const
{
	if (Argument* v = argument(_index))
		return v->codeName();
	return 0;
}

}
