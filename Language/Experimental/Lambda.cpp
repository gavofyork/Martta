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

#include <auto_ptr.h>

#include "msUnitTests.h"
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
#include "Reference.h"
#include "Const.h"
#include "BuiltinType.h"
#include "ReferencedValue.h"
#include "LocalReferencedValue.h"
#include "ArgumentReferencedValue.h"
#include "MemberReferencedValue.h"
#include "VariablePlacer.h"
#include "Lambda.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(ClosureEntry);
MARTTA_PLACEHOLDER_CPP(ClosureDefault);
MARTTA_PROPER_CPP(ClosureDefaultValue);
MARTTA_PROPER_CPP(ClosureDefaultReference);
MARTTA_PROPER_CPP(ClosureExplicit);
MARTTA_NAMED_CPP(ClosureExplicit, Variable);
ClosureExplicit::ClosureExplicit(ValueDefiner* _v)
{
	if (_v)
		middle(Variable).insertSilent(new ScopedReferencedValue(_v));
}
Kinds ClosureExplicit::allowedKinds(int _i) const { return _i == Variable ? Kinds() << Kind::of<ReferencedValue>() : Super::allowedKinds(_i); }

MARTTA_PROPER_CPP(ClosureExplicitReference);
MARTTA_PROPER_CPP(ClosureExplicitValue);
MARTTA_PROPER_CPP(Closure);
MARTTA_PROPER_CPP(Lambda);
MARTTA_NAMED_CPP(Lambda, ClosureSet);
MARTTA_PROPER_CPP(LambdaType);
MARTTA_PROPER_CPP(CommentedOut);

class ClosureExplicitSet: SimpleIdentifierSet<ClosureExplicit>
{
public:
	ClosureExplicitSet(): SimpleIdentifierSet<ClosureExplicit>(L"ClosureExplicitSet"){}
	virtual List<Named*>				identifiableAt(Position const& _p)
	{
		List<Named*> ret;
		if (_p.allowedToBeKind<ClosureExplicit>())
		{
			Position p = _p->ancestor<Lambda>()->over();
			foreach (Kind k, Kind::of<ScopedReferencedValue>().immediateDeriveds())
				foreach (Named* n, std::auto_ptr<ScopedReferencedValue>(k.spawn()->asKind<ScopedReferencedValue>())->refPossibilities(p))
					if (!ret.contains(n))
						ret += n;
		}
		return ret;
	}
	virtual void						acceptAt(Position const& _p, Named* _i, CodeScene* _cs) { _cs->setCurrent(_p.place(new ClosureExplicit(static_cast<Identifiable*>(_i)->asKind<ValueDefiner>()))); }
} static s_closureExplicitSetRegistrand;

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
	return L"<^>" + toHtml(child(ClosureSet)) + defineArgListHtml() + (childCount(Returned) ? tagOf("symbol", "&rarr;") + defineReturnHtml() + " " : "") + toHtml(body());
}

bool Lambda::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == "[")
	{
		_e->codeScene()->navigateInto(childAs<Closure>(ClosureSet)->firstChild());
	}
	else if (_e->text() == "}")
	{
		_e->codeScene()->setCurrent(this);
	}
	else if (!(_e->text() == "(" && _e->isFocused()) && LambdaNamer::keyPressed(_e))
	{}
	else if (_e->text() == "]" && navigateIntoParams(_e->codeScene()))
	{}
	else if (_e->text() == "->")
	{
		if (!childCount(Returned))
			middle(Returned).spawnPrepared();
		_e->codeScene()->navigateInto(child(Returned));
	}
	else
		return Super::keyPressed(_e);
	return true;
}

Type Lambda::returns() const
{
	if (Type t = LambdaNamer::returns())
		return t;

	List<ReturnStatement*> r = superChildrenOf<ReturnStatement>();
	if (r.size() == 1 && r[0]->childIs<Typed>(ReturnStatement::Returned))
		return r[0]->childAs<Typed>(ReturnStatement::Returned)->bareType();
	else if (!r.size())
		return BuiltinType(Void);
	return Type();
}

Type Lambda::type() const
{
	return LambdaType(const_cast<Lambda*>(this));
}

bool Lambda::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (simplePositionKeyPressHandler<Lambda>(_p, _e, "[", false, false))
	{
		_e->reinterpretLater();
		return true;
	}
	else
		return false;
}

void Lambda::compose()
{
	TopLevel* n = ancestor<Namespace>();
	if (!n)
		return Composite::compose();
	int ni = ancestorIndex<Namespace>();

	Class* c = new Class;
	c->prepareChildren();

	Constructor* ctor = new Constructor;
	c->firstFor(ctor->kind()).insertSilent(ctor);
	InitialiserList* il = new InitialiserList;
	ctor->firstFor(il->kind()).insertSilent(il);
	ctor->prepareChildren();

	MethodOperator* mop = new MethodOperator;
	c->firstFor(mop->kind()).insertSilent(mop);
	mop->middle(MethodOperator::Identity).insertSilent(new OperatorLabel(Operator::Parentheses));
	type()->asType<LambdaType>()->returnType().insertSilentCopy(mop->middle(MethodOperator::Returned));
	foreach (Argument* a, cardinalChildrenAs<Argument>())
		a->silentMove(mop->back());
	mop->prepareChildren();

	Construction* t = new Construction(ctor);
	List<ClosureExplicit*> ccx = child(ClosureSet)->cardinalChildrenOf<ClosureExplicit>();

	bool haveDefault = false;
	bool byValue = false;
	List<ClosureDefault*> ccd = child(ClosureSet)->cardinalChildrenOf<ClosureDefault>();
	if (ccd.size() == 1)
	{
		haveDefault = true;
		byValue = ccd.first()->isKind<ClosureDefaultValue>();
	}

	List<ScopedReferencedValue*> crv = child(Body)->superChildrenOf<ScopedReferencedValue>();
	while (crv.count())
	{
		ScopedReferencedValue* v = crv.takeLast();
		if (v->subject() && v->subject()->parent() != mop && v->subject()->parent() != c)
		{
			bool goodToGo = haveDefault;
			bool curByValue = byValue;
			foreach (ClosureExplicit* c, ccx)
				if (c->childAs<ReferencedValue>(ClosureExplicit::Variable)->subject() == v->subject())
				{
					curByValue = !c->isKind<ClosureExplicitReference>();
					goodToGo = true;
					break;
				}

			if (goodToGo)
			{
				Argument* a = new Argument;
				if (curByValue)
					Reference::ensure(Const::ensure(v->bareType())).insertSilentCopy(a->middle(Argument::OurType));
				else
					Reference::ensure(v->bareType()).insertSilentCopy(a->middle(Argument::OurType));
				a->prepareChildren();
				ctor->back().insertSilent(a);
				t->back().insertSilent(new ReferencedValue(v->subject()));

				Field* f = new Field;
				f->middle(Field::Accessibility).insertSilent(new AccessLabel(Private));
				if (curByValue)
					v->bareType().insertSilentCopy(f->middle(Field::OurType));
				else
					Reference::ensure(v->bareType()).insertSilentCopy(f->middle(Field::OurType));
				f->prepareChildren();
				c->back().insertSilent(f);
				v->set(f);

				InitialiserItem* iser = new InitialiserItem;
				iser->middle(InitialiserItem::TheField).insertSilent(new FloatingMemberReferencedValue(f));
				iser->middle(InitialiserItem::Value).insertSilent(new ArgumentReferencedValue(a));
				il->back().insertSilent(iser);
			}
			else
				v->set(0);

			for (int i = 0; i < crv.count();)
				if (crv[i]->subject() == v->subject())
				{
					crv[i]->set(v->subject());
					crv.removeAt(i);
				}
				else
					i++;
		}
	}

	mop->body()->child(0)->killAndDelete();
	foreach (Concept* i, child(Body)->cardinalChildren())
		i->silentMove(mop->body()->back());

	n->middle(ni).insertSilent(c);
	replace(t);
//	n->debugTree();
}

MARTTA_PROPER_CPP(AutoType);

void AutoType::initialiseClass()
{
	BuiltinType::registerExtra(L"auto", staticKind);
}

void AutoType::finaliseClass()
{
	BuiltinType::unregisterExtra(L"auto");
}

Type AutoType::declaredType(Type const& _t) const
{
	return *_t->ignore<Reference>();
}

class AutoTypeSet: public IdentifierSet
{
public:
	AutoTypeSet(): m_ourNamed(L"auto") {}
	virtual String						setId() const { return L"Martta::AutoType"; }
	virtual List<Named*>				identifiableAt(Position const& _p)
	{
		if (canPlaceVariable(_p))
			return List<Named*>() << &m_ourNamed;
		return List<Named*>();
	}
	virtual void						acceptAt(Position const& _pos, Named*, CodeScene* _cs)
	{
		placeVariable(_pos, new AutoType, _cs);
	}
	virtual String						defineEditHtml(Named*, String const& _mid)
	{
		return AutoType::mark() + AutoType::tagOf("TypeConcept", AutoType().typeHtml(_mid));
	}
	SimpleNamed m_ourNamed;
};

static AutoTypeSet s_autoTypeSet;

MARTTA_PROPER_CPP(TypeArgument);
MARTTA_NAMED_CPP(TypeArgument, Default);
MARTTA_NOTION_CPP(Template);
MARTTA_NAMED_CPP(Template, Parameters);
MARTTA_NAMED_CPP(Template, Instantiations);
MARTTA_PROPER_CPP(TemplateClass);

String TemplateParameters::preamble() const
{
	String ret;
	foreach (TypeArgument* ta, cardinalChildrenOf<TypeArgument>())
	{
		if (ret.length())
			ret += L", ";
		ret += ta->code();
	}
	return String(L"template<%1>\n").arg(ret);
}

String TemplateParameters::refPostamble() const
{
	String ret;
	foreach (TypeArgument* ta, cardinalChildrenOf<TypeArgument>())
	{
		if (ret.length())
			ret += L", ";
		ret += ta->childAs<IdLabel>(TypeArgument::Identity)->code();
	}
	return String(L"<%1>").arg(ret);
}

Concept* copyTree(Concept const* _original, Position const& _position)
{
	// Hidden in this file because it won't work generally - if the _original has a ModelPtr in one of its
	// nodes but doesn't have the accompanying Identifiable it won't know to tell the Identifiable to apresLoad()
	// itself (i.e. call loadFinished()) after copying.
	// XXX: This should fsck up now with a program involving printf.
	// FIX: Call copyTree for the entire Program, not per-module and have a specialised copyTree() for in-model copying.

	Concept* ret = _original->kind().spawn();
	if (_position != Nowhere)
		ret->silentMove(_position);
	ret->copyProperties(_original);
	foreach (Concept* c, _original->children())
		copyTree(c, ret->middle(c->index()));
	if (_position == Nowhere)
	{
		ret->loadFinished();
		_original->root()->apresLoad();	// TODO: replace a postCopy() const method that calls restorePtrs
	}
	return ret;
}

Concept*							Template::instantiation(List<Concept*> const& _params) const
{
	List<Type> tp;
	for (int i = 0; i < _params.size(); i++)
		tp += Type(*_params[i]->tryKind<TypeConcept>());
	if (!m_instantiations[tp])
	{
		Concept* ret = copyTree(this, Nowhere);
		for (int i = 0; i < argumentCount(); i++)
		{
			if (i < _params.count())
				if (TypeArgument* ta = argument(i))
					ta->killAndDelete(copyTree(_params[i], Nowhere));
			i++;
		}
		const_cast<Template*>(this)->middle(Instantiations).insertSilent(ret);
		m_instantiations[tp] = ret;
	}
	return m_instantiations[tp];
}

MARTTA_PROPER_CPP(TemplateParameters);
MARTTA_NOTION_CPP(TemplatedTypeDefinition);
MARTTA_PROPER_CPP(TemplatedReferencedType);

List<TemplatedTypeDefinition*>		TemplatedReferencedType::possibilities() const
{
	return selfAndAncestorsChildrenOf<TemplatedTypeDefinition>();
}
bool							TemplatedReferencedType::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_e->text() == L"<")
		if (ReferencedType* rt = _p->tryKind<ReferencedType>())
			if (TemplatedTypeDefinition* ttd = rt->get()->tryKind<TemplatedTypeDefinition>())
			{
				TemplatedReferencedType* trt = new TemplatedReferencedType(ttd);
				rt->deleteAndRefill(trt);
				trt->prepareChildren();
				return true;
			}
	return false;
}

String								TemplatedReferencedType::templatePostamble() const
{
	String ret;
	foreach (Concept* c, cardinalChildren())
	{
		if (!ret.isEmpty())
			ret += L", ";
		if (TypeConcept* t = c->tryKind<TypeConcept>())
			ret += t->code();
		else
		{}// TODO: Add code for all possibilities.
	}
	return String(L"<%1>").arg(ret);
}
String						TemplatedReferencedType::defineHtml() const
{
	return Super::defineHtml() + String(L"<%1>").arg(toHtml(cardinalChildren(), L", "));
}
bool						TemplatedReferencedType::keyPressed(KeyEvent const* _e)
{
	return Super::keyPressed(_e);
}
String						TemplatedReferencedType::defineEditHtml(CodeScene* _cs) const
{
	return Super::defineEditHtml(_cs) + String(L"<%1>").arg(toHtml(cardinalChildren(), L", "));
}
EditDelegateFace*			TemplatedReferencedType::newDelegate(CodeScene* _s)
{
	return Super::newDelegate(_s);
}

MS_TEST(TemplateStuff)
{
	TemplateClass* tc = new TemplateClass;
	tc->prepareChildren();
	TypeArgument* ta = tc->child(TemplateClass::Parameters)->middle(0)->replace<TypeArgument>();
	ta->middle(TypeArgument::Default).place(new BuiltinType(Int));
	tc->debugTree();
	return true;
}

}
