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
#include "Concept.h"
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
	m_strobeBracketed.clear();
}

void CodeScene::dropCursor(Concept* _shell)
{
	navigateOnto(_shell);
	while (!current()->isPlaceholder())
	{
		navigateAway(current());
		if (!current()->hasSelfAncestor(_shell))
		{
			setCurrent(_shell);
			return;
		}
	}
}

Concept* CodeScene::editConcept() const
{
	return editDelegate() ? m_editDelegate->concept() : 0;
}

EditDelegateFace* CodeScene::editDelegate() const
{
	if (!m_editDelegate)
		return 0;
	if (!m_editDelegate->concept())
		delete m_editDelegate;
	// Note the destructor of the edit delegate tell us that it's dead so m_editDelegate is always correct.
	return m_editDelegate;
}

void CodeScene::setEditing(Concept* _e)
{
	if (_e == editConcept())
		return;

//	mDebug() << "Changing editing...";

	if (editDelegate())
	{
		SafePointer<Concept> edited = m_editDelegate->concept();
		SafePointer<Concept> cur = current();
		// If the edit delegate is half-way to destruction, we can allow it to have a null subject.
//		AssertNR(edited == cur || cur->usurpsChild(edited) || !edited);
//		mDebug() << "Leaving editing...";
		if (isInScene(edited))
		{
			if (editDelegate())
				editDelegate()->tryCommit();
//			mDebug() << "Leaving editing intact...";
			if (editDelegate())
				editDelegate()->leavingEditIntact();
			// at this point, m_editDelegate and m_current may no longer be valid.
		}
		if (editDelegate())
		{
			delete m_editDelegate;
			relayout(edited);
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
			relayout(current());
			killStrobe();	// or we'll kill editing with the next keypress when the edit delegate doesn't accept it.
		}
	}
}

void CodeScene::currentAboutToChange()
{
	leaveEdit();
}

void CodeScene::currentChanged(Concept* _to, Concept* _from)
{
	if (_from)
	{
		for (Concept* e = _from; e && _to && !_to->hasAncestor(e); e = e->parent())
			e->checkForCullingLater();
	}
}

Position CodeScene::nearestBracket(Position const& _p) const
{
	int n = UndefinedIndex;
	Position ret;
	if (!_p.exists())
		return ret;
	foreach (Position i, m_bracketed)
		if (_p == i)
		{
			n = 0;
			return i;
		}
		else if (int d = _p->hasAncestor(i.concept()))
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
	if (!m_reinterpretCount)
		m_lastKeyPress = L"<style>h4 { margin: 0} em { color: red }</style><div style=\"font-size: 8pt; font-family: 'Trebuchet MS' Arial\">";
	else
		handleNote(L"<h4>Reinterpreting...</h4><hr/>");

	if (!subject() || !current())
	{
		handleNote(L"No subject/current: DONE<br/>");
		return;
	}

	SafePointer<Concept> n = current();
	Position currentPoint = n->over();

	handleNote(L"'<b>" + _e.text() + L"</b>' to <b>" + *n + L"</b><br/>");

	m_insertNext = false;

	KeyEvent e;

	if (m_strobeFocus && !_e.text().isEmpty() && _e.text() != L" ")
	{
		handleNote(L"<h4>Strobe? \"<b>" + m_strobeText + _e.text() + "</b>\"</h4>");
		List<Position> ob = m_bracketed;
		m_bracketed = m_strobeBracketed;
		m_activeStrobe = m_strobeCreation ? m_strobeCreation->over() : m_strobeFocus->over();
		e = KeyEvent(m_strobeText + _e.text(), _e.modifiers() , &*m_strobeFocus, true, m_strobeFocus->isPlaceholder(), UndefinedIndex, this);
		Concept::keyPressEventStarter(&e);
		e.executeStrobe();
		m_activeStrobe = Nowhere;
		if (e.isAccepted())
			handleNote(L"<em>YES</em><br/>");
		else
			m_bracketed = ob;
	}
//	else
//		if (!_e.text().isEmpty() && _e.text() != L" ")
//			mInfo() << "No strobe focus";

	if (!e.isAccepted())
	{
		// rejig for single key press.
		handleNote("<h4>Simple?</h4>");
		e = KeyEvent(_e.text(), _e.modifiers(), n, true, n->isPlaceholder(), UndefinedIndex, this);
		Concept::keyPressEventStarter(&e);
		if (e.isAccepted())
		{
			handleNote(L"<em>YES</em><br/>");
			killStrobe();
		}
	}

	bool allowStrobeInit = true;

	// Navigation keys or the strobe kill key.
	if (!e.isAccepted())
	{
		handleNote("<h4>Nav?</h4>");
		if (keyPressedAsNavigation(e) || e.text() == L" ")
		{
			killStrobe();
			// Navigation keys can never initialise a strobe.
			allowStrobeInit = false;
			e.accept();
			handleNote(L"<em>YES</em><br/>");
		}
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
				handleNote("<b><em>INFINITE RECURSION DETECTED. BAILING!</em></b><br/>");
			}
			m_reinterpretCount--;
		}
	}

	if (!_e.text().isEmpty() && (wchar_t)_e.text()[0] > L' ' && allowStrobeInit)
	{
		handleNote("<h4>Initialising strobe...</h4>");
		// Add to m_strobe.
		if (!m_strobeFocus && currentPoint.exists() && n && n->parent() == currentPoint.concept())
		{
			// Kick off the strobe sequence.
			m_strobeFocus = n;
			m_strobeChild = n;
			m_strobeCreation = currentPoint.concept();
			m_strobeBracketed = m_bracketed;
		}
		else if (!m_strobeFocus && n && e.strobeCreation() && e.strobeCreation() != n)
		{
			// Kick off the strobe sequence.
			m_strobeFocus = n;
			m_strobeCreation = e.strobeCreation();
			m_strobeChild = e.strobeChild();
			m_strobeBracketed = m_bracketed;
		}
		else if (!m_strobeFocus && n && !e.isAccepted())
		{
			// Kick off the strobe sequence.
			m_strobeFocus = n;
			m_strobeCreation = 0;
			m_strobeChild = 0;
			m_strobeBracketed = m_bracketed;
		}
		else
		{
//			if (n) n->debugTree();
//			mInfo() << "Not starting strobe" << n << (n ? n->parent() : &*n) << (currentPoint.exists() ? currentPoint.concept() : (Concept*)0) << e.strobeCreation();
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
	if (!m_reinterpretCount)
		m_lastKeyPress += L"</div>";
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
