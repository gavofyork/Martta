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

#pragma once

#include "SafePointer.h"
#include "Position.h"
#include "ConceptSupport.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

class CodeScene;
class Position;

class M_API_Concept KeyEvent
{
public:
	KeyEvent(String const& _text = String::null, int _modifiers = 0, Concept* _focus = 0, bool _isFocused = false, bool _focusIsPlaceholder = false, int _focalIndex = -1, CodeScene* _codeScene = 0);

	void			accept() { m_accepted = true; }
	bool			isAccepted() const { return m_accepted; }
	int				modifiers() const { return m_modifiers; }
	Concept*			focus() const { return m_focus; }
	String			text() const { return m_text; }
	Position		focusPoint() const;
	bool			isFocused() const { return m_isFocused; }
	bool			focusIsPlaceholder() const { return m_focusIsPlaceholder; }
	int				focalIndex() const { return m_focalIndex; }
	void			reinterpretLater() const;
	bool			isStrobe() const { return text().length() > 1; }
	CodeScene*		codeScene() const { return m_codeScene; }
	Concept*			strobeCreation() const { return m_strobeCreation; }
	Concept*			strobeChild() const { return m_strobeChild; }
	Position		nearestBracket(Position const& _p) const;
	void			executeStrobe();
	bool			isInserting() const;

	void			noteStrobeCreation(Concept* _creation, Concept* _old) const;

	void			setFocus(Concept* _f) { m_focus = _f; }
	void			setFocusIsPlaceholder(bool _p) { m_focusIsPlaceholder = _p; }
	void			setIsFocused(bool _f) { m_isFocused = _f; }
	void			setFocalIndex(int _e) { m_focalIndex = _e; }

private:
	String			m_text;
	int				m_modifiers;

	bool			m_accepted;
	Concept*			m_focus;
	bool			m_isFocused;
	bool			m_focusIsPlaceholder;
	int				m_focalIndex;
	CodeScene*		m_codeScene;
	Concept*			m_originalStrobeChild;
	Position		m_sCrPoint;
	Position		m_sChPoint;
	bool			m_strobed;
	mutable Concept*	m_strobeCreation;
	mutable SafePointer<Concept>	m_strobeChild;
};

}

