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

#include <msStringList.h>
using namespace MarttaSupport;

#include "VirtualPure.h"	// < interface away to VirtualPure::whacksParent() const { return true; } from Member::whacksParent() const { return false; }?

#include "MemberVariable.h"
#include "MemberEnumeration.h"
#include "Method.h"
#include "Base.h"

#include "Constructor.h"
#include "Destructor.h"
#include "MethodOperator.h"
#include "ConversionOperator.h"
#include "ArtificialDefaultConstructor.h"
#include "ArtificialCopyConstructor.h"
#include "ArtificialAssignmentOperator.h"

#include "TextLabel.h"
#include "AccessLabel.h"
#include "Const.h"
#include "Reference.h"
#include "ExplicitType.h"
#include "Class.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Class);
MARTTA_REGISTER_CSS(Class, ".Class-label { color: #000; font-weight: bold; text-shadow: -1px -1px 0px #f77; }");
MARTTA_NAMED_CPP(Class, Artificials);

Access Class::baseAccess(Class* _c) const
{
	if (_c == this)
		return Public;
	foreach (Base* i, cardinalChildrenOf<Base>())
	{
		if (i->classType() == _c)
			return i->access();
		Access a = i->classType() ? i->classType()->baseAccess(_c) : NoAccess;
		if (a == Public || (a == Protected && i->access() != Public))
			return i->access();
		else if (a == Protected)
			return Protected;
	}
	return NoAccess;
}

bool Class::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<Class>(_p, _e, "L");
}

void Class::rejigDeps()
{
	removeAllDependencies();
	foreach (Concept* i, members())
		if (i->child(Member::Accessibility))
			addDependency(i->child(Member::Accessibility));
}

bool Class::checkImplicitConstructors()
{
	bool ret = false;

	// Check default constructors.
	int nonImplicits = cardinalChildCountOf<Constructor>();
	if (!nonImplicits && !childCountOf<ArtificialDefaultConstructor>(Artificials))
	{
		// Don't have one anymore; create a ArtificialDefaultConstructor, then return true to allow any dependents to rejig themselves.
		(new ArtificialDefaultConstructor)->silentMove(middle(Artificials));
		ret = true;
	}
	else if (nonImplicits && childCountOf<ArtificialDefaultConstructor>(Artificials))
	{
		// TODO: Notifications?
		// TODO: work out a replacement default constructor if there is one and put it as a substitute
		Constructor* realDefaultConstructor = 0;
		childOf<ArtificialDefaultConstructor>(Artificials)->killAndDelete(realDefaultConstructor);
		ret = true;
	}

	// Check copy constructors.
	int artificialCCs = childCountOf<ArtificialCopyConstructor>(Artificials);
	Constructor* realCC = 0;
	foreach (Constructor* i, cardinalChildrenOf<Constructor>())
		if (i->argumentCount() == 1 && (i->argumentType(0) == Type(this).topWith(Reference()) || i->argumentType(0) == Type(this).topWith(Const()).topWith(Reference())))
		{
			realCC = i;
			break;
		}
	if (!realCC && !artificialCCs)
	{
		// Don't have one anymore; create a ArtificialCopyConstructor, then return true to allow any dependents to rejig themselves.
		middle(Artificials).insertSilent(new ArtificialCopyConstructor);
		ret = true;
	}
	else if (realCC && artificialCCs)
	{
		// TODO: Notifications?
		childOf<ArtificialCopyConstructor>(Artificials)->killAndDelete(realCC);
		ret = true;
	}

	// Check assignment operators.
	int artificialAOs = childCountOf<ArtificialAssignmentOperator>(Artificials);
	MethodOperator* realAO = 0;
	foreach (MethodOperator* i, cardinalChildrenOf<MethodOperator>())
		if ((i->id() == Operator::Equals && i->argumentType(0) == Type(this).topWith(Reference())) || i->argumentType(0) == Type(this).topWith(Const()).topWith(Reference()))
		{
			realAO = i;
			break;
		}
	if (!realAO && !artificialAOs)
	{
		// Don't have one anymore; create an ArtificialAssignmentOperator, then return true to allow any dependents to rejig themselves.
		// TODO Should really put this in dependencyRemoved so that on deletion of the realAO it substitutes the Artificial one in.
		middle(Artificials).insertSilent(new ArtificialAssignmentOperator);
		ret = true;
	}
	else if (realAO && artificialAOs)
	{
		// TODO: Notifications? Looked aftre by the imminent changed() perhaps?
		childOf<ArtificialAssignmentOperator>(Artificials)->killAndDelete(realAO);
		ret = true;
	}
	return ret;
}

void Class::onChildrenInitialised()
{
	rejigDeps();
	if (checkImplicitConstructors())
		changed(Logically);
}

void Class::onDependencyAdded(Concept* _e)
{
	if (((_e->index() >= 0 && (_e->isKind<MethodOperator>() || _e->isKind<ConversionOperator>() || _e->isKind<Constructor>())) && checkImplicitConstructors())
		|| updateWhacked())
		changed(Logically);
	if (_e->isKind<Member>())
		rejigDeps();
}

void Class::onDependencyRemoved(Concept* _e, int _oi)
{
	// TODO: Will it remove the Access label dep? Even if the member is only moved to another class?
	if (((_oi >= 0 && (_e->isKind<MethodOperator>() || _e->isKind<ConversionOperator>() || _e->isKind<Constructor>())) && checkImplicitConstructors())
		|| updateWhacked())
		changed(Logically);
	if (_e->isKind<Member>())
		rejigDeps();
}

void Class::onDependencyChanged(int, Concept* _e)
{
	if (((_e->isKind<Base>() || (_e->index() >= 0 && (_e->isKind<MethodOperator>() || _e->isKind<ConversionOperator>() || _e->isKind<Constructor>()))) && checkImplicitConstructors())
		|| updateWhacked())
		changed(Logically);
	if (_e->isKind<TextLabel>() || _e->isKind<AccessLabel>())
		changed(Visually);
}

bool Class::updateWhacked() const
{
	int owc = m_whackerCount;
	m_whackerCount = 0;
	foreach (Member* i, members(false, NoAccess))
		if (i->whacksContainer())
			m_whackerCount++;
	return m_whackerCount != owc;
}

String Class::informationHtml() const
{
	Pairs p(L"Class", true);
	p << L"Whacking members:" << String::number(m_whackerCount);
	p << L"All visible members:" << String::number(members(false, NoAccess).count());
	return Super::informationHtml() + p;
}

Kinds Class::allowedKinds(int _i) const
{
	if (_i >= 0)
		return (Kind::of<Member>(), Kind::of<Base>());
	if (_i == Artificials)
		return Kind::of<Artificial>();
	return Super::allowedKinds(_i);
}

String Class::implementationCode() const
{
	String ret;
	foreach (Member* f, cardinalChildrenOf<Member>())
		ret += f->implementationCode() + "\n";
	if (ret.endsWith("\n\n"))
		ret.chop(1);
	return ret;
}

String Class::interfaceCode() const
{
	// TODO: ordering for enums?
	String ret;
	ret += "class " + codeName() + "\n";
	if (cardinalChildCountOf<Base>())
		ret += ":";
	foreach (Base* f, cardinalChildrenOf<Base>())
		ret += f->code() + ", ";
	if (ret.endsWith(", ")) ret.chop(2);
	ret += "{\n";
	foreach (Member* f, cardinalChildrenOf<Member>())
		ret += f->interfaceCode();
	if (ret.endsWith("\n\n")) ret.chop(1);
	ret += "};\n";
	return ret;
}

List<Declaration*> Class::utilised() const
{
	List<Declaration*> ret;
	foreach (Base* i, cardinalChildrenOf<Base>())
		ret << i->classType();
	ret += Super::utilised();
	return ret;
}

List<Member*> Class::members(bool _isConst, Access _access) const
{
	List<Member*> ret;
	foreach (Member* i, cardinalChildrenOf<Member>())
		if (i->access() <= _access && (!i->isKind<MemberValue>() || i->asKind<MemberValue>()->isConst() || !_isConst))
			ret += i;
	foreach (MemberValue* i, childrenAs<MemberValue>(Artificials))
		if ((i->isConst() || !_isConst) && i->access() <= _access)
			ret += i;
	StringList names;
	foreach (Member* d, ret)
		names << d->name();

	List<Member*> buf;
	foreach (Base* i, cardinalChildrenOf<Base>())
	{
		if (!i->classType())
			continue;
		else if (_access == NoAccess)
			buf = i->classType()->members(_isConst, NoAccess);
		else if (_access == Private || (_access == Protected && i->access() <= Protected))
			buf = i->classType()->members(_isConst, Protected);
		else if (_access == Public && i->access() <= Public)
			buf = i->classType()->members(_isConst, Public);
		StringList tba;
		foreach (Member* d, filterEntitiesInv<Constructor>(buf))
			if (!names.contains(d->name()))	// only for equivalent params?
			{	tba << d->name();
				ret << d;
			}
		names += tba;
	}
	return ret;
}

bool Class::hasDefaultConstructor() const
{
	int count = 0;
	foreach (LambdaNamer* i, membersOf<Constructor>())
		if (i->argumentCount() == 0)
			count++;
	return count == 1;
}

bool Class::hasSingleCastOperator(TypeConcept const* _t, bool _const) const
{
	bool gotOne = false;
	bool dupe = false;
	bool whackedConstForBest = false;
	foreach (MemberLambda* i, membersOf<ConversionOperator>(_const, Public))
	{
		bool b = i->returns()->isSimilarTo(_t, TypeConcept::FairlyConvertible);
		if (b && (!gotOne || (gotOne && i->isConst() == _const && whackedConstForBest)))
			gotOne = true, dupe = false, whackedConstForBest = (i->isConst() != _const);
		else if (b && gotOne)
			dupe = true;
	}
	if (gotOne && !dupe)
		return true;
	return false;
}


bool Class::hasSingleConversionConstructor(TypeConcept const* _f) const
{
	bool gotOne = false;
	foreach (MemberLambda* i, membersOf<Constructor>(false, Public))
		if (i->argumentCount() == 1 && i->isValid())
			if (_f->isSimilarTo(&*i->argumentType(0), TypeConcept::FairlyConvertible))
			{
				if (!gotOne)
					gotOne = true;
				else
					return false;
			}
	return gotOne;
}

bool Class::defineSimilarityTo(TypeConcept const* _t, TypeConcept::Castability _c) const
{
	if (_c == TypeConcept::Physical && _t->isKind<ExplicitType>())
	{
		TypeDefinition* ts = _t->asKind<ExplicitType>()->subject();
		// Note Physical attribute should be tested last.
		List<Base*> bases = cardinalChildrenOf<Base>();
		while (bases.size())
		{
			Base* b = bases.takeLast();
			if (ts && b->classType() == ts)
				return true;
			else if (b->classType())
				bases += b->classType()->cardinalChildrenOf<Base>();
		}
	}
	return Super::defineSimilarityTo(_t, _c);
}

Types Class::assignableTypes() const
{
	Types ret;
	foreach (MethodOperator* i, membersOf<MethodOperator>(false, Public))
		if (i->id() == Operator::Equals)
			ret << i->asKind<LambdaNamer>()->argumentType(0);
	return ret;
}

List<ValueDefiner*> Class::applicableMembers(Concept const* _s, bool _isConst) const
{
	Access a = Public;
	Class* classScope = _s->ancestor<Class>();
	if (classScope == this)
		a = Private;
	else if (baseAccess(classScope) <= Protected)
		a = Protected;
	return membersOf<ValueDefiner>(_isConst, a);
}

bool Class::canStandAlone() const
{
	return !m_whackerCount;
}

bool Class::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == "H")
	{
		setCurrent();
	}
	else if (attemptAppend(_e))
	{}
	else
		return Super::keyPressed(_e);
	return true;
}

String Class::defineHtml() const
{
	String ret = L"<^><div><span class=\"keyword\">class</span> " + toHtml(child(Identity));
	ret += L"<div class=\"block\">";
	ret += toHtml(castEntities<Concept>(cardinalChildrenOf<Base>()), L"", L"div");
	ret += L"<div class=\"deblock minor symbol\">{</div>";
	for (int i = 0; i < AccessCount; i++)
	{
		String mem;
		Kinds recognised;
		recognised << Kind::of<Constructor>();
		foreach (MemberLambda* f, cardinalChildrenOf<Constructor>())
			if (f->access() == Access(i))
				mem += toHtml(f, L"div");
		recognised << Kind::of<Destructor>();
		foreach (MemberLambda* f, cardinalChildrenOf<Destructor>())
			if (f->access() == Access(i))
				mem += toHtml(f, L"div");
		recognised << Kind::of<Method>();
		foreach (MemberLambda* f, cardinalChildrenOf<Method>())
			if (f->access() == Access(i))
				mem += toHtml(f, L"div");
		foreach (MemberLambda* f, cardinalChildrenOf<MemberLambda>())
			if (f->access() == Access(i) && !f->Concept::isKind(recognised))
				mem += toHtml(f, L"div");
		foreach (MemberVariable* f, cardinalChildrenOf<MemberVariable>())
			if (f->access() == Access(i))
				mem += toHtml(f, L"div");
		foreach (MemberEnumeration* f, cardinalChildrenOf<MemberEnumeration>())
			if (f->access() == Access(i))
				mem += toHtml(f, L"div");
		if (!mem.isEmpty())
			ret += String(L"<div class=\"deblock AccessLabel-%1\">%1</div><div class=\"AccessLabel-%1block\">%2</div>").arg(Martta::code(Access(i))).arg(mem);
	}
	ret += L"<div class=\"deblock minor symbol\">}</div>";
	ret += L"</div></div>";

/*	if (cardinalChildCountOf<Base>())
	{
		ret += ";ynormal;' ['";
		foreach (Base* i, cardinalChildrenOf<Base>())
			ret += String(";%1").arg(i->index());
		ret += ";']'";
	}
	ret += ";yminor;' (";
	if (int n = cardinalChildCountOf<Method>())
		ret += String::number(n) + " method" + (n > 1 ? "s, " : ", ");
	if (int n = cardinalChildCountOf<MemberVariable>())
		ret += String::number(n) + " variable" + (n > 1 ? "s, " : ", ");
	if (int n = cardinalChildCountOf<Constructor>())
		ret += String::number(n) + " constructor" + (n > 1 ? "s, " : ", ");
	if (int n = cardinalChildCountOf<Destructor>())
		ret += String::number(n) + " destructor" + (n > 1 ? "s, " : ", ");
	if (int n = cardinalChildCountOf<MethodOperator>())
		ret += String::number(n) + " operator" + (n > 1 ? "s, " : ", ");
	if (ret.endsWith(", "))
		ret.chop(2);
	ret += ")";
	if (ret.endsWith("()"))
		ret.chop(2);
	ret += "'";*/
	return ret;
}

}
