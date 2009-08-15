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

#include "Entity.h"
#include "Kind.h"
#include "Dependee.h"
#include "Familial.h"
#include "ChangeMan.h"

namespace Martta
{

class Depender: public_interface Familial
{
	MARTTA_INTERFACE
	MARTTA_INHERITS(Familial, 0)

	friend class ChangeMan;

protected:
	Depender(): m_isAsleep(false) {}

	// How we react to changes.

	/// Return false if not interested in notifications in current state.
	virtual bool						shouldBeNotified() const { return !m_isAsleep; }

	/// Called when:
	/// - Our index changes and familyDependencies includes DependsOnIndex, but the parent remains the same.
	/// @note By default, it does nothing.
	virtual void						onIndexChanged(int /*_oldIndex*/) {}
	/// Called when:
	/// - A registered or family dependency's state changes (_e is the dependency) and its onChanged() returned true.
	/// - A child changes position, and children are a family dependency (_e is the child).
	/// @note By default, it does nothing.
	virtual void						onDependencyChanged(int /*_aspect*/, Entity* _e) { onDependencyChanged(_e); }
	virtual void						onDependencyChanged(Entity* /*_e*/) {}	// TODO: Remove
	/// Called when:
	/// - A registered dependency is removed and another is immediately added (_e is the new dependency).
	/// - The object fulfilling an ancestral dependency has changed, though both are non-null (_e is the old ancestor).
	/// - A family member is replaced, and there is a family dependency (_e is the new family member).
	/// @note By default, it just called the onDependencyChanged() method.
	virtual void						onDependencySwitched(Entity* _e, Entity* /*_old*/) { onDependencyChanged(Dependee::AllAspects, _e); }
	/// Called when familyDependencies includes DependsOnChildOrder and:
	/// - A child entity has had its index changed (usually through the insertion of another child at an earlier
	/// position).
	/// @note By default, it just called the onDependencyChanged() method.
	virtual void						onChildMoved(Entity* _e, int /*_oldIndex*/) { onDependencyChanged(Dependee::AllAspects, _e); }
	/// Called when:
	/// - A new parent is set where before it was null and parent is a family dependency (_e is the new parent).
	/// - A new dependent ancestor is set where before it was null (_e is the new ancestor).
	/// - A new child is added and children are a family dependency (_e is the child).
	/// @note By default, it just called the onDependencyChanged() method.
	virtual void						onDependencyAdded(Entity* _e) { onDependencyChanged(Dependee::AllAspects, _e); }
	/// Called when:
	/// - The null parent is set where there was one before and parent is a family dependency (_e is the old parent).
	/// - A child is removed and children are a family dependency (_e is the old child).
	/// - What was a dependent ancestor is removed (_e is the old ancestor).
	/// - A registered dependency has removed itself (_e is the old dependency).
	/// @note By default, it does nothing.
	virtual void						onDependencyRemoved(Entity*, int /*_index*/) {}
	/// Called when we depend on children and:
	/// - This entity has been created with prepareChildren called on it.
	/// - This entity has usurped all of another entity's children.
	/// - This entity has had its children "validified", whereby invalid ones are removed and new ones added
	///   as necessary. This is only called if > 1 child was added during the operation.
	/// This is called instead of a number of onDependencyAdded()s, and removed confusion about the state of the object
	/// as each dependency is added.
	/// @note By default, it calls onDependencyAdded() for every child entity (whether recently added or not).
	/// @note If you intend to use this, you may find it useful to change notificationRequirements() so it doesn't
	/// include BeInModel.
	virtual void						onChildrenInitialised();

	virtual ~Depender() { if (!m_isAsleep) ChangeMan::get()->dead(this); }

	/// The following API is to define exactly what we depend on.
	/// Three way of doing this:
	/// - nuclear family dependencies, with an appropriate return value from familyDependencies(),
	/// - depending on the immediate ancestor of a given kind, with an approprate return value from ancestralDependencies(),
	/// - freeform dependency management with addDependency(), removeDependency().

	enum { DependsOnNothing = 0, DependsOnParent = 1, DependsOnChildren = 2, DependsOnBoth = 3, DependsOnIndex = 4, TestOnOrder = 8, DependsOnChildOrder = DependsOnChildren | TestOnOrder };
	virtual int							familyDependencies() const { return DependsOnNothing; }

	virtual Kinds						ancestralDependencies() const { return Kinds(); }

	/// Adds a dependency.
	/// Note this will *not* call onDependencyAdded(_e) for you. You must call it yourself if you want it to run.
	inline void							addDependency(Dependee* _e) { if (!m_isAsleep) ChangeMan::get()->addDependency(this, _e); }
	/// Removes a dependency.
	/// Note this will *not* call onDependencyRemoved(_e) for you. You must call it yourself if you want it to run.
	inline void							removeDependency(Dependee* _e) { if (!m_isAsleep) ChangeMan::get()->removeDependency(this, _e); }
	/// Removes all dependencies.
	inline void							removeAllDependencies() { if (!m_isAsleep) ChangeMan::get()->removeAllDependencies(this); }
	/// @returns true if this object already has a freeform dependency on another
	inline bool							haveDependency(Dependee* _e) const { return !m_isAsleep && ChangeMan::get()->haveDependency(this, _e); }

	virtual void						childrenInitialised() { if (!m_isAsleep) ChangeMan::get()->childrenInitialised(this); }
	virtual void						childAdded(int _newChildsIndex) { if (!m_isAsleep) ChangeMan::get()->childAdded(this, _newChildsIndex); }
	virtual void						childSwitched(Entity* _currentChild, Entity* _exChild) { if (!m_isAsleep) ChangeMan::get()->childSwitched(this, _currentChild, _exChild); }
	virtual void						childRemoved(Entity* _exChild, int _exChildsIndex) { if (!m_isAsleep) ChangeMan::get()->childRemoved(this, _exChild, _exChildsIndex); }
	virtual void						childMoved(Entity* _child, int _originalIndex) { if (!m_isAsleep) ChangeMan::get()->childMoved(this, _child, _originalIndex); }
	virtual void						parentAdded() { if (!m_isAsleep) ChangeMan::get()->parentAdded(this); }
	virtual void						parentSwitched(Entity* _exParent) { if (!m_isAsleep) ChangeMan::get()->parentSwitched(this, _exParent); }
	virtual void						parentRemoved(Entity* _exParent) { if (!m_isAsleep) ChangeMan::get()->parentRemoved(this, _exParent); }
	virtual void						dependencySwitched(Entity* _currentDependency, Entity* _exDependency) { if (!m_isAsleep) ChangeMan::get()->dependencySwitched(this, _currentDependency, _exDependency); }

	bool								m_isAsleep;
};

}
