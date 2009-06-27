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

#include "Kind.h"
#include "Familial.h"
#include "ChangeMan.h"

namespace Martta
{

class Depender: public_interface Familial
{
	MARTTA_INTERFACE
	MARTTA_INHERITS(Familial, 0)

protected:
	virtual void						childrenInitialised() {}
	virtual void						childAdded(int _newChildsIndex) { (void)(_newChildsIndex); }
	virtual void						childSwitched(Entity* _currentChild, Entity* _exChild) { (void)(_currentChild); (void)(_exChild); }
	virtual void						childRemoved(Entity* _exChild, int _exChildsIndex) { (void)(_exChild); (void)(_exChildsIndex); }
	virtual void						childMoved(Entity* _child, int _originalIndex) { (void)(_child); (void)(_originalIndex); }
	virtual void						parentAdded() {}
	virtual void						parentSwitched(Entity* _exParent) { (void)(_exParent); }
	virtual void						parentRemoved(Entity* _exParent) { (void)(_exParent); }

	enum { DependsOnNothing = 0, DependsOnParent = 1, DependsOnChildren = 2, DependsOnBoth = 3, DependsOnIndex = 4, TestOnOrder = 8, DependsOnChildOrder = DependsOnChildren | TestOnOrder };
	virtual int							familyDependencies() const { return DependsOnNothing; }
	virtual Kinds						ancestralDependencies() const { return Kinds(); }
	/// Called when:
	/// - Our index changes and familyDependencies includes DependsOnIndex, but the parent remains the same.
	/// @note By default, it does nothing.
	virtual void						onIndexChanged(int /*_oldIndex*/) {}
	/// Called when:
	/// - A registered or family dependency's state changes (_e is the dependency) and its onChanged() returned true.
	/// - A child changes position, and children are a family dependency (_e is the child).
	/// @note By default, it does nothing. 
	virtual void						onDependencyChanged(Entity* /*_e*/) {}
	/// Called when:
	/// - A registered dependency is removed and another is immediately added (_e is the new dependency).
	/// - The object fulfilling an ancestral dependency has changed, though both are non-null (_e is the old ancestor).
	/// - A family member is replaced, and there is a family dependency (_e is the new family member).
	/// @note By default, it just called the onDependencyChanged() method. 
	virtual void						onDependencySwitched(Entity* _e, Entity* /*_old*/) { onDependencyChanged(_e); }
	/// Called when familyDependencies includes DependsOnChildOrder and:
	/// - A child entity has had its index changed (usually through the insertion of another child at an earlier
	/// position).
	/// @note By default, it just called the onDependencyChanged() method. 
	virtual void						onChildMoved(Entity* _e, int /*_oldIndex*/) { onDependencyChanged(_e); }
	/// Called when:
	/// - A new parent is set where before it was null and parent is a family dependency (_e is the new parent).
	/// - A new dependent ancestor is set where before it was null (_e is the new ancestor).
	/// - A new child is added and children are a family dependency (_e is the child).
	/// @note By default, it just called the onDependencyChanged() method. 
	virtual void						onDependencyAdded(Entity* _e) { onDependencyChanged(_e); }
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

	virtual ~Depender() { ChangeMan::get()->dead(this); }
};

}
