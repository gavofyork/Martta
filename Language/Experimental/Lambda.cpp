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
#include "FunctionType.h"
#include "ReturnStatement.h"
#include "Namespace.h"
#include "Class.h"
#include "OperatorLabel.h"
#include "MethodOperator.h"
#include "Constructor.h"
#include "Construction.h"
#include "Field.h"
#include "Position.h"
#include "ReferencedValue.h"
#include "Lambda.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(ClosureEntry);
MARTTA_PROPER_CPP(ClosureDefaultValue);
MARTTA_PROPER_CPP(ClosureDefaultReference);
MARTTA_PROPER_CPP(Closure);
MARTTA_PROPER_CPP(Lambda);
MARTTA_NAMED_CPP(Lambda, ClosureSet);

int Lambda::minRequired(int _i) const
{
	return (_i == ClosureSet || _i == Body) ? 1 : Super::minRequired(_i);
}

Kinds Lambda::allowedKinds(int _i) const
{
	if (_i == ClosureSet)
		return Kind::of<Closure>();
	if (_i == Body)
		return Kind::of<SoftCompound>();
	if (_i == Returned)
		return Kind::of<TypeConcept>();
	if (_i >= 0)
		return Kind::of<Argument>();
	return Super::allowedKinds(_i);
}

String Lambda::defineHtml() const
{
	return L"<^>" + toHtml(child(ClosureSet)) + defineArgListHtml() + (childCount(Returned) ? tagOf("symbol", " -> ") + defineReturnHtml() : "") + " " + toHtml(body());
}

bool Lambda::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == "[")
	{
		_e->codeScene()->navigateInto(childAs<Closure>(ClosureSet)->firstChild());
	}
	else if (!(_e->text() == "(" && _e->isFocused()) && LambdaNamer::keyPressed(_e))
	{}
	else
		return Super::keyPressed(_e);
	return true;
}

Type Lambda::type() const
{
	Type t = LambdaNamer::type();
	if (t->isType<FunctionType>() && t->asType<FunctionType>()->returnType().isNull())
	{
		List<ReturnStatement*> r = superChildrenOf<ReturnStatement>();
		if (r.size())
			// TODO: check they'll all agree
			r[0]->typeOf(ReturnStatement::Returned).insertSilentCopy(t->asType<FunctionType>()->middle(FunctionType::Returned));
	}
	return t;
}

bool Lambda::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (simplePositionKeyPressHandler<Lambda>(_p, _e, "[", false))
	{
		_e->reinterpretLater();
		return true;
	}
	else
		return false;
}

void Lambda::compose()
{
	/*IfStatement* t = new IfStatement;
	child(Condition)->silentMove(t->middle(Condition));
	if (child(Body))
		child(Body)->silentMove(t->middle(AltBody));
	else
		(new Compound)->silentMove(t->middle(AltBody));
	if (child(AltBody))
		child(AltBody)->silentMove(t->middle(Body));
	else
		(new Compound)->silentMove(t->middle(Body));
	replace(t);*/

	TopLevel* n = ancestor<Namespace>();
	if (!n)
		return Composite::compose();
	int ni = ancestorIndex<Namespace>();

	Class* c = new Class;
	c->prepareChildren();

	Constructor* ctor = new Constructor;
	c->firstFor(ctor->kind()).insertSilent(ctor);
	ctor->prepareChildren();

	MethodOperator* mop = new MethodOperator;
	c->firstFor(mop->kind()).insertSilent(mop);
	mop->middle(MethodOperator::Identity).insertSilent(new OperatorLabel(Operator::Parentheses));
	type()->asType<FunctionType>()->returnType().insertSilentCopy(mop->middle(MethodOperator::Returned));
	foreach (Argument* a, cardinalChildrenAs<Argument>())
		a->silentMove(mop->back());
	mop->prepareChildren();
	mop->body()->child(0)->killAndDelete();
	foreach (Concept* i, child(Body)->cardinalChildren())
		i->silentMove(mop->body()->back());

	/*
	foreach (Argument* a, cardinalChildrenAs<Argument>())
	{
		Field* f = new Field;
		f->prepareChildren();
		c->firstFor(f->kind()).insertSilent(f);
		a->rewirePointer(f);
	}*/
	/*
	List<ReferencedValue*> crv = child(Body)->superChildrenOf<ReferencedValue>();
	foreach (Argument* a, cardinalChildrenAs<Argument>())
	{
		Field* f = new Field;
		f->middle(Field::Accessibility).insertSilent(new AccessLabel(Private));
		f->middle(Field::OurType).insertSilent(TypeConcept::cloneOf(a->actualType(), 0));
		f->prepareChildren();
		c->firstFor(f->kind()).insertSilent(f);
		foreach (ReferencedValue* v, crv)
			if (v->subject() == a)
				v->set(f);
	}*/

	n->middle(ni).insertSilent(c);
	Construction* t = new Construction(ctor);
	replace(t);
	n->debugTree();
}

}
