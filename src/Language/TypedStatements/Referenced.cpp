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

#include "Invocation.h"
#include "MemberCallable.h"
#include "MemberVariable.h"
#include "MemberOperation.h"
#include "LongMemberOperation.h"
#include "AssignmentOperation.h"
#include "Function.h"
#include "FunctionType.h"
#include "StringType.h"
#include "Memberify.h"
#include "Const.h"
#include "RootEntity.h"
#include "EditDelegate.h"
#include "Reference.h"
#include "CodeScene.h"
#include "TextLabel.h"
#include "DefaultConstructedVariable.h"
#include "AssignedVariable.h"
#include "TextLabel.h"
#include "Referenced.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Referenced);	

enum { LocalVariables = 1, LocalCallables = 2, LocalSet = 3,
		ArgumentVariables = 4, ArgumentCallables = 8, ArgumentSet = 12,
  		MemberVariables = 16, MemberCallables = 32, MemberSet = 48,
		GlobalVariables = 64, GlobalCallables = 128, GlobalSet = 192 };

bool Referenced::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && QRegExp("[a-z]").exactMatch(_e->text()) && _p->isKind<Typed>() && _p->asKind<Typed>()->allowedTypes().size() && _p->asKind<Typed>()->allowedTypes()[0]->isType<Memberify>())
	{
		_e->reinterpretLater();
		Referenced* r = new Referenced;
		_p.place(r);
		r->m_lastSet = MemberCallables;
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

Referenced::Referenced(ValueDefinition* _v, bool _specific):
	m_subject	(0),
	m_specific	(_specific),
	m_lastSet	(LocalSet|MemberCallables)
{
	setSubject(_v);
}

bool Referenced::isSuperfluous() const
{
	return m_subject.isNull() || Super::isSuperfluous();
}

bool Referenced::onChanged()
{
	Super::onChanged();
	checkForCullingLater();
	return true;
}

QString Referenced::code() const
{
	if (!m_subject.isNull() && !m_specific && (m_subject->isKind<MemberCallable>() || m_subject->isKind<MemberVariable>()))
		return m_subject->codeName();
	if (!m_subject.isNull())
		return m_subject->reference();
	return QString();
}

Kinds Referenced::ancestralDependencies() const
{
	return Kind::of<MemberCallable>();
}

Type Referenced::type() const
{
	// If we're not referencing anything yet, return null.
	if (!m_subject.isUsable())
		return Type();
	
	Type t = m_subject->type();
	// If we're not in a member operation, check if there's some memberification that we can silently discard; 
	if (!contextIs<LongMemberOperation>() && !contextIs<MemberOperation>() && t->isType<Memberify>() && hasAncestor<MemberCallable>())
	{
		M_ASSERT(hasAncestor<Class>());
		// There is; check to see if we can remove it (by being in a scoped context and assuming the "this->" precedent).
		Memberify* m = t->asType<Memberify>();
		M_ASSERT(m->isKind(Kind::of<Memberify>()));
		if (m->scopeClass() == ancestor<Class>())
		{
			bool memberIsCallable = m->child()->isType<FunctionType>();
			bool constScope = ancestor<MemberCallable>()->isConst();
			bool constMember = memberIsCallable ? m->isConst() : m->child()->isType<Const>();
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
				m->child()->knit<Const>();
				m->unknit();
			}
		}
	}
	return t;
}

void Referenced::importDom(QDomElement const& _element)
{
	Entity::importDom(_element);
	m_subject = locateEntity<ValueDefinition>(_element.attribute("subject"));
	m_specific = _element.attribute("specific").toInt();
	m_lastSet = _element.attribute("lastSet").toInt();
	// TODO: check if depend system needs reseting here.
}

void Referenced::exportDom(QDomElement& _element) const
{
	Entity::exportDom(_element);
	_element.setAttribute("subject", m_subject->key());
	_element.setAttribute("specific", m_specific);
	_element.setAttribute("lastSet", m_lastSet);
}

QString Referenced::defineLayout(ViewKeys&) const
{
	return QString(m_lastSet&GlobalSet ? "p:/global.svg;" : "") + "^;s" + (m_subject ? m_subject->type()->idColour() : TypeEntity::null->idColour()) + ";c;'" + (m_subject ? m_subject->name() : QString()) + "'";
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
	void						setSubset(int _s) { subject()->m_lastSet = _s; updateSubset(); } 
	void						updateSubset();
	void						updateCompletion();

	QString						m_completion;
	QString						m_entityName;
	ValueDefinition*			m_entity;

	// Actual scope the symbol will be in
	QList<ValueDefinition*>		m_valuesInScope;
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
				e->entity(0)->entity(1)->replace(new SimpleType(x));
				codeScene()->silentlySetCurrent(e->entity(0)->entity(1));	// set to the place where the user expects the cursor to be (silently, sicne we might (possibly) already be in a setCurrent!).
			}
			else if (m_entityName == "string")
			{
				e->entity(0)->entity(1)->replace(new StringType);
				codeScene()->silentlySetCurrent(e->entity(0)->entity(1));	// set to the place where the user expects the cursor to be (silently, sicne we might (possibly) already be in a setCurrent!).
			}
			else
			{
				e->entity(0)->entityAs<TextLabel>(0)->setText(m_entityName);
				codeScene()->silentlySetCurrent(e->entity(0));				// set to the place where the user expects the cursor to be (silently, sicne we might (possibly) already be in a setCurrent!).
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
		m_valuesInScope << subject()->valuesInLocalScope();
	if (subject()->m_lastSet & GlobalSet)
		m_valuesInScope << subject()->rootEntity()->entitiesHereAndBeforeOf<ValueDefinition>();
	if (subject()->m_lastSet & ArgumentSet && subject()->hasAncestor<Callable>())
		for (int i = 0; i < subject()->ancestor<Callable>()->argumentCount(); i++)
			m_valuesInScope << subject()->ancestor<Callable>()->argument(i);
	if (subject()->m_lastSet & MemberVariables)
		foreach (Type t, subject()->allowedTypes())
			if (t->isType<Memberify>() && t->asType<Memberify>()->scope())
				m_valuesInScope << filterTypedsInv(Type(FunctionType(false, true)).topWith(Memberify()).topWith(Reference()), t->asType<Memberify>()->scope()->applicableMembers(subject(), t->asType<Memberify>()->isConst()));
			else if (subject()->hasAncestor<Class>())
				m_valuesInScope << castEntities<ValueDefinition>(subject()->ancestor<Class>()->membersOf<MemberVariable>(subject()->hasAncestor<MemberCallable>() ? subject()->ancestor<MemberCallable>()->isConst() : false));
	if (subject()->m_lastSet & MemberCallables)
		foreach (Type t, subject()->allowedTypes())
			if (t->isType<Memberify>() && t->asType<Memberify>()->scope())
				m_valuesInScope << filterTypeds(Type(FunctionType(false, true)).topWith(Memberify()).topWith(Reference()), t->asType<Memberify>()->scope()->applicableMembers(subject(), t->asType<Memberify>()->isConst()));
			else if (subject()->hasAncestor<Class>())
				m_valuesInScope << castEntities<ValueDefinition>(subject()->ancestor<Class>()->membersOf<MemberCallable>(subject()->hasAncestor<MemberCallable>() ? subject()->ancestor<MemberCallable>()->isConst() : false));
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
		setSubset(LocalSet|MemberCallables);
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
		foreach (ValueDefinition* t, m_valuesInScope)
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
	QList<ValueDefinition*> potentials = nameStarts(m_valuesInScope, m_entityName);
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
