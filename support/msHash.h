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
#include "msDebug.h"
#include "msList.h"

namespace MarttaSupport
{

template<typename T, typename S>
class M_EXPORT Pair
{
public:
	typedef T First;
	typedef S Second;

	m_inline Pair() {}
	m_inline Pair(T const& _first, S const& _second): m_first(_first), m_second(_second) {}
	m_inline Pair(Pair<T, S> const& _other): m_first(_other.m_first), m_second(_other.m_second) {}

	m_inline Pair<T, S> operator=(Pair<T, S> const& _other) { m_first = _other.m_first; m_second = _other.m_second; }

	m_inline T& key() { return m_first; }
	m_inline S& value() { return m_second; }
	m_inline T& first() { return m_first; }
	m_inline S& second() { return m_second; }
	m_inline T const& key() const { return m_first; }
	m_inline S const& value() const { return m_second; }
	m_inline T const& first() const { return m_first; }
	m_inline S const& second() const { return m_second; }

	m_inline bool operator==(Pair<T, S> const& _c) const { return m_first == _c.m_first && m_second == _c.m_second; }
	m_inline bool operator!=(Pair<T, S> const& _c) const { return !operator==(_c); }
	m_inline bool operator<(Pair<T, S> const& _c) const { return m_first < _c.m_first || m_first == _c.m_first && m_second < _c.m_second; }
	m_inline bool operator<=(Pair<T, S> const& _c) const { return !operator>(_c); }
	m_inline bool operator>(Pair<T, S> const& _c) const { return _c.operator<(*this); }
	m_inline bool operator>=(Pair<T, S> const& _c) const { return _c.operator<=(*this); }

private:
	T m_first;
	S m_second;
};

m_inline uint hashOf(char _key) { return (uint)_key; }
template<typename T1, typename T2> m_inline uint hashOf(Pair<T1, T2> const& _key) { uint fh = hashOf(_key.first()); return (fh << 16 | fh >> 16) ^ hashOf(_key.second()); }
m_inline uint hashOf(uchar _key) {  return (uint)_key; }
m_inline uint hashOf(signed char _key) {  return (uint)_key; }
m_inline uint hashOf(ushort _key) { return (uint)_key; }
m_inline uint hashOf(short _key) { return (uint)_key; }
m_inline uint hashOf(unsigned int _key) { return (uint)_key; }
m_inline uint hashOf(int _key) { return (uint)_key; }
m_inline uint hashOf(ulong _key) { return (uint)_key; }
m_inline uint hashOf(long _key) { return (uint)_key; }
#ifndef _MSC_VER
m_inline uint hashOf(wchar_t _key) { return (uint)_key; }
#endif
m_inline uint hashOf(char const* _key)
{
	int r = 0;
	for (char const* i = _key; *i; i++)
		r = (((r << 16) ^ *i) + ((r >> 16) ^ *i));
	return r;
}
template<typename T> m_inline uint hashOf(T const* _key) { return (uint)_key; }

template<typename T, typename S>
m_inline Pair<T, S> makePair(T const& _first, S const& _second)
{
	return Pair<T, S>(_first, _second);
}

template<class Key, class T>
m_inline Key keyOf(T const& _t) { return _t; }

template<typename Key, typename T, bool IK>
class PNode
{
public:
	m_privateinline PNode(): m_next(0) { initRef<Key>(m_key); initRef<T>(m_value); }
	m_privateinline ~PNode() { finRef<Key>(m_key); finRef<T>(m_value); }

	// Both set() and swap() methods must be called on isActive() nodes.
	m_privateinline void set(Key const& _key, T const& _value)
	{
		AssertNR(isActive());
		setRef(m_key, _key);
		setRef(m_value, _value);
	}
	template<bool IK2>
	m_privateinline void copy(PNode<Key, T, IK2> const& _n)
	{
		set(_n.key(), _n.value());
	}
	m_privateinline void swap(PNode& _n)
	{
		AssertNR(isActive());
		MarttaSupport::swap(m_key, _n.m_key);
		MarttaSupport::swap(m_value, _n.m_value);
	}

	m_privateinline Key const& key() const { return refFor<Key>(m_key); }
	m_privateinline T const& value() const { return refFor<T>(m_value); }
	m_privateinline T& value() { return refFor<T>(m_value); }

	// If there's no m_next or m_previous, then we're inactive.
	m_privateinline operator bool() const { return isActive(); }
	m_privateinline bool isActive() const { return m_next && m_next != (PNode*)1 && m_next != (PNode*)2; }
	// May only be called on inactive heads of rows. They become isActive().
	m_privateinline void activate() { AssertNR(!m_next); m_next = this; }
	// May only be called on single heads of rows. They become !isActive().
	m_privateinline void deactivate() { this->~PNode(); m_next = 0; m_value = 0; m_key = 0; }
	m_privateinline bool isFrontSentinel() const { return m_next == (PNode*)1; }
	m_privateinline bool isBackSentinel() const { return m_next == (PNode*)2; }
	m_privateinline void setFrontSentinel() { m_next = (PNode*)1; }
	m_privateinline void setBackSentinel() { m_next = (PNode*)2; }

	PNode* m_next;

private:
	typename isSimple<Key>::StorageType m_key;
	typename isSimple<T>::StorageType m_value;
};

template<typename Key, typename T>
class PNode<Key, T, true>
{
public:
	m_privateinline PNode(): m_next(0) { initRef<T>(m_value); }
	m_privateinline ~PNode() { finRef<T>(m_value); }

	// Both set() and swap() methods must be called on isActive() nodes.
	m_privateinline void set(T const& _value)
	{
		AssertNR(isActive());
		setRef(m_value, _value);
	}
	template<bool IK2>
	m_privateinline void copy(PNode<Key, T, IK2> const& _n)
	{
		set(_n.value());
	}
	m_privateinline void swap(PNode& _n)
	{
		AssertNR(isActive());
		MarttaSupport::swap(m_value, _n.m_value);
	}

	m_privateinline Key const& key() const { return keyOf<T>(value()); }
	m_privateinline T const& value() const { return refFor<T>(m_value); }
	m_privateinline T& value() { return refFor<T>(m_value); }

	// If there's no m_next or m_previous, then we're inactive.
	m_privateinline operator bool() const { return isActive(); }
	m_privateinline bool isActive() const { return m_next && m_next != (PNode*)1 && m_next != (PNode*)2; }
	// May only be called on inactive heads of rows. They become isActive().
	m_privateinline void activate() { AssertNR(!m_next); m_next = this; }
	// May only be called on single heads of rows. They become !isActive().
	m_privateinline void deactivate() { this->~PNode(); m_next = 0; m_value = 0; }
	m_privateinline bool isFrontSentinel() const { return m_next == (PNode*)1; }
	m_privateinline bool isBackSentinel() const { return m_next == (PNode*)2; }
	m_privateinline void setFrontSentinel() { m_next = (PNode*)1; }
	m_privateinline void setBackSentinel() { m_next = (PNode*)2; }

	PNode* m_next;

private:
	T* m_value;
};

template<typename Key, typename T, uint Min = 32, bool AlwaysMulti = true, bool ImplicitKey = false>
class M_EXPORT GeneralHash
{
	friend class Iterator;
	friend class ConstIterator;
	template<typename Key2, typename T2, uint Min2, bool AlwaysMulti2, bool ImplicitKey2> friend class GeneralHash;

#if defined(QT_DEBUG)
//	friend m_inline QDebug operator<<(QDebug _stream, ::MarttaSupport::GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey> const& _me) { return _me.streamToDebug(_stream); }
#endif
	friend m_inline TextStream& operator<<(TextStream& _stream, ::MarttaSupport::GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey> const& _me) { return _me.streamToDebug(_stream); }

	typedef PNode<Key, T, ImplicitKey> Node;

public:
	class ConstIterator;
	class M_EXPORT Iterator
	{
		friend class GeneralHash;
		friend class ConstIterator;

	public:
		m_inline Iterator(): m_node(0), m_masterNode(0) {}
		m_inline Iterator(Iterator const& _i): m_node(_i.m_node), m_masterNode(_i.m_masterNode) {}
		m_inline T& operator*() const { return m_node->value(); }
		m_inline T* operator->() const { return &(m_node->value()); }
		m_inline Key const& key() const { return m_node->key(); }
		m_inline T& value() const { return m_node->value(); }
		m_inline Iterator operator+(int _n) const { return Iterator(*this).operator+=(_n); }
		m_inline Iterator operator++(int) { Iterator ret(*this); operator++(); return ret; }
		m_inline Iterator& operator++();
		m_inline Iterator& operator+=(int _n) { for (int i = 0; i < _n; i++) operator++(); return *this; }
		m_inline Iterator operator-(int _n) const { return Iterator(*this).operator-=(_n); }
		m_inline Iterator& operator--();
		m_inline Iterator operator--(int) { Iterator ret(*this); operator--(); return ret; }
		m_inline Iterator& operator-=(int _n) {  for (int i = 0; i < _n; i++) operator--(); return *this; }
		m_inline bool operator==(Iterator const& _i) { return m_node == _i.m_node; }
		m_inline bool operator!=(Iterator const& _i) { return m_node != _i.m_node; }
		m_inline bool operator==(ConstIterator const& _i);
		m_inline bool operator!=(ConstIterator const& _i);

	private:
		m_privateinline Iterator(Node* _n, Node* _m): m_node(_n), m_masterNode(_m) {}
		Node* m_node;
		Node* m_masterNode;
	};
	class M_EXPORT ConstIterator
	{
		friend class GeneralHash;
		friend class Iterator;

	public:
		m_inline ConstIterator(): m_node(0), m_masterNode(0) {}
		m_inline ConstIterator(ConstIterator const& _i): m_node(_i.m_node), m_masterNode(_i.m_masterNode) {}
		m_inline T const& operator*() const { return m_node->value(); }
		m_inline T const* operator->() const { return &(m_node->value()); }
		m_inline Key const& key() const { return m_node->key(); }
		m_inline T const& value() const { return m_node->value(); }
		m_inline ConstIterator operator+(int _n) const { return ConstIterator(*this).operator+=(_n); }
		m_inline ConstIterator operator++(int) { ConstIterator ret(*this); operator++(); return ret; }
		m_inline ConstIterator& operator++();
		m_inline ConstIterator& operator+=(int _n) { for (int i = 0; i < _n; i++) operator++(); return *this; }
		m_inline ConstIterator operator-(int _n) const { return ConstIterator(*this).operator-=(_n); }
		m_inline ConstIterator& operator--();
		m_inline ConstIterator operator--(int) { ConstIterator ret(*this); operator--(); return ret; }
		m_inline ConstIterator& operator-=(int _n) {  for (int i = 0; i < _n; i++) operator--(); return *this; }
		m_inline bool operator==(ConstIterator const& _i) { return m_node == _i.m_node; }
		m_inline bool operator!=(ConstIterator const& _i) { return m_node != _i.m_node; }
		m_inline bool operator==(Iterator const& _i) { return m_node == _i.m_node; }
		m_inline bool operator!=(Iterator const& _i) { return m_node != _i.m_node; }

	private:
		m_privateinline ConstIterator(Node const* _n, Node const* _m): m_node(_n), m_masterNode(_m) {}
		Node const* m_node;
		Node const* m_masterNode;
	};

	class Box;
	friend class Box;

	class M_EXPORT Box
	{
		template<typename Key2, typename T2, uint Min2, bool AlwaysMulti2, bool ImplicitKey2> friend class GeneralHash;

	public:
		m_inline Box(): m_nodes(0), m_capacity(0), m_count(0) {}
		m_inline Box(Box const& _b): m_nodes(_b.m_nodes), m_capacity(_b.m_capacity), m_count(_b.m_count) { Box& b = const_cast<Box&>(_b); b.m_nodes = 0; b.m_capacity = b.m_count = 0; }
		m_inline Box(GeneralHash const& _h);
		m_inline ~Box();
		m_inline Box& operator=(Box const& _b) { this->~Box(); m_nodes = _b.m_nodes; m_capacity = _b.m_capacity; m_count = _b.m_count; Box& b = const_cast<Box&>(_b); b.m_nodes = 0; b.m_capacity = b.m_count = 0; return *this; }
		m_inline bool isEmpty() const { return m_nodes; }

	private:
		Node* m_nodes;
		uint m_capacity;
		uint m_count;
	};

	typedef Iterator iterator;
	typedef ConstIterator const_iterator;

	m_inline GeneralHash(): m_count(0), m_autoGrow(true), m_autoShrink(true) { allocate(Min); }
	m_inline ~GeneralHash();

	// Non-templated CC/AO for overriding defaults.
	m_inline GeneralHash(GeneralHash const& _other): m_count(0), m_autoGrow(true), m_autoShrink(true) { copy(_other); }
	m_inline GeneralHash& operator=(GeneralHash const& _other) { liveCopy(_other); return *this; }

	// Templated CC/AO.
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline GeneralHash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): m_count(0), m_autoGrow(true), m_autoShrink(true) { copy(_other); }
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline GeneralHash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }

	// Box CC/AO.
	template<uint Min2, bool AlwaysMulti2>
	m_inline GeneralHash(typename GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey>::Box const& _b): m_nodes(0) { moveFromBox(_b); }
	template<uint Min2, bool AlwaysMulti2>
	m_inline GeneralHash& operator=(typename GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey>::Box const& _b) { return moveFromBox(_b); }
	m_inline GeneralHash(Box const& _b): m_nodes(0) { moveFromBox<Min, AlwaysMulti>(_b); }
	m_inline GeneralHash& operator=(Box const& _b) { return moveFromBox<Min, AlwaysMulti>(_b); }

	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline bool operator!=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) const { return !operator==(_other); }

	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline bool operator==(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) const;

	m_inline bool autoGrow() const { return m_autoGrow; }
	m_inline bool autoShrink() const { return m_autoShrink; }
	m_inline void setAutoGrow(bool _on = true) { m_autoGrow = _on; }
	m_inline void setAutoShrink(bool _on = true) { m_autoShrink = _on; }
	m_inline void setAutoResize(bool _on = true) { m_autoGrow = m_autoShrink = _on; }
	m_inline void reserve(uint _size) { if (m_capacity >= _size) return; resize(_size); }
	m_inline int capacity() const { return m_capacity; }
	m_inline void squeeze() { resizeToPowerOfTwo(max(Min, nextHigher(m_count))); }
	m_inline void resize(uint _newSize) { resizeToPowerOfTwo(nextHigher(max(Min, _newSize) - 1)); }

	m_inline T& operator[](Key const& _key) { Node* n = findNode(_key); if (!n) return *insert(_key, T()); return n->value(); }
	m_inline T const operator[](Key const& _key) const { return value(_key); }

	m_inline bool contains(Key const& _key) const { return findNode(_key); }
	m_inline bool contains(Key const& _key, T const& _value) const;
	m_inline int count() const { return m_count; }
	m_inline bool isEmpty() const { return !m_count; }
	m_inline int size() const { return m_count; }
	m_inline int count(Key const& _key) const;

	m_inline void clear();
	m_inline Iterator insert(Key const& _key, T const& _value);
	m_inline Iterator insertMulti(Key const& _key, T const& _value);
	m_inline int remove(Key const& _key);
	m_inline int remove(Key const& _key, T const& _value) { int r = 0; while (removeOne(_key, _value)) r++; return r; }
	m_inline int removeOne(Key const& _key);
	m_inline int removeOne(Key const& _key, T const& _value);
	m_inline T take(Key const& _key);

	m_inline Key const key(T const& _value) const;
	m_inline Key const key(T const& _value, Key const& _defaultKey) const;
	m_inline List<Key> keys(T const& _value) const;
	m_inline List<Key> keys() const;
	m_inline List<Key> uniqueKeys() const;

	m_inline T const value(const Key& _key) const;
	m_inline T const value(const Key& _key, T const& _defaultValue) const;
	m_inline List<T> values() const;
	m_inline List<T> values(Key const& _key) const;


	m_inline GeneralHash<T, Key, Min, AlwaysMulti, ImplicitKey> inverted() const;

	m_inline ConstIterator constBegin() const { Node* n = m_nodes; while (n != m_nodes + m_capacity && !n->isActive()) ++n; return ConstIterator(n, n); }
	m_inline ConstIterator begin() const { return constBegin(); }
	m_inline Iterator begin() { Node* n = m_nodes; while (n != m_nodes + m_capacity && !n->isActive()) ++n; return Iterator(n, n); }
	m_inline ConstIterator constEnd() const { return ConstIterator(m_nodes + m_capacity, m_nodes + m_capacity); }
	m_inline ConstIterator end() const { return constEnd(); }
	m_inline Iterator end() { return Iterator(m_nodes + m_capacity, m_nodes + m_capacity); }
	m_inline ConstIterator constFind(Key const& _key) const;
	m_inline ConstIterator find(Key const& _key) const { return constFind(_key); }
	m_inline Iterator find(Key const& _key);
	m_inline ConstIterator constFind(Key const& _key, T const& _value) const;
	m_inline ConstIterator find(Key const& _key, T const& _value) const { return constFind(_key, _value); }
	m_inline Iterator find(Key const& _key, T const& _value);
	m_inline Iterator erase(Iterator _pos);

	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline GeneralHash& unite(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other);

protected:
	// useful for assignment operators.
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline void liveCopy(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { this->~GeneralHash(); copy(_other); }

	template<uint Min2, bool AlwaysMulti2>
	m_inline GeneralHash& moveFromBox(typename GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey>::Box const& _b);

private:
	// Finds or creates the next available node for the appropriate _index.
	// The node will be considered most recent.
	m_privateinline Node* freshNode(uint _index);
	m_privateinline Node* freshNode(Node* _m);
	// Finds or creates the next available node for the appropriate _index.
	// The node will be considered least recent.
	m_privateinline Node* staleNode(uint _index);
	m_privateinline Node* staleNode(Node* _m);
	m_privateinline Node* appendNode(Node* _after);
	m_privateinline Node* insertNode(Node* _before);

	// copy will call allocate() first, so if you're not in a constructor you probably want liveCopy().
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline void copy(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other);

	m_inline void moveAndDelete(Node* _n, uint _c);
	m_inline void reinsertNode(Node* _n, bool _pleaseDelete);
	m_privateinline int indexOf(Key const& _key) const { return hashOf(_key) & (m_capacity - 1); }
	m_inline void resizeToPowerOfTwo(uint _size);
	// Find most recent node of key _key.
	m_privateinline Node* findNode(Key const& _key) const { return findNode(m_nodes + indexOf(_key), _key); }
	// Find most recent node of key _key, given the master node _m. Slightly faster if master node is provided.
	m_inline Node* findNode(Node* _m, Key const& _key) const;
	m_inline Node* findNode(Node* _m, Key const& _key, T const& _value) const;
	m_inline void allocate(uint _s);
	m_inline void deallocate(Node*& _n);

	// Resizes the hash if necessary
	m_inline void checkSize();

#if defined(QT_DEBUG) || defined(QT_NO_DEBUG)
	m_inline QDebug streamToDebug(QDebug _stream) const;
#endif
	m_inline TextStream& streamToDebug(TextStream& _stream) const;

	Node* m_nodes;
	uint m_capacity;
	uint m_count;
	bool m_autoGrow;
	bool m_autoShrink;
};

template<typename Key, typename T, int Min = 32>
class Hash: public GeneralHash<Key, T, Min, false, false>
{
public:
	m_inline Hash(): GeneralHash<Key, T, Min, false, false>() {}

	m_inline Hash(Hash const& _other): GeneralHash<Key, T, Min, false, false>(_other) {}
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline Hash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): GeneralHash<Key, T, Min, false, false>(_other) {}

	m_inline Hash& operator=(Hash<Key, T, Min> const& _other) { liveCopy(_other); return *this; }
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline Hash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }

	m_inline Hash(typename Hash<Key, T, Min>::Box const& _b): GeneralHash<Key, T, Min, false, false>(_b) {}
	m_inline Hash& operator=(typename Hash<Key, T, Min>::Box const& _b) { GeneralHash<Key, T, Min, false, false>::operator=(_b); return *this; }
};

template<typename Key, typename T, int Min = 32>
class MultiHash: public GeneralHash<Key, T, Min, true, false>
{
public:
	m_inline MultiHash(): GeneralHash<Key, T, Min, true, false>() {}
	m_inline MultiHash(MultiHash<Key, T, Min> const& _other): GeneralHash<Key, T, Min, true, false>(_other) {}
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline MultiHash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): GeneralHash<Key, T, Min, true, false>(_other) {}

	m_inline MultiHash& operator=(MultiHash<Key, T, Min> const& _other) { liveCopy(_other); return *this; }
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline MultiHash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }

	m_inline MultiHash(typename MultiHash<Key, T, Min>::Box const& _b): GeneralHash<Key, T, Min, true, false>(_b) {}
	m_inline MultiHash& operator=(typename MultiHash<Key, T, Min>::Box const& _b) { GeneralHash<Key, T, Min, true, false>::operator=((typename GeneralHash<Key, T, Min, true, false>::Box const&)_b); return *this; }
/*	template<uint Min2, bool AlwaysMulti2>
	m_inline MultiHash(typename GeneralHash<Key, T, Min2, AlwaysMulti2, false>::Box const& _b): GeneralHash<Key, T, Min, true, false>(_b) {}
	template<uint Min2, bool AlwaysMulti2>
	m_inline MultiHash& operator=(typename GeneralHash<Key, T, Min2, AlwaysMulti2, false>::Box const& _b) { GeneralHash<Key, T, Min, true, false>::operator=(_b); return *this; }*/
};

template<typename Key, typename T, int Min = 32>
class ImplicitHash: public GeneralHash<Key, T, Min, false, true>
{
public:
	m_inline ImplicitHash(): GeneralHash<Key, T, Min, false, true>() {}
	m_inline ImplicitHash(ImplicitHash<Key, T, Min> const& _other): GeneralHash<Key, T, Min, false, true>(_other) {}
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline ImplicitHash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): GeneralHash<Key, T, Min, false, true>(_other) {}

	m_inline ImplicitHash& operator=(ImplicitHash<Key, T, Min> const& _other) { liveCopy(_other); return *this; }
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline ImplicitHash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }

	m_inline ImplicitHash(typename ImplicitHash<Key, T, Min>::Box const& _b): GeneralHash<Key, T, Min, false, true>(_b) {}
	m_inline ImplicitHash& operator=(typename ImplicitHash<Key, T, Min>::Box const& _b) { GeneralHash<Key, T, Min, false, true>::operator=(_b); return *this; }
/*	template<uint Min2, bool AlwaysMulti2>
	m_inline ImplicitHash(typename GeneralHash<Key, T, Min2, AlwaysMulti2, true>::Box const& _b): GeneralHash<Key, T, Min, false, true>(_b) {}
	template<uint Min2, bool AlwaysMulti2>
	m_inline ImplicitHash& operator=(typename GeneralHash<Key, T, Min2, AlwaysMulti2, true>::Box const& _b) { GeneralHash<Key, T, Min, false, true>::operator=(_b); return *this; }*/
};

template<typename Key, typename T, int Min = 32>
class ImplicitMultiHash: public GeneralHash<Key, T, Min, true, true>
{
public:
	m_inline ImplicitMultiHash(): GeneralHash<Key, T, Min, true, true>() {}
	m_inline ImplicitMultiHash(ImplicitMultiHash<Key, T, Min> const& _other): GeneralHash<Key, T, Min, true, true>(_other) {}
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline ImplicitMultiHash(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other): GeneralHash<Key, T, Min, true, true>(_other) {}

	m_inline ImplicitMultiHash& operator=(ImplicitMultiHash<Key, T, Min> const& _other) { liveCopy(_other); return *this; }
	template<uint Min2, bool AlwaysMulti2, bool ImplicitKey2>
	m_inline ImplicitMultiHash& operator=(GeneralHash<Key, T, Min2, AlwaysMulti2, ImplicitKey2> const& _other) { liveCopy(_other); return *this; }

	m_inline ImplicitMultiHash(typename ImplicitMultiHash<Key, T, Min>::Box const& _b): GeneralHash<Key, T, Min, true, true>(_b) {}
	m_inline ImplicitMultiHash& operator=(typename ImplicitMultiHash<Key, T, Min>::Box const& _b) { GeneralHash<Key, T, Min, true, true>::operator=(_b); return *this; }
/*	template<uint Min2, bool AlwaysMulti2>
	m_inline ImplicitMultiHash(typename GeneralHash<Key, T, Min2, AlwaysMulti2, true>::Box const& _b): GeneralHash<Key, T, Min, true, true>(_b) {}
	template<uint Min2, bool AlwaysMulti2>
	m_inline ImplicitMultiHash& operator=(typename GeneralHash<Key, T, Min2, AlwaysMulti2, true>::Box const& _b) { GeneralHash<Key, T, Min, true, true>::operator=(_b); return *this; }*/
};

template<class T> m_inline List<T> solve(List<T> const& _q, MultiHash<T, T> const& _deps)
{
	List<T> q = _q;
	List<T> ret;
	MultiHash<T, T> deps = _deps;
	MultiHash<T, T> depees = _deps.inverted();
	while (!q.isEmpty())
	{
		T n = q.takeLast();
		ret << n;
		foreach (T m, depees.values(n))	// go through all that depend on n
		{
			depees.removeOne(n, m);
			deps.removeOne(m, n);
			if (!deps.count(m))			// if they themselves don't depend on anything else, whack them in q.
				q << m;
		}
	}

	if (deps.size())
		return List<T>();
	return ret;
}

}

#include "msHash.inl"
