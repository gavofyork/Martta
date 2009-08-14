/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "Position.h"
#include "Entity.h"
#include "EditDelegate.h"
#include "SpecialKeys.h"
#include "CodeScene.h"

namespace Martta
{

List<CodeScene*> CodeScene::s_allScenes;

DataFinder* DataFinder::s_this = 0;

CodeScene::CodeScene():
	m_insert						(false),
	m_insertNext					(false),
	m_insertLock					(false),
	m_reinterpretCurrentKeyEvent	(false),
	m_reinterpretCount				(0),
	m_editDelegate					(0)
{
	s_allScenes.append(this);
}

CodeScene::~CodeScene()
{
	s_allScenes.removeAll(this);
}

void CodeScene::killStrobe()
{
	m_strobeChild = 0;
	m_strobeFocus = 0;
	m_strobeCreation = 0;
	m_strobeText = String::null;
}

Entity* CodeScene::editEntity() const
{
	return editDelegate() ? m_editDelegate->subject() : 0;
}

EditDelegateFace* CodeScene::editDelegate() const
{
	if (!m_editDelegate)
		return 0;
	if (!m_editDelegate->subject())
		delete m_editDelegate;
	// Note the destructor of the edit delegate tell us that it's dead so m_editDelegate is always correct.
	return m_editDelegate;
}

void CodeScene::setEditing(Entity* _e)
{
	if (_e == editEntity())
		return;

	mDebug() << "Changing editing...";

	if (editDelegate())
	{
		SafePointer<Entity> edited = m_editDelegate->subject();
		SafePointer<Entity> cur = current();
		// If the edit delegate is half-way to destruction, we can allow it to have a null subject.
//		AssertNR(edited == cur || cur->usurpsChild(edited) || !edited);
		mDebug() << "Leaving editing...";
		if (isInScene(edited))
		{
			editDelegate()->tryCommit();
			mDebug() << "Leaving editing intact...";
			m_editDelegate->leavingEditIntact();
			// at this point, m_editDelegate and m_current may no longer be valid.
		}
		if (m_editDelegate)
		{
			delete m_editDelegate;
			relayoutLater(edited);
		}
	}

	if (_e && !_e->isFixed())
	{
		if (isFocusable(_e))
			setCurrent(_e);
		else if (_e->isUsurped())
			setCurrent(_e->parent());
		else
			return;

		_e->newDelegate(this);
		if (m_editDelegate)
		{
			m_editDelegate->initialised();
			relayoutLater(current());
		}
	}
}

void CodeScene::currentAboutToChange()
{
	leaveEdit();
}

void CodeScene::currentChanged(Entity* _to, Entity* _from)
{
	if (_from)
	{
		for (Entity* e = _from; e && _to && !_to->hasAncestor(e); e = e->parent())
			e->checkForCullingLater();
	}
}

Position CodeScene::nearestBracket(Position const& _p) const
{
	int n = UndefinedIndex;
	Position ret;
	foreach (Position i, m_bracketed)
		if (_p == i)
		{
			n = 0;
			return i;
		}
		else if (int d = _p->hasAncestor(i.entity()))
		{
			if (d < n)
			{
				n = d;
				ret = i;
			}
		}
	return ret;
}

void CodeScene::keyPressHandler(KeyEvent& _e)
{
	if (!subject() || !current())
		return;

	SafePointer<Entity> n = current();
	Position currentPoint = n->over();

	m_insertNext = false;

	KeyEvent e;

	if (m_strobeFocus && !_e.text().isEmpty() && _e.text() != L" ")
	{
		m_activeStrobe = m_strobeCreation ? m_strobeCreation->over() : m_strobeFocus->over();
		e = KeyEvent(m_strobeText + _e.text(), _e.modifiers() , &*m_strobeFocus, true, m_strobeFocus->isPlaceholder(), UndefinedIndex, this);
		Entity::keyPressEventStarter(&e);
		e.executeStrobe();
		m_activeStrobe = Nowhere;
	}

	if (!e.isAccepted())
	{
		// rejig for single key press.
		e = KeyEvent(_e.text(), _e.modifiers(), n, true, n->isPlaceholder(), UndefinedIndex, this);
		Entity::keyPressEventStarter(&e);
		if (e.isAccepted())
			killStrobe();
	}

	bool allowStrobeInit = true;

	// Navigation keys or the strobe kill key.
	if (!e.isAccepted() && (keyPressedAsNavigation(e) || e.text() == L" "))
	{
		killStrobe();
		// Navigation keys can never initialise a strobe.
		allowStrobeInit = false;
		e.accept();
	}

	if (m_reinterpretCurrentKeyEvent)
		allowStrobeInit = false;

	if (e.isAccepted())
	{
		m_insert = m_insertLock || m_insertNext;
		if (m_reinterpretCurrentKeyEvent)
		{
			m_reinterpretCurrentKeyEvent = false;
			m_reinterpretCount++;
			if (m_reinterpretCount < 3)	// Catch inf. rec.
				keyPressHandler(_e);
			else
			{
				mInfo() << &*current();
				subject()->debugTree();
				Assert(false, "Reinterpret key event in infinite recursion");
			}
			m_reinterpretCount--;
		}
	}

	if (!_e.text().isEmpty() && (wchar_t)_e.text()[0] > L' ' && allowStrobeInit)
	{
		// Add to m_strobe.
		if (!m_strobeFocus && currentPoint.exists() && n && n->parent() == currentPoint.entity())
		{
			// Kick off the strobe sequence.
			m_strobeFocus = n;
			m_strobeChild = n;
			m_strobeCreation = currentPoint.entity();
		}
		else if (!m_strobeFocus && n && e.strobeCreation() && e.strobeCreation() != n)
		{
			// Kick off the strobe sequence.
			m_strobeFocus = n;
			m_strobeCreation = e.strobeCreation();
			m_strobeChild = e.strobeChild();
		}
		else if (!m_strobeFocus && n && !e.isAccepted())
		{
			// Kick off the strobe sequence.
			m_strobeFocus = n;
			m_strobeCreation = 0;
			m_strobeChild = 0;
		}
//		if (m_strobeCreation && m_strobeFocus)
//			m_strobeChild = m_strobeCreation->child(m_strobeFocus->ancestorIndex(&*m_strobeCreation));
//		else
//			m_strobeChild = 0;
		if (m_strobeFocus)
			m_strobeText += _e.text()[0];
	}
	if (e.isAccepted())
		_e.accept();
}

bool CodeScene::keyPressedAsNavigation(KeyEvent const& _e)
{
	if (_e.text() == InsertKey)
	{
		if (m_insertLock)
		{
			m_insertLock = false;
			m_insertNext = false;
		}
		else if (m_insert)
			m_insertLock = true;
		else
			m_insertNext = true;
	}
	else if (_e.text() == L" " && editDelegate())
		setEditing(0);
	else
		return false;
	return true;
}

}
