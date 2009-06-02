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

#include <QtXml>

#include "CodeScene.h"
#include "Class.h"
#include "Enumeration.h"
#include "Const.h"
#include "Constructor.h"
#include "Base.h"
#include "ConversionOperator.h"
#include "VirtualPure.h"
#include "ExplicitType.h"
#include "CompletionDelegate.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ExplicitType);

bool ExplicitType::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && QRegExp("[A-Z:]").exactMatch(_e->text()))
	{
		_e->reinterpretLater();
		// switch to Explicit Type.
		Entity* e = _p.place(new ExplicitType);
		e->setEditing(_e->codeScene());
	}
	else
		return false;
	return true;
}

Types ExplicitType::assignableTypes() const
{
	return m_subject ? m_subject->assignableTypes() : Types();
}

QList<ValueDefinition*> ExplicitType::applicableMembers(Entity* _s, bool _isConst) const
{
	Access a = Public;
	if (_s->ancestor<Class>() && _s->ancestor<Class>() == m_subject)
		a = Private;
	else if (m_subject && m_subject->isKind<Class>() && m_subject->asKind<Class>()->baseAccess(_s->ancestor<Class>()) <= Protected)
		a = Protected;
	if (Class* c = m_subject ? m_subject->asKind<Class>() : 0)
		return c->membersOf<ValueDefinition>(_isConst, a);
	return QList<ValueDefinition*>();
}

bool ExplicitType::haveSingleCastOperator(TypeEntity const* _t, bool _const) const
{
	if (!m_subject)
		return false;
	
	bool gotOne = false;
	bool dupe = false;

	if (Class* c = m_subject->isKind<Class>() ? m_subject->asKind<Class>() : 0)
	{
		bool whackedConstForBest = false;
		foreach (MemberCallable* i, c->membersOf<ConversionOperator>(_const, Public))
		{	
			bool b = i->returns()->isSimilarTo(_t, FairlyConvertible);
			if (b && (!gotOne || gotOne && (i->isConst() == _const) && whackedConstForBest))
				gotOne = true, dupe = false, whackedConstForBest = (i->isConst() != _const);
			else if (b && gotOne)
				dupe = true;
		}
	}
	else
	{
		gotOne = Type(*_t) == Type(*this);
	}
	if (gotOne && !dupe)
		return true;
	return false;
}

bool ExplicitType::hasDefaultConstructor() const
{
	if (!subject())
		return false;
	return subject()->hasDefaultConstructor();
}

bool ExplicitType::haveSingleConversionConstructor(TypeEntity const* _f) const
{
	if (!m_subject)
		return Unrelated;
	
	Class* c = m_subject->isKind<Class>() ? m_subject->asKind<Class>() : 0;

	if (c)
	{
		bool gotOne = false;
		foreach (MemberCallable* i, c->membersOf<Constructor>(false, Public))
			if (i->argumentCount() == 1 && i->isValid())
				if (_f->isSimilarTo(&*i->argumentType(0), FairlyConvertible))
				{
					if (!gotOne)
						gotOne = true;
					else
						return false;
				}
		if (gotOne)
			return true;
	}
	return false;
}

bool ExplicitType::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	if (m_subject)
	{
		if (_c == Convertible && haveSingleCastOperator(_t))
			return true;
	
		if (Class* c = m_subject->tryKind<Class>())
		{
			if (_c == Physical && _t->isKind<ExplicitType>())
			{
				TypeDefinition* ts = _t->asKind<ExplicitType>()->m_subject;
				if (c == ts)
					return true;
				// Note Physical attribute should be tested last.
				QList<Base*> bases = c->entitiesOf<Base>();
				while (bases.size())
				{
					Base* b = bases.takeLast();
					if (ts && b->classType() == ts)
						return true;
					else if (b->classType())
						bases += b->classType()->entitiesOf<Base>();
				}
			}
		}
		else if (Enumeration* e = m_subject->tryKind<Enumeration>())
		{
			if (_t->isKind<ExplicitType>())
			{
				TypeDefinition* ts = _t->asKind<ExplicitType>()->m_subject;
				if (e == ts)
					return true;
			}
		}
	}
	return Super::defineSimilarityTo(_t, _c);
}

bool ExplicitType::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	// TODO: only non-explicit
	return m_subject && _c == Convertible && haveSingleConversionConstructor(_f) ||
			Super::defineSimilarityFrom(_f, _c);
}

QString ExplicitType::idColour() const
{
	return "#f77";
}

bool ExplicitType::canStandAlone() const
{
	return m_subject.isUsable() && m_subject->isKind<Class>() ? !m_subject->entitiesOf<VirtualPure>().size() : true;
}

bool ExplicitType::keyPressed(EntityKeyEvent const* _e)
{
	if (QRegExp("[A-Z:]").exactMatch(_e->text()))
	{
		_e->reinterpretLater();
		setEditing(_e->codeScene());
	}
	else
		return Super::keyPressed(_e);
	return true;
}

QString ExplicitType::defineLayout(ViewKeys&) const
{
	return "^;fb;s" + idColour() + ";'" + (m_subject.isUsable() ? m_subject->name() : "[]") + "'";
}

bool ExplicitType::isSuperfluous() const
{
	return context()->allowedKinds(contextIndex()).commonBase() != kind() && m_subject.isNull();
}

QList<TypeDefinition*> ExplicitType::possibilities()
{
	QList<TypeDefinition*> ret;
	TypeDefinition* old = m_subject;
	foreach (TypeDefinition* i, context()->entitiesHereAndBeforeOf<TypeDefinition>())
	{
		m_subject = i;
		if (context()->isChildInValidState(contextIndex()))
			ret << i;
	}
	m_subject = old;
	return ret;
}

QString ExplicitType::defineEditLayout(ViewKeys&, TypeDefinition*) const
{
	return "fb;s" + idColour() + ";^;%1";
}

EditDelegateFace* ExplicitType::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<ExplicitType, TypeDefinition*>(this, _s);
}

void ExplicitType::importDom(QDomElement const& _element)
{
	Super::importDom(_element);
	m_subject = locateEntity<TypeDefinition>(_element.attribute("subject"));
}

void ExplicitType::exportDom(QDomElement& _element) const
{
	Super::exportDom(_element);
	_element.setAttribute("subject", m_subject->key());
}

}
