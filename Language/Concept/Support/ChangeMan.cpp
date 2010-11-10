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

#include "Concept.h"
#include "Depender.h"
#include "Dependee.h"
#include "ChangeMan.h"

namespace Martta
{

ChangeMan* ChangeMan::s_this = 0;

ChangeListener::ChangeListener()
{
	ChangeMan::get()->m_listeners.append(this);
}

ChangeListener::~ChangeListener()
{
	ChangeMan::get()->m_listeners.removeOne(this);
}

ChangeMan::Entry::Entry(Depender* _depender, Operation _op, Concept* _o1, Concept* _o2, int _index):
	m_depender(static_cast<Concept*>(_depender)),
	m_object1(_o1),
	m_object2(_o2),
	m_index(_index),
	m_op(_op)
{}

ChangeMan::Entry::Entry(Depender* _depender, int _aspect, Concept* _changer):
	m_depender(static_cast<Concept*>(_depender)),
	m_object1(_changer),
	m_object2(0),
	m_index(_aspect),
	m_op(DependencyChanged)
{}

void ChangeMan::oneFootInTheGrave(Dependee* _going, Dependee* _replacement)
{
	for (MultiHash<Dependee*, Depender*>::Iterator i = m_dependers.find(_going); i != m_dependers.end() && i.key() == _going; i = m_dependers.erase(i))
	{
		MultiHash<Depender*, Dependee*>::Iterator j = m_dependees.find(i.value(), i.key());
		AssertNR (j != m_dependees.end());
		m_dependees.erase(j);
		if (i.value()->shouldBeNotified())
		{
			if (_replacement)
				m_changeQueue << Entry(i.value(), DependencySwitched, _replacement->concept(), _going->concept());
			else
				m_changeQueue << Entry(i.value(), DependencyRemoved, _going->concept());
		}
	}
	processQueue();
}

bool ChangeMan::changed(Dependee* _changer, int _aspect)
{
	if (m_asleep)
		return false;

	// if (already in changed() call for this object with subseteq of _aspect) return false;
	foreach (Changing e, m_changing)
		if (e.m_changer == _changer && (e.m_aspect & _aspect) == _aspect)
			return false;
	m_changing.append(Changing(_changer, _aspect));

	Concept* e = _changer->concept();

	foreach (ChangeListener* l, m_listeners)
		l->onChanged(_changer->concept(), _aspect);

	foreach (Depender* d, m_dependers.values(_changer))
		if (d->shouldBeNotified())
			m_changeQueue << Entry(d, _aspect, e);
	foreach (Depender* d, e->children())
		if (d->shouldBeNotified() && d->familyDependencies() & Depender::DependsOnParent)
			m_changeQueue << Entry(d, _aspect, e);
	if (Depender* d = e->parent())
		if (d->shouldBeNotified() && d->familyDependencies() & Depender::DependsOnChildren)
			m_changeQueue << Entry(d, _aspect, e);
	List<Concept*> es;
	es << _changer->concept();
	while (es.size())
	{
		Concept* de = es.takeLast();
		if (Depender* d = de)
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

	return true;
}

void ChangeMan::dead(Dependee* _gone)
{
	AssertNR(m_changeQueue.size() == 0);
	for (MultiHash<Dependee*, Depender*>::Iterator i = m_dependers.find(_gone); i != m_dependers.end() && i.key() == _gone; i = m_dependers.erase(i))
	{
		MultiHash<Depender*, Dependee*>::Iterator j = m_dependees.find(i.value(), i.key());
		AssertNR (j != m_dependees.end());
		m_dependees.erase(j);
	}
}

void ChangeMan::dead(Depender* _gone)
{
	AssertNR(m_changeQueue.size() == 0);
	removeAllDependencies(_gone);
}

void ChangeMan::removeAllDependencies(Depender* _gone)
{
	for (MultiHash<Depender*, Dependee*>::Iterator i = m_dependees.find(_gone); i != m_dependees.end() && i.key() == _gone; i = m_dependees.erase(i))
	{
		MultiHash<Dependee*, Depender*>::Iterator j = m_dependers.find(i.value(), i.key());
		AssertNR (j != m_dependers.end());
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
	case ConceptChildrenInitialised:
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
	bool pc = m_processingQueue;
	m_processingQueue = true;

	while (m_changeQueue.size())
	{
		Entry e = m_changeQueue.takeFirst();
		m_changesDone << e;
		processEntry(e);
	}

	m_processingQueue = pc;
	if (!m_processingQueue)
		m_changing.clear();
}

void ChangeMan::childrenInitialised(Depender* _this)
{
	if (m_asleep)
		return;
	if (_this->shouldBeNotified())
	{
		foreach (ChangeListener* l, m_listeners)
			l->onChildrenInitialised(_this->concept());
		if (_this->familyDependencies() & Depender::DependsOnChildren)
			m_changeQueue << Entry(_this, ConceptChildrenInitialised);
	}
	processQueue();
	m_hasChanged = true;
}

void ChangeMan::childAdded(Depender* _this, int _index)
{
	AssertNR(_this->concept()->child(_index));
	if (m_asleep)
		return;
	if (_this->shouldBeNotified())
	{
		foreach (ChangeListener* l, m_listeners)
			l->onChildAdded(_this->concept(), _index);
		if (_this->familyDependencies() & Depender::DependsOnChildren)
		{
			m_changeQueue << Entry(_this, DependencyAdded, _this->concept()->child(_index));
			if (_index >= 0)
				for (int i = _index + 1; i < _this->concept()->cardinalChildCount(); i++)
					childMoved(_this, _this->concept()->child(i), i - 1);
		}
	}
	processQueue();
	m_hasChanged = true;
}

void ChangeMan::childSwitched(Depender* _this, Concept* _ch, Concept* _old)
{
	AssertNR(_ch);
	AssertNR(_ch->parent() == _this->concept());
	AssertNR(_old);
	AssertNR(_old->parent() != _this->concept());
	if (m_asleep)
		return;
	if (_this->shouldBeNotified())
	{
		foreach (ChangeListener* l, m_listeners)
			l->onChildSwitched(_this->concept(), _ch, _old);
		if (_this->familyDependencies() & Depender::DependsOnChildren)
			m_changeQueue << Entry(_this, DependencySwitched, _ch, _old);
	}
	processQueue();
}

void ChangeMan::dependencySwitched(Depender* _this, Concept* _currentDependency, Concept* _exDependency)
{
	Dependee* dee = _currentDependency;
	(void)(dee);
	AssertNR(dee);
	AssertNR(_exDependency);
	AssertNR(m_dependees.contains(_this, dee));
	if (m_asleep)
		return;
	if (_this->shouldBeNotified())
	{
		foreach (ChangeListener* l, m_listeners)
			l->onDependencySwitched(_this->concept(), _currentDependency, _exDependency);
		m_changeQueue << Entry(_this, DependencySwitched, _currentDependency, _exDependency);
	}
	processQueue();
}

void ChangeMan::childRemoved(Depender* _this, Concept* _old, int _index)
{
	AssertNR(_old);
	AssertNR(_old->parent() != _this->concept());
	if (m_asleep)
		return;
	if (_this->shouldBeNotified())
	{
		foreach (ChangeListener* l, m_listeners)
			l->onChildRemoved(_this->concept(), _old, _index);
		if (_this->familyDependencies() & Depender::DependsOnChildren)
		{
			m_changeQueue << Entry(_this, DependencyRemoved, _old, 0, _index);
			if (_index >= 0)
				for (int i = _index; i < _this->concept()->cardinalChildCount(); i++)
					childMoved(_this, _this->concept()->child(i), i + 1);
		}
	}
	processQueue();
	m_hasChanged = true;
}

void ChangeMan::childMoved(Depender* _this, Concept* _ch, int _oI)
{
	AssertNR(_ch);
	if (m_asleep)
		return;
	if (_this->shouldBeNotified())
	{
		foreach (ChangeListener* l, m_listeners)
			l->onChildMoved(_this->concept(), _ch, _oI);
		if (_this->familyDependencies() & Depender::TestOnOrder)
			m_changeQueue << Entry(_this, ChildMoved, _ch, 0, _oI);
	}
	if (Depender* d = _ch)
		if (d->shouldBeNotified() && d->familyDependencies() & Depender::DependsOnIndex)
			m_changeQueue << Entry(d, IndexChanged, 0, 0, _oI);
	processQueue();
	m_hasChanged = true;
}

void ChangeMan::parentAdded(Depender* _this)
{
	Concept* p = _this->concept()->parent();
	AssertNR(p);
	if (m_asleep)
		return;
	if (_this->shouldBeNotified())
	{
		foreach (ChangeListener* l, m_listeners)
			l->onParentAdded(_this->concept());
		if (_this->familyDependencies() & Depender::DependsOnParent)
			m_changeQueue << Entry(_this, DependencyAdded, p);
	}

	List<Concept*> es;
	es << _this->concept();
	while (es.size())
	{
		Concept* e = es.takeLast();
		if (Depender* d = e)
			foreach (Kind k, d->ancestralDependencies())
				if (Concept* a = e->ancestor(k))
					if (p->hasSelfAncestor(a))
					{
						foreach (ChangeListener* l, m_listeners)
							l->onAncestorAdded(e, a);
						m_changeQueue << Entry(d, DependencyAdded, a);
					}
		es << e->children();
	}
	processQueue();
}

void ChangeMan::parentSwitched(Depender* _this, Concept* _old)
{
	Concept* p = _this->concept()->parent();
	AssertNR(_old);
	AssertNR(p);
	AssertNR(p != _old);
	if (m_asleep)
		return;
	if (_this->shouldBeNotified())
	{
		foreach (ChangeListener* l, m_listeners)
			l->onParentSwitched(_this->concept(), _old);
		if (_this->familyDependencies() & Depender::DependsOnParent)
			m_changeQueue << Entry(_this, DependencySwitched, p, _old);
	}

	List<Concept*> es;
	es << _this->concept();
	while (es.size())
	{
		Concept* e = es.takeLast();
		if (Depender* d = e)
			foreach (Kind k, d->ancestralDependencies())
			{
				Concept* a = e->ancestor(k);
				if (!a->hasAncestor(p))	// If the "new" ancestor itself has an ancestor that is the new parent, then it cannot have changed. Otherwise...
				{
					Concept* oldAnc = _old->selfAncestor(k);
					if (a && !oldAnc)											// Had no old ancestor but do have a new one. Added...
					{
						foreach (ChangeListener* l, m_listeners)
							l->onAncestorAdded(e, a);
						m_changeQueue << Entry(d, DependencyAdded, a);
					}
					else if (oldAnc && !a)										// Had old ancestor but no new one. Removed...
					{
						foreach (ChangeListener* l, m_listeners)
							l->onAncestorRemoved(e, oldAnc);
						m_changeQueue << Entry(d, DependencyRemoved, oldAnc);
					}
					else if (oldAnc != a)										// Have both, but they're different. Changed...
					{
						foreach (ChangeListener* l, m_listeners)
							l->onAncestorSwitched(e, a, oldAnc);
						m_changeQueue << Entry(d, DependencySwitched, a, oldAnc);
					}
				}
			}
		es << e->children();
	}
	processQueue();
}

void ChangeMan::parentRemoved(Depender* _this, Concept* _old)
{
	AssertNR(_old);
	AssertNR(!_this->concept()->parent());
	if (m_asleep)
		return;
	if (_this->shouldBeNotified())
	{
		foreach (ChangeListener* l, m_listeners)
			l->onParentRemoved(_this->concept(), _old);
		if (_this->familyDependencies() & Depender::DependsOnParent)
			m_changeQueue << Entry(_this, DependencyRemoved, _old);
	}

	List<Concept*> es;
	es << _this->concept();
	while (es.size())
	{
		Concept* e = es.takeLast();
		if (Depender* d = e)
			foreach (Kind k, d->ancestralDependencies())
				if (!e->hasAncestor(k))
					if (Concept* oldAnc = _old->selfAncestor(k))
					{
						foreach (ChangeListener* l, m_listeners)
							l->onAncestorRemoved(e, oldAnc);
						m_changeQueue << Entry(d, DependencyRemoved, oldAnc);
					}
		es << e->children();
	}
	processQueue();
}

}
