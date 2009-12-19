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

#include "Concept.h"
#include "CodeScene.h"
#include "KeyEvent.h"

namespace Martta
{

KeyEvent::KeyEvent(String const& _text, int _modifiers, Concept* _focus, bool _isFocused, bool _focusIsPlaceholder, int _focalIndex, CodeScene* _codeScene):
	m_text					(_text),
	m_modifiers				(_modifiers),
	m_accepted				(false),
	m_focus					(_focus),
	m_isFocused				(_isFocused),
	m_focusIsPlaceholder	(_focusIsPlaceholder),
	m_focalIndex			(_focalIndex),
	m_codeScene				(_codeScene),
	m_strobed				(false),
	m_strobeCreation		(0),
	m_strobeChild			(0)
{
	if (m_codeScene && m_text.length() > 1)
	{
		m_originalStrobeChild = m_codeScene->strobeChild();
		Position sCrPoint;
		Position sChPoint;
		if (m_codeScene->strobeCreation())
		{
	//		m_strobeCreation->debugTree();
			AssertNR(m_codeScene->strobeChild());
			m_sCrPoint = m_codeScene->strobeCreation()->over();
			m_sChPoint = m_codeScene->strobeChild()->over();
			m_codeScene->strobeCreation()->silentMove(Nowhere);
			m_codeScene->strobeChild()->silentMove(m_sCrPoint);
		}
		mDebug() << "(strobe) text:" << m_text;
	}
}

bool KeyEvent::isInserting() const
{
	return m_codeScene->insert();
}

void KeyEvent::noteStrobeCreation(Concept* _creation, Concept* _old) const
{
	// Quick exit if we're not in the strobe phase of the key press handler.
	// If we don't exit, we'll end up moving/killing any active strobe's creation before the strobe has been cancelled.
	if (m_text.length() < 2 && m_codeScene->strobeChild())
		return;

	AssertNR(!m_strobed);
	SafePointer<Concept> cre = _creation;
	mDebug() << _creation;
	m_strobeCreation = _creation;
	m_strobeChild = _old;
	const_cast<KeyEvent*>(this)->m_strobed = true;	// QUICK mutable.

	// If the child wasn't replaced by something else.
	if (m_codeScene->strobeChild() == m_originalStrobeChild && m_sChPoint)	// && c because we only need to move the strobeChild if there was a strobe creation (before, anyways).
		m_codeScene->strobeChild()->notifyOfStrobe(m_codeScene->strobeCreation());

	Assert(m_codeScene->strobeCreation() != m_strobeCreation, "New strobe creation cannot be same as old strobe creation.");
	if (m_codeScene->strobeCreation() && m_codeScene->strobeCreation() != m_strobeCreation)
	{
		// Notifications?
		m_codeScene->strobeCreation()->killAndDelete();
	}

	if (m_strobeChild)
	{
		m_codeScene->setStrobeCreation(m_strobeCreation);
		m_codeScene->setStrobeChild(m_strobeChild);	// CRASH Could have been deleted - the case when you do 'i++' in an empty main().
	}
	else
	{
		// Strobe child died - cancel strobe and issue warning (can't strobe further now).
		m_codeScene->killStrobe();
		mCritical() << "ERROR: Strober killed strobe child so cannot continue strobing.";
	}
}

void KeyEvent::executeStrobe()
{
	if (!m_strobed && m_text.length() > 1)
	{
		if (m_sCrPoint.exists() && m_sCrPoint.entity() != m_originalStrobeChild)
		{
			// We strobed without saying anything
			mInfo() << "Strobe with no noteStrobeCreation";
			noteStrobeCreation(m_sCrPoint.entity(), m_codeScene->strobeChild());
		}
	}
	if (m_text.length() > 1 && m_sCrPoint && m_sChPoint)
	{
		m_codeScene->strobeChild()->silentMove(m_sChPoint);
		m_codeScene->strobeCreation()->silentMove(m_sCrPoint);
	}
}

void KeyEvent::reinterpretLater() const
{
	m_codeScene->reinterpretCurrentKeyEvent();
}

Position KeyEvent::focusPoint() const
{
	return m_focus->over();
}

Position KeyEvent::nearestBracket(Position const& _p) const
{
	return m_codeScene->nearestBracket(_p);
}

}
