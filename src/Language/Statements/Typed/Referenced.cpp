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

#include "DecorationContext.h"
#include "CommonGraphics.h"
#include "MemberLambda.h"
#include "Namespace.h"
#include "Invocation.h"
#include "MemberVariable.h"
#include "MemberOperation.h"
#include "LongMemberOperation.h"
#include "AssignmentOperation.h"
#include "Function.h"
#include "FunctionType.h"
#include "StringType.h"
#include "Memberify.h"
#include "Const.h"
#include "BasicRoot.h"
#include "EditDelegate.h"
#include "Reference.h"
#include "CodeScene.h"
#include "TextLabel.h"
#include "DefaultConstructedVariable.h"
#include "Argument.h"
#include "AssignedVariable.h"
#include "TextLabel.h"
#include "Member.h"
#include "Referenced.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Referenced);	

#define JOIN(X, Y) X ## Y
#define SET(X) JOIN(X, Set) = JOIN(X, Variables) | JOIN(X, Lambdas)

enum { LocalVariables = 1<<0, LocalLambdas = 1<<1, SET(Local),
		ArgumentVariables = 1<<2, ArgumentLambdas = 1<<3, SET(Argument),
  		MemberVariables = 1<<4, MemberLambdas = 1<<5, SET(Member),
		ScopedVariables = 1<<6, ScopedLambdas = 1<<7, SET(Scoped),
		GlobalVariables = 1U<<14, GlobalLambdas = 1U<<15, SET(Global) };
#undef SET
#undef JOIN

bool Referenced::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && QRegExp("[a-z]").exactMatch(_e->text()) && _p->isKind<Typed>() && _p->asKind<Typed>()->ourAllowedTypes().size() && _p->asKind<Typed>()->ourAllowedTypes()[0]->isType<Memberify>())
	{
		_e->reinterpretLater();
		Referenced* r = new Referenced;
		_p.place(r);
		r->m_lastSet = MemberLambdas;
		r->setEditing(_e->codeScene());
	}
	else if (_p.exists() && _p->isPlaceholder() && QRegExp("[a-zML:_]").exactMatch(_e->text()))
	{
		_e->reinterpretLater();
		Referenced* r = new Referenced;
		_p.place(r);
		r->setEditing(_e->codeScene());
	}
	else
		return false;
	return true;
}

Referenced::Referenced(ValueDefiner* _v, bool _specific):
	m_subject	(0),
	m_specific	(_specific),
	m_lastSet	(LocalSet|MemberLambdas|ScopedSet)
{
	setSubject(_v);
}

bool Referenced::isSuperfluous() const
{
	return m_subject.isNull() || Super::isSuperfluous();
}

QString Referenced::code() const
{
	if (!m_subject.isNull() && !m_specific && m_subject->isKind<Member>())
		return m_subject->codeName();
	if (!m_subject.isNull())
		return m_subject->reference();
	return QString();
}

Kinds Referenced::ancestralDependencies() const
{
	Kinds ret = Kind::of<MemberLambda>();
	if (!m_subject.isNull() && !parentIs<GenericMemberOperation>() && m_subject->isKind<MemberValue>() && hasAncestor<MemberLambda>() && ancestor<Class>() != m_subject->asKind<MemberValue>()->classType())
		ret << Kind::of<Class>();
	return ret;
}

bool Referenced::isInValidState() const
{
	// If we're not referencing anything yet, return null.
	if (!m_subject.isUsable())
		return false;
		
	if (!parentIs<GenericMemberOperation>() && m_subject->isKind<MemberValue>() && hasAncestor<MemberLambda>())
	{
		M_ASSERT(hasAncestor<Class>());
		if (!ancestor<Class>()->membersOf<MemberValue>(ancestor<MemberLambda>()->isConst()).contains(m_subject->asKind<MemberValue>()))
			return false;
	}
	return Super::isInValidState();
}

Type Referenced::type() const
{
	// If we're not referencing anything yet, return null.
	if (!m_subject.isUsable())
		return Type();
	
	Type t = m_subject->type();
	// If we're not in a member operation, check if there's some memberification that we can silently discard; 
	if (!parentIs<GenericMemberOperation>() && t->isType<Memberify>() && hasAncestor<MemberLambda>())
	{
		M_ASSERT(hasAncestor<Class>());
		// There is; check to see if we can remove it (by being in a scoped parent and assuming the "this->" precedent).
		Memberify* m = t->asType<Memberify>();
		M_ASSERT(m->isKind<Memberify>());
		if (ancestor<Class>()->baseAccess(m->scopeClass()) <= Protected)
		{
			bool memberIsCallable = m->original()->isType<FunctionType>();
			bool constScope = ancestor<MemberLambda>()->isConst();
			bool constMember = memberIsCallable ? m->isConst() : m->original()->isType<Const>();
			if (constMember || !constMember && !constScope)
			{
				// Member Variable/FunctionType inside a method. Either enclosing method is non-const or FunctionType is const.
				// Allowed.
				m->unknit();
			}
			else if (!memberIsCallable && constScope && !constMember)
			{
				// Member Variable referenced inside a const method
				// Allowed but made const.
				m->original()->knit<Const>();
				m->unknit();
			}
		}
	}
	return t;
}

void Referenced::importDom(QDomElement const& _element)
{
	Entity::importDom(_element);
//	m_subject = rootEntity()->locate<ValueDefiner>(_element.attribute("subject"));
	m_subject.restoreFrom(_element.attribute("subject"));
	m_specific = _element.attribute("specific").toInt();
	m_lastSet = _element.attribute("lastSet").toInt();
	// TODO: check if depend system needs reseting here.
}

void Referenced::exportDom(QDomElement& _element) const
{
	Entity::exportDom(_element);
	_element.setAttribute("subject", m_subject.key());
	_element.setAttribute("specific", m_specific);
	_element.setAttribute("lastSet", m_lastSet);
}

void Referenced::decorate(DecorationContext const& _c) const
{
	//TODO: Check!
	if (Entity* e = m_subject ? m_subject->self() : 0)
	{
		bool dec = false;
		if (e->hasAncestor<Namespace>())
		{
			if (e->isKind<MemberVariable>())
				dec = true;
			else if (e->isKind<Argument>())
				dec = true;
		}
		if (dec)
		{
			QRectF r(alignedForUnitPen(_c(1)));
			r.setWidth(qMin(_c(0).width(), r.height() * 2));

			QRgb c = qRgb(0, 0, 0);
		
			QRadialGradient go(_c(1).center(), r.height() * 2);
			go.setColorAt(0.f, qRgba(c, 32));
			go.setColorAt(1.f, qRgba(c, 0));
			_c->setPen(Qt::NoPen);
			_c->setBrush(go);
			_c->drawRoundRect(r, 50, 100);
		}
	}
	Super::decorate(_c);
}

QString Referenced::defineLayout(ViewKeys&) const
{
	QString ret = QString(m_lastSet&GlobalSet ? "p:/global.svg;" : "");
	if (m_subject && m_subject->isKind<MemberVariable>())
		ret += "(;M4;[[[;fs-2;fb;c#777;e#fff;'M';]]];);";
	else if (m_subject && m_subject->isKind<Argument>())
		ret += "(;M4;[[[;fs-2;fb;c#777;e#fff;'_';]]];);";
	ret += "^;s" + (m_subject ? m_subject->type()->idColour() : TypeEntity::null->idColour()) + ";c;'" + (m_subject ? m_subject->name() : QString()) + "'";
	return ret;
}

class ReferencedEdit: public EditDelegate<Referenced>
{
public:
	ReferencedEdit(Referenced* _e, CodeScene* _s);

	virtual void				leavingEditIntact();
	virtual void				commit();
	virtual bool				keyPressed(EntityKeyEvent const* _e);
	virtual bool				isValid() const;
	virtual QString				defineLayout(ViewKeys&) const;
	
private:
	void						setSubset(uint _s) { subject()->m_lastSet = _s; updateSubset(); } 
	void						updateSubset();
	void						updateCompletion();

	QString						m_completion;
	QString						m_entityName;
	ValueDefiner*				m_entity;

	// Actual scope the symbol will be in
	QList<ValueDefiner*>		m_valuesInScope;
	bool						m_immediateCommits;
};

EditDelegateFace* Referenced::newDelegate(CodeScene* _s)
{
	return new ReferencedEdit(this, _s);
}

ReferencedEdit::ReferencedEdit(Referenced* _e, CodeScene* _s): EditDelegate<Referenced>(_e, _s)
{
	m_immediateCommits = !_e->isPlaceholder();
	m_entity = subject()->subject();
	m_entityName = m_entity ? m_entity->name() : "";
	tryCommit();
	updateSubset();
}

void ReferencedEdit::leavingEditIntact()
{
	if (m_subject && m_subject->isInModel() && !isValid() && !m_entityName.isEmpty())
	{
		Entity* e = 0;
		if (m_subject->over().allowedToBeKind<DefaultConstructedVariable>())
			e = new DefaultConstructedVariable;
		else if (m_subject->over().allowedToBeKind<AssignedVariable>())
			e = new AssignedVariable;
		if (e)
		{
			e->prepareChildren();
			int x;
			if ((x = SimpleType::id(m_entityName)) != -1)
			{
				e->childOf<TypeEntity>()->replace(new SimpleType(x));
				codeScene()->silentlySetCurrent(e->child(Identifiable::Identity));	// set to the place where the user expects the cursor to be (silently, sicne we might (possibly) already be in a setCurrent!).
			}
			else if (m_entityName == "string")
			{
				e->childOf<TypeEntity>()->replace(new StringType);
				codeScene()->silentlySetCurrent(e->child(Identifiable::Identity));	// set to the place where the user expects the cursor to be (silently, sicne we might (possibly) already be in a setCurrent!).
			}
			else
			{
				M_ASSERT(e->childIs<TextLabel>(Identifiable::Identity));
				e->childAs<TextLabel>(Identifiable::Identity)->setText(m_entityName);
				codeScene()->silentlySetCurrent(e->childOf<TypeEntity>());							// set to the place where the user expects the cursor to be (silently, sicne we might (possibly) already be in a setCurrent!).
			}
			
			// set subject to zero so we don't go through this again when the kill()ed subject gets removed from the scene.
			Entity* s = m_subject;
			m_subject = 0;
			s->replace(e);
			// We have now been deleted.
		}
	}
}

QString ReferencedEdit::defineLayout(ViewKeys&) const
{
	return QString(subject()->m_lastSet&GlobalSet ? "p:/global.svg;" : "") + "^;s" + (subject()->m_subject ? subject()->m_subject->type()->idColour() : TypeEntity::null->idColour()) + ";'" + m_entityName + "';s;ygrayed;'" + m_completion + "'";
}

void ReferencedEdit::updateSubset()
{
	m_valuesInScope.clear();
	if (subject()->m_lastSet & LocalSet)
		m_valuesInScope << castEntities<ValueDefiner>(subject()->valuesInLocalScope());	// TODO: Change over
	if (subject()->m_lastSet & ScopedSet)
		m_valuesInScope << castEntities<ValueDefiner>(subject()->ancestor<Declaration>()->valuesKnown());
	if (subject()->m_lastSet & GlobalSet)
		m_valuesInScope << subject()->rootEntity()->selfAndAncestorsChildrenOf<ValueDefiner>();
	if (subject()->m_lastSet & ArgumentSet && subject()->hasAncestor<LambdaNamer>())
		for (int i = 0; i < subject()->ancestor<LambdaNamer>()->argumentCount(); i++)
			m_valuesInScope << subject()->ancestor<LambdaNamer>()->argument(i);
	if (subject()->m_lastSet & MemberSet)
	{
		Type method = Type(FunctionType(false, true)).topWith(Memberify()).topWith(Reference());
		foreach (Type t, subject()->ourAllowedTypes())
		{
			QList<ValueDefiner*> appMems;
			if (t->isType<Memberify>() && t->asType<Memberify>()->scope())
				appMems = t->asType<Memberify>()->scope()->applicableMembers(subject(), t->asType<Memberify>()->isConst());
			else if (subject()->hasAncestor<Class>())
				appMems = castEntities<ValueDefiner>(subject()->ancestor<Class>()->membersOf<MemberValue>(subject()->hasAncestor<MemberLambda>() ? subject()->ancestor<MemberLambda>()->isConst() : false));
			if (subject()->m_lastSet & MemberSet == MemberVariables)
				appMems = filterTypedsInv(method, appMems);
			else if (subject()->m_lastSet & MemberSet == MemberLambdas)
				appMems = filterTypeds(method, appMems);
			m_valuesInScope << appMems;
		}
	}
}

void ReferencedEdit::commit()
{
	subject()->setSubject(m_entity);
}

bool ReferencedEdit::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->key() == Qt::Key_Tab)
		m_entityName += m_completion;
	else if (_e->key() == Qt::Key_Backspace && m_entityName.length())
	{
		if (_e->modifiers() == Qt::ControlModifier)
			m_entityName.chop(1);
		else
		{
			int potentials = nameStarts(m_valuesInScope, m_entityName).size();
			while (nameStarts(m_valuesInScope, m_entityName).size() == potentials && m_entityName.length())
				m_entityName.chop(1);
		}
	}
	else if (_e->key() == Qt::Key_Backspace && !m_entityName.length())
		setSubset(LocalSet|MemberLambdas);
	else if (_e->text() == "_" && !m_entityName.length())
		setSubset(ArgumentSet);
	else if (_e->text() == "M" && !m_entityName.length())
		setSubset(MemberVariables);
	else if (_e->text() == "L" && !m_entityName.length())
		setSubset(LocalSet);
	else if (_e->text() == ":" && !m_entityName.length())
		setSubset(GlobalSet);
	else if (QRegExp("[a-z]").exactMatch(_e->text()) || QRegExp("[0-9_A-Z]").exactMatch(_e->text()) && m_entityName.length())
		m_entityName += _e->text().toLower();
	else if (_e->text() == " " || _e->text() == "[" || _e->text() == "#")
	{
		codeScene()->leaveEdit();
		_e->reinterpretLater();	// since we may have been deleted and replaced with e.g. a variable, we want appropriately.
		return true;
	}
	else
		return false;
	updateCompletion();
	m_entity = 0;
	if (!(m_entityName + m_completion).isEmpty())
		foreach (ValueDefiner* t, m_valuesInScope)
			if (t->name() == m_entityName + m_completion)
				m_entity = t;
	return true;
}

bool ReferencedEdit::isValid() const
{
	return m_entity;
}

void ReferencedEdit::updateCompletion()
{
	m_completion = "";
	QList<ValueDefiner*> potentials = nameStarts(m_valuesInScope, m_entityName);
	if (potentials.size() == 1)
	{
		m_completion = potentials[0]->name().mid(m_entityName.size());
		m_entityName = potentials[0]->name().left(m_entityName.size());
	}
	else if (potentials.size())
	{
		while (nameStarts(m_valuesInScope, m_entityName + m_completion).size() == potentials.size())
			m_completion += potentials[0]->name()[m_entityName.size() + m_completion.size()];
		m_completion.chop(1);
	}
}

}
