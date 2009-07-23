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
using MarttaSupport::StringList;
using MarttaSupport::String;
using MarttaSupport::Hash;
using MarttaSupport::List;

#include "EntitySupport.h"

namespace Martta
{

class EditDelegateFace;
class Position;
class Entity;
class Stylist;

class CodeScene
{
public:
	CodeScene();
	virtual ~CodeScene();

	static List<CodeScene*> const&	all() { return s_allScenes; }

	// Set the current attributes.
	virtual Entity*				subject() const = 0;
	virtual void				setSubject(Entity* _subject) = 0;

	// Stylist
	virtual Stylist*			stylist() const = 0;
	virtual void				setStylist(Stylist* _s) = 0;

	// What's happening?
	virtual Entity*				current() const = 0;
	virtual Entity*				editEntity() const = 0;
	virtual EditDelegateFace*	editDelegate() const = 0;
	virtual bool				isCurrent(Entity* _e) const = 0;
	virtual bool				isEditing(Entity* _e) const = 0;
	virtual bool				isInScene(Entity* _e) const = 0;
	virtual bool				isFocusable(Entity* _e) const = 0;

	// Relative determiners.
	virtual Entity*				next(Entity* _e) const = 0;
	virtual Entity*				previous(Entity* _e) const = 0;
	virtual Entity*				traverse(Entity* _e, bool _upwards, float _x) = 0;
	virtual Entity*				nearest(Entity* _e) = 0;

	// Focus changers (often make use of above).
	virtual void				setCurrent(Entity* _e) = 0;
	virtual void				navigateInto(Entity* _centre) = 0;									/// Selects _centre's leftmost, innermost focusable child. e.g. X on ()s: (++X + 4)
	virtual void				navigateOnto(Entity* _shell) = 0;									/// Selects _shell's leftmost focusable child. e.g. ++X on ()s: (++X + 4)
	enum NavigationDirection { Forwards, Backwards };
	virtual void				navigateAway(Entity* _from, NavigationDirection _d = Forwards) = 0;	/// Selects closest focusable entity visually _d from _from. e.g. 4 on ()s: (++X + 4)
	virtual void				navigateToNew(Entity* _from) = 0;									/// Selects closest focusable sibling-owned entity visually forwards from _from, or parent if none.

	// NONVIRTUAL For viewkeys.
	void						setViewKey(Entity* _e, String const& _key, bool _v) { m_viewKeys[_e][_key] = String::number(_v); }
	ViewKeys const&				viewKeys(Entity* _e) { return m_viewKeys[_e]; }

	// NONVIRTUAL Bracketing code.
	void						setBracketed(Position const& _p) { m_bracketed.append(_p); }
	void						removeBracket(Position const& _p) { m_bracketed.removeAt(m_bracketed.lastIndexOf(_p)); }
	bool						isBracketed(Position const& _p) const { return m_bracketed.contains(_p); }
	Position					nearestBracket(Position const& _p) const;

	// NONVIRTUAL For EditDelegateFace.
	void						forgetMe(EditDelegateFace* _me) { if (m_editDelegate == _me) m_editDelegate = 0; }
	void						rememberMe(EditDelegateFace* _me) { AssertNR(!m_editDelegate); m_editDelegate = _me; }

	// Layout retrieval/cache.
	virtual String				layoutString(Entity* _e) = 0;									///< @returns the layout string for the entity _e.
	virtual StringList			layoutList(Entity* _e) = 0;
	/// Marks the given entity as requiring a repaint.
	virtual bool				markDirty(Entity* _e) = 0;

	/// @returns the status of the insertion flag.
	virtual bool				insert() const = 0;
	/// Notifies us that the current key event should be recycled (i.e. handled again).
	virtual void				reinterpretCurrentKeyEvent() = 0;

	// Hacks
	virtual void				silentlySetCurrent(Entity* _e) = 0;

	/// Set the focused item to that which represents _e.
	virtual void				setEditing(Entity* _e) = 0;
	virtual void				leaveEdit() = 0;

	virtual void				repaint(Entity* _e) = 0;
	virtual void				relayout(Entity* _e) = 0;
	/// Resets the parent's defineLayout cache.
	virtual void				resetLayoutCache(Entity* _e) = 0;
	/// For when an entity has changed in the scene.
	virtual void				relayoutLater(Entity* _e) = 0;
	void						leaving(Entity* _e);
	virtual void				leaving(Entity* _e, Position const& _grave) = 0;

	virtual void				killStrobe() = 0;
	virtual void				setStrobeCreation(Entity*) = 0;
	virtual void				setStrobeChild(Entity*) = 0;
	virtual Entity*				strobeCreation() const = 0;
	virtual Entity*				strobeChild() const = 0;

protected:
//	void						notifyOfStrobe(Entity* _child, Entity* _creation);

	// State
	Hash<Entity*, ViewKeys>		m_viewKeys;
	List<Position>				m_bracketed;

	EditDelegateFace*			m_editDelegate;

private:
	static List<CodeScene*>		s_allScenes;
};

}

