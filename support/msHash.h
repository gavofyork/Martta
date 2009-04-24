/**
 * @filename msHash.h
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

#pragma once

#include "msSupport.h"
#include "msList.h"

namespace MarttaSupport
{

template<typename T, typename S>
class /*MS_EXPORT*/ Pair
{
public:
	typedef T First;
	typedef S Second;

	Pair() {}
	Pair(T const& _first, S const& _second): m_first(_first), m_second(_second) {}
	Pair(Pair<T, S> const& _other): m_first(_other.m_first), m_second(_other.m_second) {}

	Pair<T, S> operator=(Pair<T, S> const& _other) { m_first = _other.m_first; m_second = _other.m_second; }
	
	T& key() { return m_first; }
	S& value() { return m_second; }
	T& first() { return m_first; }
	S& second() { return m_second; }
	T const& key() const { return m_first; }
	S const& value() const { return m_second; }
	T const& first() const { return m_first; }
	S const& second() const { return m_second; }
	
	inline bool operator==(Pair<T, S> const& _c) const { return m_first == _c.m_first && m_second == _c.m_second; }
	inline bool operator!=(Pair<T, S> const& _c) const { return !operator==(_c); }
	inline bool operator<(Pair<T, S> const& _c) const { return m_first < _c.m_first || m_first == _c.m_first && m_second < _c.m_second; }
	inline bool operator<=(Pair<T, S> const& _c) const { return !operator>(_c); }
	inline bool operator>(Pair<T, S> const& _c) const { return _c.operator<(*this); }
	inline bool operator>=(Pair<T, S> const& _c) const { return _c.operator<=(*this); }
	
private:
	T m_first;
	S m_second;
};

template<typename T, typename S>
inline Pair<T, S> makePair(T const& _first, S const& _second)
{
	return Pair<T, S>(_first, _second);
}

template<class Key, class T>
inline Key keyOf(T const& _t) { return _t; }

template<typename Key, typename T, bool IK>
class PNode
{
public:
	privateinline PNode(): m_next(0) { if (sizeof(Key) > sizeof(Key*)) m_key = 0; if (sizeof(T) > sizeof(T*)) m_value = 0; }
	privateinline ~PNode() { if (sizeof(Key) > sizeof(Key*)) delete m_key; if (sizeof(T) > sizeof(T*)) delete m_value; }

	// Both set() and swap() methods must be called on inActive() nodes.
	privateinline void set(Key const& _key, T const& _value)
	{
		ASSERT(!isActive());
		if (sizeof(Key) > sizeof(Key*))
			if (m_key)
				*m_key = _key;
			else
				m_key = new Key(_key);
		else
			*(Key*)&m_key = _key;
		if (sizeof(T) > sizeof(T*))
			if (m_value)
				*m_value = _value;
			else
				m_value = new T(_value);
		else
			*(T*)&m_value = _value;
	}
	template<bool IK2>
	privateinline void copy(PNode<Key, T, IK2> const& _n)
	{
		ASSERT(!isActive());
		if (sizeof(Key) > sizeof(Key*))
			if (m_key)
				*m_key = _n.key();
			else
				m_key = new Key(_n.key());
		else
			*(Key*)&m_key = _n.key();
		if (sizeof(T) > sizeof(T*))
			if (m_value)
				*m_value = _n.value();
			else
				m_value = new T(_n.value());
		else
			*(T*)&m_value = _n.value();
	}
	privateinline void swap(PNode& _n)
	{
		ASSERT(!isActive());
		MarttaSupport::swap(m_key, _n.m_key);
		MarttaSupport::swap(m_value, _n.m_value);
	}
		
	privateinline Key const& key() const { if (sizeof(Key) > sizeof(Key*)) return *m_key; else return *(Key const*)&m_key; }
	privateinline T const& value() const { if (sizeof(T) > sizeof(T*)) return *m_value; else return *(T const*)&m_value; }
	privateinline T& value() { if (sizeof(T) > sizeof(T*)) return *m_value; else return *(T*)&m_value; }
	
	// If there's no m_next or m_previous, then we're inactive.
	privateinline operator bool() const { return isActive(); }
	privateinline bool isActive() const { return m_next && m_next != (PNode*)1; }
	// May only be called on inactive heads of rows. They become isActive().
	privateinline void activate() { ASSERT(!m_next); m_next = this; }
	// May only be called on single heads of rows. They become !isActive().
	privateinline void deactivate() { this->~PNode(); m_next = 0; m_value = 0; m_key = 0; }
	privateinline bool isSentinel() const { return m_next == (PNode*)1; }
	privateinline void setSentinel() { m_next = (PNode*)1; }

	PNode* m_next;
		
private:
	Key* m_key;
	T* m_value;
};

template<typename Key, typename T>
class PNode<Key, T, true>
{
public:
	PNode(): m_next(0) { if (sizeof(T) > sizeof(T*)) m_value = 0; }
	~PNode() { if (sizeof(T) > sizeof(T*)) delete m_value; }

	// Both set() and usurp() methods must be called on inActive() nodes.
	privateinline void set(T const& _value)
	{
		ASSERT(!isActive());
		if (sizeof(T) > sizeof(T*))
			if (m_value)
				*m_value = _value;
			else
				m_value = new T(_value);
		else
			*(T*)&m_value = _value;
	}
	template<bool IK2>
	privateinline void copy(PNode<Key, T, IK2> const& _n)
	{
		ASSERT(!isActive());
		if (sizeof(T) > sizeof(T*))
			if (m_value)
				*m_value = _n.value();
			else
				m_value = new T(_n.value());
		else
			*(T*)&m_value = _n.value();
	}
	privateinline void swap(PNode& _n)
	{
		ASSERT(!isActive());
		MarttaSupport::swap(m_value, _n.m_value);
	}
	
	privateinline Key const& key() const { return keyOf<T>(value()); }
	privateinline T const& value() const { if (sizeof(T) > sizeof(T*)) return *m_value; else return *(T const*)&m_value; }
	privateinline T& value() { if (sizeof(T) > sizeof(T*)) return *m_value; else return *(T*)&m_value; }
		
	// If there's no m_next or m_previous, then we're inactive.
	privateinline operator bool() const { return isActive(); }
	privateinline bool isActive() const { return m_next && m_next != (PNode*)1; }
	// May only be called on inactive heads of rows. They become isActive().
	privateinline void activate() { ASSERT(!m_next); m_next = this; }
	// May only be called on single heads of rows. They become !isActive().
	privateinline void deactivate() { this->~PNode(); m_next = 0; m_value = 0; }
	privateinline bool isSentinel() const { return m_next == (PNode*)1; }
	privateinline void setSentinel() { m_next = (PNode*)1; }
	
	PNode* m_next;
		
private:
	T* m_value;
};

template<typename Key, typename T, t::uint Min = 32, bool AlwaysMulti = true, bool ImplicitKey = false>
class /*MS_EXPORT*/ GeneralHash
{
	friend class Iterator;
	friend class ConstIterator;
	template<typename Key2, typename T2, t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2> friend class GeneralHash;

#if defined(QT_DEBUG) || defined(QT_NO_DEBUG)
	friend inline QDebug operator<<(QDebug _stream, ::MarttaSupport::GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey> const& _me) { return _me.streamToDebug(_stream); }
#endif

	typedef PNode<Key, T, ImplicitKey> Node;

public:
	class ConstIterator;
	class /*MS_EXPORT*/ Iterator
	{
		friend class GeneralHash;
		friend class ConstIterator;

	public:
		inline Iterator(): m_node(0), m_masterNode(0) {}
		inline Iterator(Iterator const& _i): m_node(_i.m_node), m_masterNode(_i.m_masterNode) {}
		inline T& operator*() const { return m_node->value(); }
		inline T* operator->() const { return &(m_node->value()); }
		inline Key const& key() const { return m_node->key(); }
		inline T& value() const { return m_node->value(); }
		inline Iterator operator+(int _n) const { return Iterator(*this).operator+=(_n); }
		inline Iterator operator++(int) { Iterator ret(*this); operator++(); return ret; }
		Iterator& operator++();
		inline Iterator& operator+=(int _n) { for (int i = 0; i < _n; i++) operator++(); return *this; }
		inline Iterator operator-(int _n) const { return Iterator(*this).operator-=(_n); }
		Iterator& operator--();
		inline Iterator operator--(int) { Iterator ret(*this); operator--(); return ret; }
		inline Iterator& operator-=(int _n) {  for (int i = 0; i < _n; i++) operator--(); return *this; }
		inline bool operator==(Iterator const& _i) { return m_node == _i.m_node; }
		inline bool operator!=(Iterator const& _i) { return m_node != _i.m_node; }
		inline bool operator==(ConstIterator const& _i);
		inline bool operator!=(ConstIterator const& _i);

	private:
		privateinline Iterator(Node* _n, Node* _m): m_node(_n), m_masterNode(_m) {}
		Node* m_node;
		Node* m_masterNode;
	};
	class /*MS_EXPORT*/ ConstIterator
	{
		friend class GeneralHash;
		friend class Iterator;
		
	public:
		inline ConstIterator(): m_node(0), m_masterNode(0) {}
		inline ConstIterator(ConstIterator const& _i): m_node(_i.m_node), m_masterNode(_i.m_masterNode) {}
		inline T const& operator*() const { return m_node->value(); }
		inline T const* operator->() const { return &(m_node->value()); }
		inline Key const& key() const { return m_node->key(); }
		inline T const& value() const { return m_node->value(); }
		inline ConstIterator operator+(int _n) const { return ConstIterator(*this).operator+=(_n); }
		inline ConstIterator operator++(int) { ConstIterator ret(*this); operator++(); return ret; }
		ConstIterator& operator++();
		inline ConstIterator& operator+=(int _n) { for (int i = 0; i < _n; i++) operator++(); return *this; }
		inline ConstIterator operator-(int _n) const { return ConstIterator(*this).operator-=(_n); }
		ConstIterator& operator--();
		inline ConstIterator operator--(int) { ConstIterator ret(*this); operator--(); return ret; }
		inline ConstIterator& operator-=(int _n) {  for (int i = 0; i < _n; i++) operator--(); return *this; }
		inline bool operator==(ConstIterator const& _i) { return m_node == _i.m_node; }
		inline bool operator!=(ConstIterator const& _i) { return m_node != _i.m_node; }
		inline bool operator==(Iterator const& _i) { return m_node == _i.m_node; }
		inline bool operator!=(Iterator const& _i) { return m_node != _i.m_node; }

	private:
		privateinline ConstIterator(Node const* _n, Node const* _m): m_node(_n), m_masterNode(_m) {}
		Node const* m_node;
		Node const* m_masterNode;
	};

	class Box;
	friend class Box;

	class /*MS_EXPORT*/ Box
	{
		template<typename Key2, typename T2, t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2> friend class GeneralHash;

	public:
		inline Box(): m_nodes(0), m_capacity(0), m_count(0) {}
		inline Box(Box const& _b): m_nodes(_b.m_nodes), m_capacity(_b.m_capacity), m_count(_b.m_count) { Box& b = const_cast<Box&>(_b); b.m_nodes = 0; b.m_capacity = b.m_count = 0; }
		inline Box(GeneralHash const& _h);
		~Box();
		inline Box& operator=(Box const& _b) { this->~Box(); m_nodes = _b.m_nodes; m_capacity = _b.m_capacity; m_count = _b.m_count; Box& b = const_cast<Box&>(_b); b.m_nodes = 0; b.m_capacity = b.m_count = 0; return *this; }
		inline bool isEmpty() const { return m_nodes; }
	
	private:
		Node* m_nodes;
		t::uint m_capacity;
		t::uint m_count;
	};
	
	inline GeneralHash(): m_count(0), m_autoGrow(true), m_autoShrink(true) { allocate(Min); }
	~GeneralHash();
	
	// Non-templated CC/AO for overriding defaults.
	inline GeneralHash(GeneralHash const& _other): m_count(0), m_autoGrow(true), m_autoShrink(true) { copy(_other); }
	inline GeneralHash& operator=(GeneralHash const& _other) { liveCopy(_other); return *this; }

	// Templated CC/AO.
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline GeneralHash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): m_count(0), m_autoGrow(true), m_autoShrink(true) { copy(_other); }
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline GeneralHash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }
	
	// Box CC/AO.
	template<t::uint Min2, bool AlwaysMulti2>
	inline GeneralHash(typename GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey>::Box const& _b): m_nodes(0) { moveFromBox(_b); }
	template<t::uint Min2, bool AlwaysMulti2>
	inline GeneralHash& operator=(typename GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey>::Box const& _b) { return moveFromBox(_b); }
	inline GeneralHash(Box const& _b): m_nodes(0) { moveFromBox<Min, AlwaysMulti>(_b); }
	inline GeneralHash& operator=(Box const& _b) { return moveFromBox<Min, AlwaysMulti>(_b); }
	
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline bool operator!=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) const { return !operator==(_other); }
	
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	bool operator==(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) const;

	inline bool autoGrow() const { return m_autoGrow; }
	inline bool autoShrink() const { return m_autoShrink; }
	inline void setAutoGrow(bool _on = true) { m_autoGrow = _on; }
	inline void setAutoShrink(bool _on = true) { m_autoShrink = _on; }
	inline void setAutoResize(bool _on = true) { m_autoGrow = m_autoShrink = _on; }
	inline void reserve(t::uint _size) { if (m_capacity >= _size) return; resize(_size); }
	inline int capacity() const { return m_capacity; }
	inline void squeeze() { resizeToPowerOfTwo(max(Min, nextHigher(m_count))); }
	inline void resize(t::uint _newSize) { resizeToPowerOfTwo(nextHigher(max(Min, _newSize) - 1)); }

	inline T& operator[](Key const& _key) { Node* n = findNode(_key); if (!n) return *insert(_key, T()); return n->value(); }
	inline T const operator[](Key const& _key) const { return value(_key); }
	
	inline bool contains(Key const& _key) const { return findNode(_key); }
	bool contains(Key const& _key, T const& _value) const;
	inline int count() const { return m_count; }
	inline bool isEmpty() const { return !m_count; }
	inline int size() const { return m_count; }
	int count(Key const& _key) const;

	void clear();
	Iterator insert(Key const& _key, T const& _value);
	Iterator insertMulti(Key const& _key, T const& _value);
	int remove(Key const& _key);
	//TODO: int remove(Key const& _key, T const& _value);
	int removeOne(Key const& _key);
	T take(Key const& _key);

	Key const key(T const& _value) const;
	Key const key(T const& _value, Key const& _defaultKey) const;
	List<Key> keys(T const& _value) const;
	List<Key> keys() const;
	List<Key> uniqueKeys() const;
	
	inline T const value(const Key& _key) const;
	inline T const value(const Key& _key, T const& _defaultValue) const;
	List<T> values() const;
	List<T> values(Key const& _key) const;

	inline ConstIterator constBegin() const { Node* n = m_nodes; while (n != m_nodes + m_capacity && !n->isActive()) ++n; return ConstIterator(n, n); }
	inline ConstIterator begin() const { return constBegin(); }
	inline Iterator begin() { Node* n = m_nodes; while (n != m_nodes + m_capacity && !n->isActive()) ++n; return Iterator(n, n); }
	inline ConstIterator constEnd() const { return ConstIterator(m_nodes + m_capacity, m_nodes + m_capacity); }
	inline ConstIterator end() const { return constEnd(); }
	inline Iterator end() { return Iterator(m_nodes + m_capacity, m_nodes + m_capacity); }
	inline ConstIterator constFind(Key const& _key) const;
	inline Iterator find(Key const& _key);
	Iterator erase(Iterator _pos);

	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	GeneralHash& unite(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other);

protected:
	// useful for assignment operators.
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline void liveCopy(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { this->~GeneralHash(); copy(_other); }

	template<t::uint Min2, bool AlwaysMulti2>
	inline GeneralHash& moveFromBox(typename GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey>::Box const& _b);

private:
	// Finds or creates the next available node for the appropriate _index.
	// The node will be considered most recent.
	privateinline Node* freshNode(t::uint _index);
	privateinline Node* freshNode(Node* _m);
	// Finds or creates the next available node for the appropriate _index.
	// The node will be considered least recent.
	privateinline Node* staleNode(t::uint _index);
	privateinline Node* staleNode(Node* _m);
	privateinline Node* appendNode(Node* _after);
	privateinline Node* insertNode(Node* _before);
	
	// copy will call allocate() first, so if you're not in a constructor you probably want liveCopy().
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	void copy(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other);
	
	void moveAndDelete(Node* _n, t::uint _c);
	void reinsertNode(Node* _n, bool _pleaseDelete);	
	privateinline int indexOf(Key const& _key) const { return hashOf(_key) & (m_capacity - 1); }
	void resizeToPowerOfTwo(t::uint _size);
	// Find most recent node of key _key.
	privateinline Node* findNode(Key const& _key) const { return findNode(m_nodes + indexOf(_key), _key); }
	// Find most recent node of key _key, given the master node _m. Slightly faster if master node is provided.
	Node* findNode(Node* _m, Key const& _key) const;
	void allocate(t::uint _s);
	void deallocate(Node*& _n);

	// Resizes the hash if necessary
	void checkSize();
	
#if defined(QT_DEBUG) || defined(QT_NO_DEBUG)
	QDebug streamToDebug(QDebug _stream) const;
#endif
	
	Node* m_nodes;
	t::uint m_capacity;
	t::uint m_count;
	bool m_autoGrow;
	bool m_autoShrink;
};

inline t::uint hashOf(char _key) { return (t::uint)_key; }
template<typename T1, typename T2> inline t::uint hashOf(Pair<T1, T2> const& _key) { t::uint fh = hashOf(_key.first()); return (fh << 16 | fh >> 16) ^ hashOf(_key.second()); }
inline t::uint hashOf(uchar _key) {  return (t::uint)_key; }
inline t::uint hashOf(signed char _key) {  return (t::uint)_key; }
inline t::uint hashOf(ushort _key) { return (t::uint)_key; }
inline t::uint hashOf(short _key) { return (t::uint)_key; }
inline t::uint hashOf(unsigned int _key) { return (t::uint)_key; }
inline t::uint hashOf(int _key) { return (t::uint)_key; }
inline t::uint hashOf(ulong _key) { return (t::uint)_key; }
inline t::uint hashOf(long _key) { return (t::uint)_key; }
#ifndef _MSC_VER
inline t::uint hashOf(wchar_t _key) { return (t::uint)_key; }
#endif
inline t::uint hashOf(char const* _key)
{
	int r = 0;
	for (char const* i = _key; *i; i++)
		r = (((r << 16) ^ *i) + ((r >> 16) ^ *i));
	return r;
}
template<typename T> inline t::uint hashOf(T const* _key) { return (t::uint)_key; }

template<typename Key, typename T, int Min = 32>
class Hash: public GeneralHash<Key, T, Min, false, false>
{
public:
	inline Hash(): GeneralHash<Key, T, Min, false, false>() {}
	
	inline Hash(Hash const& _other): GeneralHash<Key, T, Min, false, false>(_other) {}
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline Hash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): GeneralHash<Key, T, Min, false, false>(_other) {}
	
	inline Hash& operator=(Hash<Key, T, Min> const& _other) { liveCopy(_other); return *this; }
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline Hash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }

	inline Hash(typename Hash<Key, T, Min>::Box const& _b): GeneralHash<Key, T, Min, false, false>(_b) {}
	inline Hash& operator=(typename Hash<Key, T, Min>::Box const& _b) { GeneralHash<Key, T, Min, false, false>::operator=(_b); return *this; }
};

template<typename Key, typename T, int Min = 32>
class MultiHash: public GeneralHash<Key, T, Min, true, false>
{
public:
	inline MultiHash(): GeneralHash<Key, T, Min, true, false>() {}
	inline MultiHash(MultiHash<Key, T, Min> const& _other): GeneralHash<Key, T, Min, true, false>(_other) {}
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline MultiHash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): GeneralHash<Key, T, Min, true, false>(_other) {}

	inline MultiHash& operator=(MultiHash<Key, T, Min> const& _other) { liveCopy(_other); return *this; }
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline MultiHash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }

	inline MultiHash(typename MultiHash<Key, T, Min>::Box const& _b): GeneralHash<Key, T, Min, true, false>(_b) {}
	inline MultiHash& operator=(typename MultiHash<Key, T, Min>::Box const& _b) { GeneralHash<Key, T, Min, true, false>::operator=((typename GeneralHash<Key, T, Min, true, false>::Box const&)_b); return *this; }
/*	template<t::uint Min2, bool AlwaysMulti2>
	inline MultiHash(typename GeneralHash<Key, T, Min2, AlwaysMulti2, false>::Box const& _b): GeneralHash<Key, T, Min, true, false>(_b) {}
	template<t::uint Min2, bool AlwaysMulti2>
	inline MultiHash& operator=(typename GeneralHash<Key, T, Min2, AlwaysMulti2, false>::Box const& _b) { GeneralHash<Key, T, Min, true, false>::operator=(_b); return *this; }*/
};

template<typename Key, typename T, int Min = 32>
class ImplicitHash: public GeneralHash<Key, T, Min, false, true>
{
public:
	inline ImplicitHash(): GeneralHash<Key, T, Min, false, true>() {}
	inline ImplicitHash(ImplicitHash<Key, T, Min> const& _other): GeneralHash<Key, T, Min, false, true>(_other) {}
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline ImplicitHash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): GeneralHash<Key, T, Min, false, true>(_other) {}

	inline ImplicitHash& operator=(ImplicitHash<Key, T, Min> const& _other) { liveCopy(_other); return *this; }
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline ImplicitHash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }

	inline ImplicitHash(typename ImplicitHash<Key, T, Min>::Box const& _b): GeneralHash<Key, T, Min, false, true>(_b) {}
	inline ImplicitHash& operator=(typename ImplicitHash<Key, T, Min>::Box const& _b) { GeneralHash<Key, T, Min, false, true>::operator=(_b); return *this; }
/*	template<t::uint Min2, bool AlwaysMulti2>
	inline ImplicitHash(typename GeneralHash<Key, T, Min2, AlwaysMulti2, true>::Box const& _b): GeneralHash<Key, T, Min, false, true>(_b) {}
	template<t::uint Min2, bool AlwaysMulti2>
	inline ImplicitHash& operator=(typename GeneralHash<Key, T, Min2, AlwaysMulti2, true>::Box const& _b) { GeneralHash<Key, T, Min, false, true>::operator=(_b); return *this; }*/
};

template<typename Key, typename T, int Min = 32>
class ImplicitMultiHash: public GeneralHash<Key, T, Min, true, true>
{
public:
	inline ImplicitMultiHash(): GeneralHash<Key, T, Min, true, true>() {}
	inline ImplicitMultiHash(ImplicitMultiHash<Key, T, Min> const& _other): GeneralHash<Key, T, Min, true, true>(_other) {}
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline ImplicitMultiHash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): GeneralHash<Key, T, Min, true, true>(_other) {}

	inline ImplicitMultiHash& operator=(ImplicitMultiHash<Key, T, Min> const& _other) { liveCopy(_other); return *this; }
	template<t::uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	inline ImplicitMultiHash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }

	inline ImplicitMultiHash(typename ImplicitMultiHash<Key, T, Min>::Box const& _b): GeneralHash<Key, T, Min, true, true>(_b) {}
	inline ImplicitMultiHash& operator=(typename ImplicitMultiHash<Key, T, Min>::Box const& _b) { GeneralHash<Key, T, Min, true, true>::operator=(_b); return *this; }
/*	template<t::uint Min2, bool AlwaysMulti2>
	inline ImplicitMultiHash(typename GeneralHash<Key, T, Min2, AlwaysMulti2, true>::Box const& _b): GeneralHash<Key, T, Min, true, true>(_b) {}
	template<t::uint Min2, bool AlwaysMulti2>
	inline ImplicitMultiHash& operator=(typename GeneralHash<Key, T, Min2, AlwaysMulti2, true>::Box const& _b) { GeneralHash<Key, T, Min, true, true>::operator=(_b); return *this; }*/
};

}

#include "msHash.inl"
