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
	enum Operation
	{
		DependencyChanged,
		EntityChildrenInitialised,
		DependencyAdded,
		DependencyRemoved,
		DependencySwitched,
		ChildMoved,
		IndexChanged
	};
	
	// TODO Maybe change _exo & _o to Dependees. Only do this if we filter added/removed/switched stuff at Depender level to involve only Dependees.
	// If this were all done it would allow non-immediate change processing (which may or may not be a good thing), by cleaning up the queue through 
	// Dependee/Depender dead notifications.
	struct Entry
	{
		Entry(Depender* _depender, Operation _op, Entity* _o1 = 0, Entity* _o2 = 0, int _index = UndefinedIndex): m_depender(_depender), m_object1(_o1), m_object2(_o2), m_index(_index), m_op(_op) {}
		Entry(Depender* _depender, int _aspect, Entity* _changer): m_depender(_depender), m_object1(_changer), m_object2(0), m_index(_aspect), m_op(DependencyChanged) {}
		Depender* m_depender;
		Entity* m_object1;
		Entity* m_object2;
		union
		{
			int m_index;
			int m_aspect;
		};
		Operation m_op;
	};
	
	struct Changing
	{
		Changing(Dependee* _changer, int _aspect): m_changer(_changer), m_aspect(_aspect) {}
		Dependee* m_changer;
		int m_aspect;
	};

	static ChangeMan*					get() { return s_this ? s_this : (s_this = new ChangeMan); }
	
	/// returns false if already inside an equivalent changed() call.
	bool								changed(Dependee* _changer, int _aspect);
	void								oneFootInTheGrave(Dependee* _going);
	
	void								dead(Dependee* _gone);
	void								dead(Depender* _gone);
	
	virtual void						childrenInitialised(Depender* _this);
	virtual void						childAdded(Depender* _this, int _newChildsIndex);
	virtual void						childSwitched(Depender* _this, Entity* _currentChild, Entity* _exChild);
	virtual void						childRemoved(Depender* _this, Entity* _exChild, int _deadChildsIndex);
	virtual void						childMoved(Depender* _this, Entity* _child, int _oldIndex);
	virtual void						parentAdded(Depender* _this);
	virtual void						parentSwitched(Depender* _this, Entity* _exParent);
	virtual void						parentRemoved(Depender* _this, Entity* _exParent);
	virtual void						dependencySwitched(Depender* _this, Entity* _currentDependency, Entity* _exDependency);

	/// Adds a dependency.
	/// Note this will *not* call onDependencyAdded(_e) for you. You must call it yourself if you want it to run.
	void								addDependency(Depender* _der, Dependee* _dee) { m_dependees.insert(_der, _dee); m_dependers.insert(_dee, _der); }
	/// Removes a dependency.
	/// Note this will *not* call onDependencyRemoved(_e) for you. You must call it yourself if you want it to run.
	void								removeDependency(Depender* _der, Dependee* _dee) { m_dependees.erase(m_dependees.find(_der, _dee)); m_dependers.erase(m_dependers.find(_dee, _der)); }
	/// Removes all dependencies.
	void								removeAllDependencies(Depender* _der);
	bool								haveDependency(Depender const* _der, Dependee const* _dee) const { return m_dependees.constFind(const_cast<Depender*>(_der), const_cast<Dependee*>(_dee)) != m_dependees.end(); }

private:
	ChangeMan() {}
	virtual ~ChangeMan() {}

	void								processQueue();
	void								processEntry(Entry const& _e);

	// These two are symmetrical and used for storing the freeform dependencies.
	QMultiHash<Dependee*, Depender*>	m_dependers;
	QMultiHash<Depender*, Dependee*>	m_dependees;
	
	QList<Changing>						m_changing;
	
	QList<Entry>						m_changeQueue;
	QList<Entry>						m_changesDone;
	
	static ChangeMan*					s_this;
};

}
