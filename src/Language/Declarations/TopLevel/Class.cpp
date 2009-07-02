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

#include "MemberEnumeration.h"
#include "Constructor.h"
#include "ArtificialDefaultConstructor.h"
#include "ArtificialCopyConstructor.h"
#include "Destructor.h"
#include "Method.h"
#include "ExplicitType.h"
#include "MemberVariable.h"
#include "Base.h"
#include "MethodOperator.h"
#include "ArtificialAssignmentOperator.h"
#include "TextLabel.h"
#include "AccessLabel.h"
#include "Const.h"
#include "Reference.h"
#include "ConversionOperator.h"
#include "Class.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Class);

Access Class::baseAccess(Class* _c) const
{
	if (_c == this)
		return Public;
	foreach (Base* i, cardinalChildrenOf<Base>())
	{
		if (i->classType() == _c)
			return i->access();
		Access a = i->classType() ? i->classType()->baseAccess(_c) : NoAccess;
		if (a == Public || a == Protected && i->access() != Public)
			return i->access(); 
		else if (a == Protected)
			return Protected;
	}
	return NoAccess;
}
	
bool Class::keyPressedOnInsertionPoint(Position const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<Class>(_p, _e, "L");
}

void Class::rejigDeps()
{
	removeAllDependencies();
	foreach (Entity* i, members())
		addDependency(i->child(Member::Accessibility));
}

Entity* Class::isExpander() const
{
	return cardinalChildCount() ? child(0) : const_cast<Class*>(this);
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
		if (i->id() == Operator::Equals && i->argumentType(0) == Type(this).topWith(Reference()) || i->argumentType(0) == Type(this).topWith(Const()).topWith(Reference()))
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
		changed();
	relayoutLater();
}

void Class::onDependencyAdded(Entity* _e)
{
	if (_e->index() >= 0 && (_e->isKind<MethodOperator>() || _e->isKind<ConversionOperator>() || _e->isKind<Constructor>()))
		if (checkImplicitConstructors())
			changed();
	if (_e->isKind<Member>())
		rejigDeps();
}

void Class::onDependencyRemoved(Entity* _e, int _oi)
{
	// TODO: Will it remove the Access label dep? Even if the member is only moved to another class?
	if (_oi >= 0 && (_e->isKind<MethodOperator>() || _e->isKind<ConversionOperator>() || _e->isKind<Constructor>()))
		if (checkImplicitConstructors())
			changed();
	if (_e->isKind<Member>())
		rejigDeps();
}

void Class::onDependencyChanged(Entity* _e)
{
	if (_e->isKind<Base>() || _e->index() >= 0 && (_e->isKind<MethodOperator>() || _e->isKind<ConversionOperator>() || _e->isKind<Constructor>()))
		if (checkImplicitConstructors())
			changed();
	if (_e->isKind<TextLabel>())
		changed(Visually);
	if (_e->isKind<AccessLabel>())
		relayoutLater();
}

Kinds Class::allowedKinds(int _i) const
{
	if (_i >= 0)
		return (Kind::of<Member>(), Kind::of<Base>());
	if (_i == Artificials)
		return Kind::of<Artificial>();
	return Super::allowedKinds(_i);
}

QString Class::implementationCode() const
{
	QString ret;
	foreach (Member* f, cardinalChildrenOf<Member>())
		ret += f->implementationCode() + "\n";
	if (ret.endsWith("\n\n"))
		ret.chop(1);
	return ret;
}

QString Class::interfaceCode() const
{
	// TODO: ordering for enums?
	QString ret;
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

QList<Declaration*> Class::utilised() const
{
	QList<Declaration*> ret;
	foreach (Base* i, cardinalChildrenOf<Base>())
		ret << i->classType();
	ret += Super::utilised();
	return ret;
}

QList<Declaration*> Class::members(bool _isConst, Access _access) const
{
	QList<Declaration*> ret;
	foreach (MemberValue* i, cardinalChildrenOf<MemberValue>())
		if ((i->isConst() || !_isConst) && i->access() <= _access)
			ret += i;
	foreach (MemberValue* i, childrenAs<MemberValue>(Artificials))
		if ((i->isConst() || !_isConst) && i->access() <= _access)
			ret += i;
	foreach (Base* i, cardinalChildrenOf<Base>())
		if (!i->classType())
			continue;
		else if (_access == Private || _access == Protected && i->access() <= Protected)
			ret << i->classType()->members(_isConst, Protected);
		else if (_access == Public && i->access() <= Public)
			ret << i->classType()->members(_isConst, Public);
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

Types Class::assignableTypes() const
{
	Types ret;
	foreach (MethodOperator* i, membersOf<MethodOperator>(false, Public))
		if (i->id() == Operator::Equals)
			ret << i->asKind<LambdaNamer>()->argumentType(0);
	return ret;
}

bool Class::keyPressed(EntityKeyEvent const* _e)
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

QString Class::defineLayout(ViewKeys& _keys) const
{
	QString ret = ("^;ycode;'class ';fb;cblack;s" + Type(const_cast<Class*>(this))->idColour() + ";!%1;s;ycode").arg(Identity);
	
	if (_keys["expanded"].toBool())
	{
		foreach (Base* i, cardinalChildrenOf<Base>())
			ret += QString(";n;i;%1").arg(i->index());
		
		ret += ";n;'{'";

		for (int i = 0; i < AccessCount; i++)
		{	
			QString mem;
			Kinds recognised;
			recognised << Kind::of<Constructor>();
			foreach (MemberLambda* f, cardinalChildrenOf<Constructor>())
				if (f->access() == Access(i))
					mem += QString(";n;%1").arg(f->index());
			recognised << Kind::of<Destructor>();
			foreach (MemberLambda* f, cardinalChildrenOf<Destructor>())
				if (f->access() == Access(i))
					mem += QString(";n;%1").arg(f->index());
			recognised << Kind::of<Method>();
			foreach (MemberLambda* f, cardinalChildrenOf<Method>())
				if (f->access() == Access(i))
					mem += QString(";n;%1").arg(f->index());
			foreach (MemberLambda* f, cardinalChildrenOf<MemberLambda>())
				if (f->access() == Access(i) && !f->Entity::isKind(recognised))
					mem += QString(";n;%1").arg(f->index());
			foreach (MemberVariable* f, cardinalChildrenOf<MemberVariable>())
				if (f->access() == Access(i))
					mem += QString(";n;%1").arg(f->index());
			foreach (MemberEnumeration* f, cardinalChildrenOf<MemberEnumeration>())
				if (f->access() == Access(i))
					mem += QString(";n;%1").arg(f->index());
			if (!mem.isEmpty())
				ret += ";n;s" + AccessLabel(Access(i)).idColour().name() + "44;'" + Martta::code(Access(i)) + "';s" + mem;
		}
		ret += ";n;'}'";
	}
	else
	{
		if (cardinalChildCountOf<Base>())
		{
			ret += ";ynormal;' ['";
			foreach (Base* i, cardinalChildrenOf<Base>())
				ret += QString(";%1").arg(i->index());
			ret += ";']'";
		}
		ret += ";yminor;' (";
		if (int n = cardinalChildCountOf<Method>())
			ret += QString::number(n) + " method" + (n > 1 ? "s, " : ", ");
		if (int n = cardinalChildCountOf<MemberVariable>())
			ret += QString::number(n) + " variable" + (n > 1 ? "s, " : ", ");
		if (int n = cardinalChildCountOf<Constructor>())
			ret += QString::number(n) + " constructor" + (n > 1 ? "s, " : ", ");
		if (int n = cardinalChildCountOf<Destructor>())
			ret += QString::number(n) + " destructor" + (n > 1 ? "s, " : ", ");
		if (int n = cardinalChildCountOf<MethodOperator>())
			ret += QString::number(n) + " operator" + (n > 1 ? "s, " : ", ");
		if (ret.endsWith(", "))
			ret.chop(2);
		ret += ")";
		if (ret.endsWith("()"))
			ret.chop(2);
		ret += "'";
	}
	return ret;
}

}
