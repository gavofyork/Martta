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

#include <QKeyEvent>

#include "Position.h"

namespace Martta
{
	
class Entity;
class CodeScene;

class EntityKeyEvent: public QKeyEvent
{
public:
	EntityKeyEvent(QKeyEvent const& _e, Entity* _focus, bool _isFocused, bool _focusIsPlaceholder, int _focalIndex, CodeScene* _codeScene):
		QKeyEvent				(_e),
		m_focus					(_focus),
		m_isFocused				(_isFocused),
		m_focusIsPlaceholder	(_focusIsPlaceholder),
		m_focalIndex			(_focalIndex),
		m_codeScene				(_codeScene),
		m_strobeCreation		(0),
		m_strobeChild			(0)
	{
	}
	EntityKeyEvent(QKeyEvent const& _e, QString const& _strobe, Entity* _focus, bool _isFocused, bool _focusIsPlaceholder, int _focalIndex, CodeScene* _codeScene):
	QKeyEvent				(QEvent::ShortcutOverride, 0, _e.modifiers(), _strobe + _e.text(), _e.isAutoRepeat(), (_strobe + _e.text()).length()),
		m_focus					(_focus),
		m_isFocused				(_isFocused),
		m_focusIsPlaceholder	(_focusIsPlaceholder),
		m_focalIndex			(_focalIndex),
		m_codeScene				(_codeScene),
		m_strobeCreation		(0),
		m_strobeChild			(0)
	{
	}

	Entity*			focus() const { return m_focus; }
	Position	focusPoint() const;
	bool			isFocused() const { return m_isFocused; }
	bool			focusIsPlaceholder() const { return m_focusIsPlaceholder; }
	int				focalIndex() const { return m_focalIndex; }
	void			reinterpretLater() const;
	bool			isStrobe() const { return text().length() > 1; }
	CodeScene*		codeScene() const { return m_codeScene; }
	Entity*			strobeCreation() const { return m_strobeCreation; }
	Entity*			strobeChild() const { return m_strobeChild; }
	Position	nearestBracket(Position const& _p) const;

	void			noteStrobeCreation(Entity* _creation, Entity* _old) const { M_ASSERT(!m_strobeCreation); m_strobeCreation = _creation; m_strobeChild = _old; }
	
	void			setFocus(Entity* _f) { m_focus = _f; }
	void			setFocusIsPlaceholder(bool _p) { m_focusIsPlaceholder = _p; }
	void			setIsFocused(bool _f) { m_isFocused = _f; }
	void			setFocalIndex(int _e) { m_focalIndex = _e; }
	
private:
	Entity*			m_focus;
	bool			m_isFocused;
	bool			m_focusIsPlaceholder;
	int				m_focalIndex;
	CodeScene*		m_codeScene;
	mutable Entity*	m_strobeCreation;
	mutable SafePointer<Entity>	m_strobeChild;
};

}

