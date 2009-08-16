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

#include <msString.h>
#include <msHash.h>
#include <msList.h>
using namespace MarttaSupport;

#include "SafePointer.h"
#include "EntitySupport.h"
#include "Position.h"

namespace Martta
{

class EditDelegateFace;
class Position;
class Entity;

class DataFinder
{
public:
	static DataFinder*			get() { return s_this ? s_this : (s_this = new DataFinder); }
	static void					set(DataFinder* _f) { delete s_this; s_this = _f; }

	virtual String				fullPathOf(String const&) { return String::null; }

private:
	static DataFinder*			s_this;
};

class CodeScene
{
public:
	CodeScene();
	virtual ~CodeScene();

	static List<CodeScene*> const&	all() { return s_allScenes; }

	// Set the current attributes.
	virtual Entity*				subject() const = 0;
	virtual void				setSubject(Entity* _subject) = 0;

	// What's happening?
	virtual Entity*				current() const = 0;
	virtual bool				isCurrent(Entity const* _e) const = 0;
	virtual bool				isFocusable(Entity const* _e) const = 0;
	virtual bool				isInScene(Entity const* _e) const = 0;
	virtual void				rememberCurrent() = 0;
	virtual void				restoreCurrent() = 0;

	// Focus changers (often make use of above).
	virtual void				setCurrent(Entity const* _e) = 0;
	virtual void				navigateInto(Entity* _centre) = 0;									/// Selects _centre's leftmost, innermost focusable child. e.g. X on ()s: (++X + 4)
	virtual void				navigateOnto(Entity* _shell) = 0;									/// Selects _shell's leftmost focusable child. e.g. ++X on ()s: (++X + 4)
	enum NavigationDirection { Forwards, Backwards };
	virtual void				navigateAway(Entity* _from, NavigationDirection _d = Forwards) = 0;	/// Selects closest focusable entity visually _d from _from. e.g. 4 on ()s: (++X + 4)
	virtual void				navigateToNew(Entity* _from) = 0;									/// Selects closest focusable sibling-owned entity visually forwards from _from, or parent if none.

	// NONVIRTUAL Bracketing code.
	void						setBracketed(Position const& _p) { m_bracketed.append(_p); }
	void						removeBracket(Position const& _p) { m_bracketed.removeAt(m_bracketed.lastIndexOf(_p)); }
	bool						isBracketed(Position const& _p) const { return m_bracketed.contains(_p); }
	Position					nearestBracket(Position const& _p) const;

	// NONVIRTUAL For EditDelegateFace.
	void						forgetMe(EditDelegateFace* _me) { if (m_editDelegate == _me) m_editDelegate = 0; }
	void						rememberMe(EditDelegateFace* _me) { AssertNR(!m_editDelegate); m_editDelegate = _me; }

	// NONVIRTUAL For strobing.
	void						killStrobe();
	void						setStrobeCreation(Entity* _e) { m_strobeCreation = _e; }
	void						setStrobeChild(Entity* _e) { m_strobeChild = _e; }
	Entity*						strobeCreation() const { return m_strobeCreation; }
	Entity*						strobeChild() const { return m_strobeChild; }

	// NONVIRTUAL For editing.
	void						setEditing(Entity* _e);
	void						leaveEdit() { if (editDelegate()) setEditing(0); }
	Entity*						editEntity() const;
	EditDelegateFace*			editDelegate() const;
	bool						isEditing(Entity const* _e) const { return _e == editEntity(); }

	/// @returns the status of the insertion flag.
	bool						insert() const { return m_insert; }

	/// Notifies us that the current key event should be recycled (i.e. handled again).
	void						reinterpretCurrentKeyEvent() { m_reinterpretCurrentKeyEvent = true; }

	// Hack - should set the current entity but not issue an update and not call any
	// focus change notifications etc. Used when leavingEditIntact since it is often called
	// ultimately from another setCurrent(). Also used in CodeView when focus needs to be
	// preserved around a DOM update (in refresh()).
	virtual void				silentlySetCurrent(Entity* _e) = 0;

	virtual bool				manageKeyPress(KeyEvent const&, Entity const*) { return false; }

	virtual void				relayout(Entity* _e) = 0;
protected:
	// Handlers to be called pre- and post-current changing.
	void						currentAboutToChange();
	void						currentChanged(Entity* _to, Entity* _from);

	void						keyPressHandler(KeyEvent& _e);
	virtual bool				keyPressedAsNavigation(KeyEvent const&);

	// State
	List<Position>				m_bracketed;

	SafePointer<Entity>			m_strobeCreation;
	SafePointer<Entity>			m_strobeChild;
	SafePointer<Entity>			m_strobeFocus;
	String						m_strobeText;
	Position					m_activeStrobe;

	bool						m_insert;
	bool						m_insertNext;
	bool						m_insertLock;

	bool						m_reinterpretCurrentKeyEvent;
	int							m_reinterpretCount;

private:
	/// Cycles through possible insert states. Call when the Insert key has been pressed.
	void						insertCycle();

	mutable EditDelegateFace*	m_editDelegate;

	static List<CodeScene*>		s_allScenes;
};

}

