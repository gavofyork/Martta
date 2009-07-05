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

#include "Depender.h"
#include "Dependee.h"
#include "ChangeMan.h"

namespace Martta
{

ChangeMan* ChangeMan::s_this = 0;

void ChangeMan::oneFootInTheGrave(Dependee* _going)
{
	for (MultiHash<Dependee*, Depender*>::Iterator i = m_dependers.find(_going); i != m_dependers.end() && i.key() == _going; i = m_dependers.erase(i))
	{
		MultiHash<Depender*, Dependee*>::Iterator j = m_dependees.find(i.value(), i.key());
		M_ASSERT (j != m_dependees.end());
		m_dependees.erase(j);
		if (i.value()->botherNotifying())
			m_changeQueue << Entry(i.value(), DependencyRemoved, _going->self());
	}
	processQueue();
}

bool ChangeMan::changed(Dependee* _changer, int _aspect)
{
	if (m_asleep)
		return false;

	// if (already in changed() call for this object with subseteq of _aspect) return false;
	foreach (Changing e, m_changing)
		if (e.m_changer == _changer && e.m_aspect & _aspect == _aspect)
			return false;
	m_changing.append(Changing(_changer, _aspect));
	
	Entity* e = _changer->self();
	foreach (Depender* d, m_dependers.values(_changer))
		if (d->botherNotifying())
			m_changeQueue << Entry(d, _aspect, e);
	foreach (Depender* d, e->childrenOf<Depender>())
		if (d->botherNotifying() && d->familyDependencies() & Depender::DependsOnParent)
			m_changeQueue << Entry(d, _aspect, e);
	if (Depender* d = e->tryParent<Depender>())
		if (d->botherNotifying() && d->familyDependencies() & Depender::DependsOnChildren)
			m_changeQueue << Entry(d, _aspect, e);
	List<Entity*> es;
	es << _changer->self();
	while (es.size())
	{
		Entity* de = es.takeLast();
		if (Depender* d = de->tryKind<Depender>())
			foreach (Kind k, d->ancestralDependencies())
				if (e == de->ancestor(k))
				{
					m_changeQueue << Entry(d, _aspect, e);
					break;
				}
		es << de->children();
	}
	
	processQueue();
	m_hasChanged = true;
	
	m_changing.removeLast();
	return true;
}

void ChangeMan::dead(Dependee* _gone)
{
	M_ASSERT(m_changeQueue.size() == 0);
	for (MultiHash<Dependee*, Depender*>::Iterator i = m_dependers.find(_gone); i != m_dependers.end() && i.key() == _gone; i = m_dependers.erase(i))
	{
		MultiHash<Depender*, Dependee*>::Iterator j = m_dependees.find(i.value(), i.key());
		M_ASSERT (j != m_dependees.end());
		m_dependees.erase(j);
	}
}

void ChangeMan::dead(Depender* _gone)
{
	M_ASSERT(m_changeQueue.size() == 0);
	removeAllDependencies(_gone); 
}

void ChangeMan::removeAllDependencies(Depender* _gone)
{
	for (MultiHash<Depender*, Dependee*>::Iterator i = m_dependees.find(_gone); i != m_dependees.end() && i.key() == _gone; i = m_dependees.erase(i))
	{
		MultiHash<Dependee*, Depender*>::Iterator j = m_dependers.find(i.value(), i.key());
		M_ASSERT (j != m_dependers.end());
		m_dependers.erase(j);
	}
}

void ChangeMan::processEntry(Entry const& _e)
{
	switch (_e.m_op)
	{
	case DependencyChanged:
		_e.m_depender->onDependencyChanged(_e.m_aspect, _e.m_object1);
		break;
	case EntityChildrenInitialised:
		_e.m_depender->onChildrenInitialised();
		break;
	case DependencyAdded:
		_e.m_depender->onDependencyAdded(_e.m_object1);
		break;
	case DependencyRemoved:
		_e.m_depender->onDependencyRemoved(_e.m_object1, _e.m_index);
		break;
	case DependencySwitched:
		_e.m_depender->onDependencySwitched(_e.m_object1, _e.m_object2);
		break;
	case ChildMoved:
		_e.m_depender->onChildMoved(_e.m_object1, _e.m_index);
		break;
	case IndexChanged:
		_e.m_depender->onIndexChanged(_e.m_index);
		break;
	}
}

void ChangeMan::processQueue()
{
	while (m_changeQueue.size())
	{
		Entry e = m_changeQueue.takeFirst();
		processEntry(e);
		m_changesDone << e;
	}
}

void ChangeMan::childrenInitialised(Depender* _this)
{
	if (m_asleep)
		return;
	if (_this->botherNotifying() && _this->familyDependencies() & Depender::DependsOnChildren)
		m_changeQueue << Entry(_this, EntityChildrenInitialised);
	processQueue();
	m_hasChanged = true;
}

void ChangeMan::childAdded(Depender* _this, int _index)
{
	M_ASSERT(_this->self()->child(_index));
	if (m_asleep)
		return;
	if (_this->botherNotifying() && _this->familyDependencies() & Depender::DependsOnChildren)
	{
		m_changeQueue << Entry(_this, DependencyAdded, _this->self()->child(_index));
		if (_index >= 0)
			for (int i = _index + 1; i < _this->self()->cardinalChildCount(); i++)
				childMoved(_this, _this->self()->child(i), i - 1);
	}
	processQueue();
	m_hasChanged = true;
}

void ChangeMan::childSwitched(Depender* _this, Entity* _ch, Entity* _old)
{
	M_ASSERT(_ch);
	M_ASSERT(_ch->parent() == _this->self());
	M_ASSERT(_old);
	M_ASSERT(_old->parent() != _this->self());
	if (m_asleep)
		return;
	if (_this->botherNotifying() && _this->familyDependencies() & Depender::DependsOnChildren)
		m_changeQueue << Entry(_this, DependencySwitched, _ch, _old);
	processQueue();
}

void ChangeMan::dependencySwitched(Depender* _this, Entity* _currentDependency, Entity* _exDependency)
{
	Dependee* dee = _currentDependency->asKind<Dependee>();
	M_ASSERT(dee);
	M_ASSERT(_exDependency);
	M_ASSERT(m_dependees.contains(_this, dee));
	if (m_asleep)
		return;
	if (_this->botherNotifying())
		m_changeQueue << Entry(_this, DependencySwitched, _currentDependency, _exDependency);
	processQueue();
}

void ChangeMan::childRemoved(Depender* _this, Entity* _old, int _index)
{
	M_ASSERT(_old);
	M_ASSERT(_old->parent() != _this->self());
	if (m_asleep)
		return;
	if (_this->botherNotifying() && _this->familyDependencies() & Depender::DependsOnChildren)
	{
		m_changeQueue << Entry(_this, DependencyRemoved, _old, 0, _index);
		if (_index >= 0)
			for (int i = _index; i < _this->self()->cardinalChildCount(); i++)
				childMoved(_this, _this->self()->child(i), i + 1);
	}
	processQueue();
	m_hasChanged = true;
}

void ChangeMan::childMoved(Depender* _this, Entity* _ch, int _oI)
{
	M_ASSERT(_ch);
	if (m_asleep)
		return;
	if (_this->botherNotifying() && _this->familyDependencies() & Depender::TestOnOrder)
		m_changeQueue << Entry(_this, ChildMoved, _ch, 0, _oI);
	if (Depender* d = _ch->tryKind<Depender>())
		if (d->botherNotifying() && d->familyDependencies() & Depender::DependsOnIndex)
			m_changeQueue << Entry(d, IndexChanged, 0, 0, _oI);
	processQueue();
	m_hasChanged = true;
}

void ChangeMan::parentAdded(Depender* _this)
{
	Entity* p = _this->self()->parent();
	M_ASSERT(p);
	if (m_asleep)
		return;
	if (_this->botherNotifying() && _this->familyDependencies() & Depender::DependsOnParent)
		m_changeQueue << Entry(_this, DependencyAdded, p);

	List<Entity*> es;
	es << _this->self();
	while (es.size())
	{
		Entity* e = es.takeLast();
		if (Depender* d = e->tryKind<Depender>())
			foreach (Kind k, d->ancestralDependencies())
				if (Entity* a = e->ancestor(k))
					if (p->hasSelfAncestor(a))
						m_changeQueue << Entry(d, DependencyAdded, a);
		es << e->children();
	}
	processQueue();
}

void ChangeMan::parentSwitched(Depender* _this, Entity* _old)
{
	Entity* p = _this->self()->parent();
	M_ASSERT(_old);
	M_ASSERT(p);
	M_ASSERT(p != _old);
	if (m_asleep)
		return;
	if (_this->botherNotifying() && _this->familyDependencies() & Depender::DependsOnParent)
		m_changeQueue << Entry(_this, DependencySwitched, p, _old);
		
	List<Entity*> es;
	es << _this->self();
	while (es.size())
	{
		Entity* e = es.takeLast();
		if (Depender* d = e->tryKind<Depender>())
			foreach (Kind k, d->ancestralDependencies())
			{
				Entity* a = e->ancestor(k);
				if (!a->hasAncestor(p))	// If the "new" ancestor itself has an ancestor that is the new parent, then it cannot have changed. Otherwise...
				{
					Entity* oldAnc = _old->selfAncestor(k);
					if (a && !oldAnc)											// Had no old ancestor but do have a new one. Added...
						m_changeQueue << Entry(d, DependencyAdded, a);
					else if (oldAnc && !a)										// Had old ancestor but no new one. Removed...
						m_changeQueue << Entry(d, DependencyRemoved, oldAnc);
					else if (oldAnc != a)										// Have both, but they're different. Changed...
						m_changeQueue << Entry(d, DependencySwitched, a, oldAnc);
				}
			}
		es << e->children();
	}
	processQueue();
}

void ChangeMan::parentRemoved(Depender* _this, Entity* _old)
{
	M_ASSERT(_old);
	M_ASSERT(!_this->self()->parent());
	if (m_asleep)
		return;
	if (_this->botherNotifying() && _this->familyDependencies() & Depender::DependsOnParent)
		m_changeQueue << Entry(_this, DependencyRemoved, _old);
		
	List<Entity*> es;
	es << _this->self();
	while (es.size())
	{
		Entity* e = es.takeLast();
		if (Depender* d = e->tryKind<Depender>())
			foreach (Kind k, d->ancestralDependencies())
				if (!e->hasAncestor(k))
					if (Entity* a = _old->selfAncestor(k))
						m_changeQueue << Entry(d, DependencyRemoved, a);
		es << e->children();
	}
	processQueue();
}

}
