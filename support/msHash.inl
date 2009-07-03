/**
 * @filename msList.h
 * @author Gavin Wood
 * @date 21/06/2008
 * @copyright quid pro code, Ltd.
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MarttaSupport License version 1.0
 *
 * The contents of this file are subject to the MarttaSupport License
 * version 1.0 (the "License"); you may not use this file except in 
 * compliance with the License. You should have received a copy of the 
 * MarttaSupport License "COPYING.MarttaSupport" along with Martta; if not
 * you may obtain a copy of the License at http://quidprocode.co.uk/Martta/
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

namespace MarttaSupport
{

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Box::Box(GeneralHash const& _h):
	m_nodes		(_h.m_nodes),
	m_capacity	(_h.m_capacity),
	m_count		(_h.m_count)
{
	const_cast<GeneralHash&>(_h).allocate(Min);
	const_cast<GeneralHash&>(_h).m_count = 0;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Box::~Box()
{
	if (m_nodes)
	{
		for (t::uint i = 0; i < m_capacity; i++)
			if (m_nodes[i])
				for (Node* j = m_nodes[i].m_next, *nj = m_nodes[i].m_next ? m_nodes[i].m_next->m_next : 0; j != m_nodes + i; j = nj, nj = j->m_next)
					delete j;
		delete [] (m_nodes - 1);
	}
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::~GeneralHash()
{
	for (t::uint i = 0; i < m_capacity; i++)
		if (m_nodes[i])
			for (Node* j = m_nodes[i].m_next, *nj = m_nodes[i].m_next ? m_nodes[i].m_next->m_next : 0; j != m_nodes + i; j = nj, nj = j->m_next)
				delete j;
	deallocate(m_nodes);
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator&
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator::operator++()
{
	// jump to next if we're at the end of the current master node...
	if (m_node->isSentinel())
		return *this;
	if (m_node->m_next == m_masterNode)
		do
			m_node = ++m_masterNode;
		while (!m_node->isSentinel() && !m_masterNode->isActive());
	else
		m_node = m_node->m_next;
	return *this;
}
		
template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator&
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator::operator--()
{
	ASSERT(!m_node->isSentinel());
	
	if (m_node == m_masterNode)
	{
		do
			--m_masterNode;
		while (!m_masterNode->isSentinel() && !m_masterNode->isActive());
		if (m_masterNode->isSentinel())
			// Give up now; we've reached the sentinel.
			m_masterNode = m_node;
		else
		{
			// Found an active master. Skip to the last item...
			m_node = m_masterNode;
			while (m_node->m_next != m_masterNode)
				m_node = m_node->m_next;
		}
	}
	else
	{
		// There are items before us. Find the one whose next is us...
		Node *o = m_node;
		for (m_node = m_masterNode; m_node->m_next != o; m_node = m_node->m_next) ;
	}
	return *this;
}
		
template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::ConstIterator&
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::ConstIterator::operator++()
{
	// jump to next if we're at the end of the current master node...
	if (m_node->isSentinel())
		return *this;
	if (m_node->m_next == m_masterNode)
		do
			m_node = ++m_masterNode;
		while (!m_node->isSentinel() && !m_masterNode->isActive());
	else
		m_node = m_node->m_next;
	return *this;
}
		
template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::ConstIterator&
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::ConstIterator::operator--()
{
	ASSERT(!m_node->isSentinel());
	
	if (m_node == m_masterNode)
	{
		do
			--m_masterNode;
		while (!m_masterNode->isSentinel() && !m_masterNode->isActive());
		if (m_masterNode->isSentinel())
			// Give up now; we've reached the sentinel.
			m_masterNode = m_node;
		else
		{
			// Found an active master. Skip to the last item...
			m_node = m_masterNode;
			while (m_node->m_next != m_masterNode)
				m_node = m_node->m_next;
		}
	}
	else
	{
		// There are items before us. Find the one whose next is us...
		Node const* o = m_node;
		for (m_node = m_masterNode; m_node->m_next != o; m_node = m_node->m_next);
	}
	return *this;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
T const GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::value(const Key& _key) const
{
	Node* n = findNode(_key);
	if (!n)
		return T();
	return n->value();
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
T const GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::value(const Key& _key, T const& _defaultValue) const
{
	Node* n = findNode(_key);
	if (!n)
		return _defaultValue;
	return n->value();
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::ConstIterator
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::constFind(Key const& _key) const
{
	Node* m = m_nodes + indexOf(_key);
	Node* n = findNode(m, _key);
	return n ? ConstIterator(n, m) : constEnd();
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::find(Key const& _key)
{
	Node* m = m_nodes + indexOf(_key);
	Node* n = findNode(m, _key);
	return n ? Iterator(n, m) : end();
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
template<t::uint Min2, bool AlwaysMulti2>
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>&
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::moveFromBox(typename GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey>::Box const& _b)
{
	if (m_nodes)
		deallocate(m_nodes);
	m_nodes = _b.m_nodes;
	m_capacity = _b.m_capacity;
	m_count = _b.m_count;
	typename GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey>::Box& b = const_cast<typename GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey>::Box&>(_b);
	b.m_nodes = 0;
	b.m_capacity = 0;
	b.m_count = 0;
	if (!m_nodes)
	{
		allocate(Min);
		m_count = 0;
	}	
	return *this;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Node*
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::findNode(Node* _m, Key const& _key) const
{
	if (!_m->isActive())
		return 0;
	Node* i = _m;
	do
		if (i->key() == _key)
			return i;
	while ((i = i->m_next) != _m);
	return 0;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
void GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::allocate(t::uint _s)
{
	ASSERT(_s >= Min);
	m_nodes = new Node[_s + 2] + 1;
	m_capacity = _s;
	m_nodes[-1].setSentinel();
	m_nodes[_s].setSentinel();
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
void GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::deallocate(Node*& _n)
{
	delete [] (_n - 1);
	_n = 0;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
void GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::checkSize()
{
	if (m_autoGrow && m_count > m_capacity / 2)
		resizeToPowerOfTwo(m_capacity * 2);
	else if (m_autoShrink && m_count < m_capacity / 8 && m_capacity > Min)
		resizeToPowerOfTwo(m_capacity / 2);
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Node*
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::freshNode(t::uint _index)
{ 	
	ASSERT(_index < m_capacity);
	return freshNode(m_nodes + _index);
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Node*
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::freshNode(Node* _m)
{ 	
	if (_m->isActive())
		return insertNode(_m);
	_m->activate();
	return _m;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Node*
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::staleNode(t::uint _index)
{ 	
	ASSERT(_index < m_capacity);
	return freshNode(m_nodes + _index);
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Node*
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::staleNode(Node* _m)
{ 	
	if (_m->isActive())
		return appendNode(_m);
	_m->activate();
	return _m;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Node*
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::appendNode(Node* _after)
{
	Node* nn = new Node;
	nn->m_next = _after->m_next;
	_after->m_next = nn;
	return nn;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Node*
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::insertNode(Node* _before)
{
	Node* nn = new Node;
	nn->m_next = _before->m_next;
	_before->m_next = nn;
	nn->swap(*_before);
	return _before;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
int GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::count(Key const& _key) const
{
	Node* m = m_nodes + indexOf(_key);
	if (!m->isActive())
		return 0;
	int ret = 0;
	Node* i = m;
	do
		if (i->key() == _key)
			ret++;
		else if (ret)
			return ret;
	while ((i = i->m_next) != m);
	return ret;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
int GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::remove(Key const& _key)
{
	checkSize();
	Node* m = m_nodes + indexOf(_key);
	if (!m->isActive())
		return 0;
	Node* p = 0;
	Node* i = m;
	do
	{
		if (i->key() == _key)
			goto OK;
		p = i;
	}
	while ((i = i->m_next) != m);
	return 0;
	OK:
	int ret = 1;
	Node* x = i->m_next;
	if (p)
		delete i;
	while ((i = x) != m && i->key() == _key)
	{
		x = i->m_next;
		delete i;
		ret++;
	}
	// either keys are different or i == m.
	// l is one before the sequence begins or zero if it begins on m.
	
	if (p)				// some before us.
		p->m_next = i;
	else if (i != m)	// none before us and some after us.
	{
		// substitute in p for m.
		m->swap(*i);
		m->m_next = i->m_next;
		delete i;
	}
	else				// none before us and none after us.
		m->deactivate();
	m_count -= ret;
	return ret;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
int GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::removeOne(Key const& _key)
{
	checkSize();
	Node* m = m_nodes + indexOf(_key);
	if (!m->isActive())
		return 0;
	Node* p = 0;
	Node* i = m;
	do
	{
		if (i->key() == _key)
			goto OK;
		p = i;
	}
	while ((i = i->m_next) != m);
	return 0;
	OK:
	Node* x = i->m_next;
	if (p)
		delete i;
	i = x;
	// l is one before the dead node or zero if it is m.
	
	if (p)				// some before us.
		p->m_next = i;
	else if (i != m)	// none before us and some after us.
	{
		// substitute in p for m.
		m->swap(*i);
		m->m_next = i->m_next;
		delete i;
	}
	else				// none before us and none after us.
		m->deactivate();
	m_count--;
	return 1;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::erase(Iterator _pos)
{

	Node* m = _pos.m_masterNode;
	Node* p = 0;
	Node* i = m;
#ifdef RELEASE
	while ((i = i->m_next) != _pos.m_node)
		p = i;
#else
	if (!m->isActive())
	{
		ASSERT(false);
		/// Bad Iterator; masterNode is inactive.
		return _pos;
	}
	do
	{
		if (i == _pos.m_node)
			goto OK;
		p = i;
	}
	while ((i = i->m_next) != m);
	/// Bad Iterator; masterNode doesn't contain node.
	ASSERT(false);
	return _pos;
	OK:
#endif
	Node* x = i->m_next;
	if (p)
		delete i;
	i = x;
	// l is one before the dead node or zero if it is m.
	
	m_count--;
	if (p)				// some before us.
	{
		p->m_next = i;
		if (i == m)
			return ++Iterator(p, m);
		else
			return Iterator(i, m);
	}
	else if (i != m)	// none before us and some after us.
	{
		// substitute in p for m.
		m->swap(*i);
		m->m_next = i->m_next;
		delete i;
		return Iterator(m, m);
	}
	else				// none before us and none after us.
	{
		++_pos;
		m->deactivate();
		return _pos;
	}
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
T GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::take(Key const& _key)
{
	checkSize();
	Node* m = m_nodes + indexOf(_key);
	if (!m->isActive())
		return T();
	Node* p = 0;
	Node* i = m;
	do
	{
		if (i->key() == _key)
			goto OK;
		p = i;
	}
	while ((i = i->m_next) != m);
	return T();
	OK:
	T ret = i->value();
	Node* x = i->m_next;
	if (p)
		delete i;
	i = x;
	// l is one before the dead node or zero if it is m.
	
	if (p)				// some before us (we *were* p->m_next).
		p->m_next = i;
	else if (i != m)	// none before us and some after us (we *are* m).
	{
		// substitute in i for m.
		m->swap(*i);
		m->m_next = i->m_next;
		delete i;
	}
	else				// none before us and none after us (we *are* m).
		m->deactivate();
	m_count--;
	return ret;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
inline bool GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator::operator==(ConstIterator const& _i)
{
	return m_node == _i.m_node;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
inline bool GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator::operator!=(ConstIterator const& _i)
{
	return m_node != _i.m_node;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
void GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::clear()
{
	this->~GeneralHash();
	allocate(Min);
	m_count = 0;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::insert(Key const& _key, T const& _value)
{
	if (AlwaysMulti)
		return insertMulti(_key, _value);
	checkSize();
	t::uint index = indexOf(_key);
	Node* nn = findNode(m_nodes + index, _key);
	if (nn)
		nn->value() = _value;
	else
	{
		nn = freshNode(index);
		m_count++;
		nn->set(_key, _value);
	}
	return Iterator(nn, m_nodes + index);
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Iterator GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::insertMulti(Key const& _key, T const& _value)
{
	checkSize();
	t::uint index = indexOf(_key);
	Node* nn = findNode(m_nodes + index, _key);
	if (nn)
		nn = insertNode(nn);
	else
		nn = freshNode(index);
	m_count++;
	nn->set(_key, _value);
	return Iterator(nn, m_nodes + index);
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
List<T> GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::values() const
{
	List<T> ret;
	for (t::uint i = 0; i < m_capacity; i++)
		if (m_nodes[i])
		{
			Node* m = m_nodes + i;
			Node* j = m;
			do
				ret << j->value();
			while ((j = j->m_next) != m);
		}
	return ret;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
void GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::copy(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other)
{
	allocate(_other.m_capacity);
	for (t::uint i = 0; i < _other.m_capacity; i++)
		if (_other.m_nodes[i])
		{
			Node* l = m_nodes + i;
			Node* m = _other.m_nodes + i;
			l->activate();
			l->copy(*m);
			Node* j = m;
			while ((j = j->m_next) != m)
			{
				l = appendNode(l);
				l->copy(*j);
			}
		}
	m_count = _other.m_count;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>& GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::unite(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other)
{
	reserve(1 << (floorLog2((m_count + _other.m_count) * 2) + 1));
	for (t::uint i = 0; i < _other.m_capacity; i++)
		if (_other.m_nodes[i])
		{
			Node* m = _other.m_nodes + i;
			Node* j = m;
			do
			{
				Key const& key = j->key();
				t::uint index = indexOf(key);
				Node* nn = findNode(m_nodes + index, key);
				if (nn)
					nn = insertNode(nn);
				else
					nn = freshNode(index);
				nn->set(key, j->value());
				while (j->m_next != m && j->m_next->key() == key)
				{
					j = j->m_next;
					nn = appendNode(nn);
					nn->set(key, j->value());
				}
			}
			while ((j = j->m_next) != m);
		}
	return *this;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
Key const GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::key(T const& _value) const
{
	for (t::uint i = 0; i < m_capacity; i++)
		if (m_nodes[i])
		{
			Node* m = m_nodes + i;
			Node* j = m;
			do
				if (j->value() == _value)
					return j->key();
			while ((j = j->m_next) != m);
		}
	return Key();
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
Key const GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::key(T const& _value, Key const& _defaultKey) const
{
	for (t::uint i = 0; i < m_capacity; i++)
		if (m_nodes[i])
		{
			Node* m = m_nodes + i;
			Node* j = m;
			do
				if (j->value() == _value)
					return j->key();
			while ((j = j->m_next) != m);
		}
	return _defaultKey;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
List<Key> GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::keys(T const& _value) const
{
	List<Key> ret;
	for (t::uint i = 0; i < m_capacity; i++)
		if (m_nodes[i])
		{
			Node* m = m_nodes + i;
			Node* j = m;
			do
				if (j->value() == _value)
				{
					Key const& key = j->key();
					ret << key;
					while (j->m_next != m && j->m_next->key() == key)
						j = j->m_next;
				}
			while ((j = j->m_next) != m);
		}
	return ret;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
List<T> GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::values(Key const& _key) const
{
	List<T> ret;
	Node* m = m_nodes + indexOf(_key);
	if (!m->isActive())
		return ret;
	Node* j = m;
	do
		if (j->key() == _key)
			ret << j->value();
		else if (!ret.isEmpty())
			break;
	while ((j = j->m_next) != m);
	return ret;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
List<Key> GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::keys() const
{
	List<Key> ret;
	for (t::uint i = 0; i < m_capacity; i++)
		if (m_nodes[i])
		{
			Node* m = m_nodes + i;
			Node* j = m;
			do
				ret += j->key();
			while ((j = j->m_next) != m);
		}
	return ret;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
bool GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::contains(Key const& _key, T const& _value) const
{
	Node* m = m_nodes + indexOf(_key);
	if (!m->isActive())
		return false;
	bool found = false;
	Node* j = m;
	do
		if (j->key() == _key)
			if (j->value() == _value)
				return true;
			else
				found = true;
		else if (found)
			break;
	while ((j = j->m_next) != m);
	return false;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
bool GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::operator==(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) const
{
	if (m_count != _other.m_count)
		return false;
	for (t::uint i = 0; i < m_capacity; i++)
		if (m_nodes[i])
		{
			Node* m = m_nodes + i;
			Node* j = m;
			do
				if (!_other.contains(j->key(), j->value()))
					return false;
			while ((j = j->m_next) != m);
		}
	return true;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
List<Key> GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::uniqueKeys() const
{
	List<Key> ret;
	for (t::uint i = 0; i < m_capacity; i++)
		if (m_nodes[i])
		{
			Node* m = m_nodes + i;
			Node* j = m;
			do
			{
				Key const& key = j->key();
				ret << key;
				while (j->m_next != m && j->m_next->key() == key)
					j = j->m_next;
			}
			while ((j = j->m_next) != m);
		}
	return ret;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
void GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::resizeToPowerOfTwo(t::uint _size)
{
	ASSERT(isPOT(_size));
	ASSERT(_size >= _Min);
	if (m_capacity == _size)
		return;
	Node* oldNodes = m_nodes;
	int oldCapacity = m_capacity;
	allocate(_size);
	if (oldNodes)
		moveAndDelete(oldNodes, oldCapacity);
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
void GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::moveAndDelete(Node* _n, t::uint _c)
{
	if (m_capacity > _c)
	{	
		// capacity increasing; don't know the index off hand (one of (m_capacity / _c) though).
		t::uint lowerBits = floorLog2(_c);
		for (t::uint ii = 0; ii < _c; ii++)
		{
			Node** ls = new Node*[m_capacity / _c];
			memset(ls, 0, sizeof(Node*) * m_capacity / _c);
			if (_n[ii])
			{
				Node* m = _n + ii;
				Node* j = m;
				Node* x;
				do
				{
					x = j->m_next;
					t::uint i = indexOf(j->key());
					Node* l = ls[i >> lowerBits];
					if (l)				// If there's already one in destination bucket and the source is not a master node
					{
						ASSERT(j != m);
						// stitch...
						j->m_next = l->m_next;
						l->m_next = j;
						l = j;
					}
					else
					{
						// create... (empty destination list)
						l = freshNode(m_nodes + i);
						l->swap(*j);
						if (j != m)
							delete j;
					}
					ls[i >> lowerBits] = l;
					j = x;
				}
				while ((j = x) != m);
			}
			delete [] ls;
		}
	}
	else
	{
		// capacity decreasing; 
		// go through new buckets, then iterate over possible old buckets.
		for (t::uint i = 0; i < m_capacity; i++)
		{
			ASSERT(!m_nodes[i]);
			Node* l = 0;
			for (t::uint ii = i; ii < _c; ii += m_capacity)
				if (_n[ii])
				{
					Node* m = _n + ii;
					Node* j = m;
					Node* x;
					do
					{
						x = j->m_next;
						if (j != m)
						{
							ASSERT(l);
							// stitch...
							j->m_next = l->m_next;
							l->m_next = j;
							l = j;
						}
						else
						{
							// create... (assigning from a master node)
							ASSERT(j == m);
							if (l)
								l = appendNode(l);
							else
								l = freshNode(i);
							l->swap(*j);
							if (j != m)
								delete j;
						}
					}
					while ((j = x) != m);
				}
		}
	}
	deallocate(_n);
}
	
template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey> void GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::reinsertNode(Node* _n, bool _pleaseDelete)
{
	// appends the node to the list.
	t::uint index = indexOf(_n->key());
	if (m_nodes[index] && _pleaseDelete)
	{
		// go to end...
		Node* m = m_nodes + index;
		Node* l = m;
		while (l->m_next != m) l = l->m_next;
		// stitch...
		l->m_next = _n;
		_n->m_next = m;
	}
	else
	{
		Node* nn = freshNode(index);
		nn->swap(*_n);
		if (_pleaseDelete)
			delete _n;
	}
}

#if defined(QT_DEBUG) || defined(QT_NO_DEBUG)
template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
QDebug GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::streamToDebug(QDebug _stream) const
{
	_stream << "#{";
	bool first = true;
	for (t::uint i = 0; i < m_capacity; i++)
		if (m_nodes[i])
		{
			Node* m = m_nodes + i;
			Node* j = m;
			do
			{
				Key const& key = j->key();
				if (first)
					first = false;
				else
					_stream << ",";
				_stream << key << "=> {" << j->value();
				while (j->m_next != m && j->m_next->key() == key)
					_stream << "," << (j = j->m_next)->value();
				_stream << "}";
			}
			while ((j = j->m_next) != m);
		}
	return _stream << "}";
}
#endif

}
