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

#include "Meta.h"

namespace Martta
{

class Depender;
class Dependee;

class ChangeMan
{
public:
	static ChangeMan*					get() { return s_this ? s_this : (s_this = new ChangeMan); }
	
	void								changed(Dependee* _changer, int _aspect) { (void)(_changer); (void)(_aspect); }
	
	void								dead(Dependee* _gone) { (void)(_gone); }
	void								dead(Depender* _gone) { (void)(_gone); }
	
/*	void								childrenInitialised(Dependee* _this);
	void								childAdded(Dependee* _this, int _newChildsIndex);
	void								childSwitched(Dependee* _this, Entity* _currentChild, Entity* _exChild);
	void								childRemoved(Dependee* _this, Entity* _exChild, int _deadChildsIndex);
	void								childMoved(Dependee* _this, Entity* _child, int _oldIndex);
	void								parentAdded(Dependee* _this);
	void								parentSwitched(Dependee* _this, Entity* _exParent);
	void								parentRemoved(Dependee* _this, Entity* _exParent);

	void								dependencyAdded(Depender* _this, Entity* _e) { change(_this, DependencyAdded, _e); _this->onDependencyAdded(_e); }
	void								dependencyRemoved(Depender* _this, Entity* _e, int _index = UndefinedIndex) { change(_this, DependencyRemoved, _e); _this->onDependencyRemoved(_e, _index); }
	void								dependencyChanged(Depender* _this, Entity* _e) { change(_this, DependencyChanged, _e); _this->onDependencyChanged(_e); }
	void								dependencySwitched(Depender* _this, Entity* _e, Entity* _o) { change(_this, DependencySwitched, _e); _this->onDependencySwitched(_e, _o); }
	void								notifyOfChildMove(Depender* _this, Entity* _e, int _oI) { change(_this, ChildMoved, _e); _this->onChildMoved(_e, _oI); }
	void								indexChanged(Depender* _this, int _oI) { change(_this, ContextIndexChanged, 0); _this->onIndexChanged(_oI); }

	/// Adds a dependency.
	/// Note this will *not* call onDependencyAdded(_e) for you. You must call it yourself if you want it to run.
	void								addDependency(Entity* _e);
	/// Removes a dependency.
	/// Note this will *not* call onDependencyRemoved(_e) for you. You must call it yourself if you want it to run.
	void								removeDependency(Entity* _e);
	/// Removes all dependencies.
	void								removeAllDependencies();
	bool								haveDependency(Entity* _e) const { return m_dependencies.contains(_e); }

	/// Rejigs our ancestral dependencies. This should be (TODO: and isn't yet) called whenever any of our ancestors have changed parent
	/// or been switched, or when the ouput of ancestralDependencies() changes.
	void								updateAncestralDependencies();
*/
	virtual ~ChangeMan() {}

private:
	// These two are symmetrical.
	QHash<Dependee*, Depender*>			m_dependers;
	QHash<Depender*, Dependee*>			m_dependees;
	
	static ChangeMan*					s_this;
};

}
