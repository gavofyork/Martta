/**
 * @filename msList.h
 * @author Gavin Wood
 * @date 03/06/2008
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

#if defined(QT_DEBUG) //|| defined(QT_NO_DEBUG)
#include <QDebug>
#endif

#include "msSupport.h"
#include "msDebug.h"

namespace MarttaSupport
{

template<typename T>
class M_EXPORT List
{
#if defined(QT_DEBUG) //|| defined(QT_NO_DEBUG)
	friend m_inline QDebug operator<<(QDebug _stream, ::MarttaSupport::List<T> const& _me)
	{
		return _me.streamToDebug(_stream);
	}
#endif
	friend m_inline TextStream& operator<<(TextStream& _stream, ::MarttaSupport::List<T> const& _me)
	{
		return _me.streamToDebug(_stream);
	}

	typedef typename isSimple<T>::StorageType ST;

public:
	class Box;
	friend class Box;
	class ConstIterator;

	enum { isTrivial = isSimple<T>::value };

	// star; use instead of * operator on a item of m_data.
	m_inline static T& star(ST& _d) { if (!isTrivial) return **(T**)&_d; else return *(T*)&_d; }
	m_inline static T const& star(ST const& _d) { if (!isTrivial) return **(T const**)&_d; else return *(T const*)&_d; }

	class Box
	{
		template<typename T2> friend class List;

	public:
		m_inline Box(): m_data(0), m_count(0), m_reserved(0) {}
		m_inline Box(List<T> const& _return): m_data(0), m_count(0), m_reserved(0) { operator=(_return); }
		m_inline Box(Box const& _move): m_data(0), m_count(0), m_reserved(0) { operator=(_move); }
		Box& operator=(List<T> const& _return);
		Box& operator=(Box const& _move);
		~Box();

	private:
		ST* m_data;
		int m_count;
		int m_reserved;
	};

	class M_EXPORT Iterator
	{
		friend class List;
		class ConstIterator;

	public:
		Iterator(): m_ptr(0) {}
		Iterator(Iterator const& _i): m_ptr(_i.m_ptr) {}
		m_inline T const& operator*() const { return star(*m_ptr); }
		m_inline T const* operator->() const { return &star(*m_ptr); }
		m_inline Iterator operator+(int _n) const { return Iterator(m_ptr + _n); }
		m_inline Iterator& operator++(int) { Iterator ret(*this); m_ptr++; return ret; }
		m_inline Iterator& operator++() { m_ptr++; return *this; }
		m_inline Iterator& operator+=(int _n) { m_ptr += _n; return *this; }
		m_inline int operator-(Iterator _i) const { return m_ptr - _i.m_ptr; }
		m_inline Iterator operator-(int _n) const { return Iterator(m_ptr - _n); }
		m_inline Iterator& operator--(int) { Iterator ret(*this); m_ptr--; return ret; }
		m_inline Iterator& operator--() { m_ptr--; return *this; }
		m_inline Iterator& operator-=(int _n) { m_ptr -= _n; return *this; }
		m_inline bool operator==(Iterator const& _i) { return m_ptr == _i.m_ptr; }
		m_inline bool operator==(ConstIterator const& _i);
		m_inline bool operator!=(Iterator const& _i) { return m_ptr != _i.m_ptr; }
		m_inline bool operator!=(ConstIterator const& _i);
		m_inline bool operator<(Iterator const& _other) const { return m_ptr < _other.m_ptr; }
		m_inline bool operator<(ConstIterator const& _other) const;
		m_inline bool operator<=(Iterator const& _other) const { return m_ptr <= _other.m_ptr; }
		m_inline bool operator<=(ConstIterator const& _other) const;
		m_inline bool operator>(Iterator const& _other) const { return m_ptr > _other.m_ptr; }
		m_inline bool operator>(ConstIterator const& _other) const;
		m_inline bool operator>=(Iterator const& _other) const { return m_ptr >= _other.m_ptr; }
		m_inline bool operator>=(ConstIterator const& _other) const;

	private:
		Iterator(ST* _p): m_ptr(_p) {}
		ST* m_ptr;
	};
	class M_EXPORT ConstIterator
	{
		friend class List;
		friend class Iterator;

	public:
		ConstIterator(): m_ptr(0) {}
		ConstIterator(ConstIterator const& _i): m_ptr(_i.m_ptr) {}
		m_inline T const& operator*() const { return star(*m_ptr); }
		m_inline T const* operator->() const { return &star(*m_ptr); }
		m_inline ConstIterator operator+(int _n) const { return ConstIterator(m_ptr + _n); }
		m_inline ConstIterator& operator++(int) { ConstIterator ret(*this); m_ptr++; return ret; }
		m_inline ConstIterator& operator++() { m_ptr++; return *this; }
		m_inline ConstIterator& operator+=(int _n) { m_ptr += _n; return *this; }
		m_inline int operator-(ConstIterator _i) const { return m_ptr - _i.m_ptr; }
		m_inline ConstIterator operator-(int _n) const { return ConstIterator(m_ptr - _n); }
		m_inline ConstIterator& operator--(int) { ConstIterator ret(*this); m_ptr--; return ret; }
		m_inline ConstIterator& operator--() { m_ptr--; return *this; }
		m_inline ConstIterator& operator-=(int _n) { m_ptr -= _n; return *this; }
		m_inline bool operator==(Iterator const& _i) { return m_ptr == _i.m_ptr; }
		m_inline bool operator==(ConstIterator const& _i) { return m_ptr == _i.m_ptr; }
		m_inline bool operator!=(Iterator const& _i) { return m_ptr != _i.m_ptr; }
		m_inline bool operator!=(ConstIterator const& _i) { return m_ptr != _i.m_ptr; }
		m_inline bool operator<(Iterator const& _other) const { return m_ptr < _other.m_ptr; }
		m_inline bool operator<(ConstIterator const& _other) const { return m_ptr < _other.m_ptr; }
		m_inline bool operator<=(Iterator const& _other) const { return m_ptr <= _other.m_ptr; }
		m_inline bool operator<=(ConstIterator const& _other) const { return m_ptr <= _other.m_ptr; }
		m_inline bool operator>(Iterator const& _other) const { return m_ptr > _other.m_ptr; }
		m_inline bool operator>(ConstIterator const& _other) const { return m_ptr > _other.m_ptr; }
		m_inline bool operator>=(Iterator const& _other) const { return m_ptr >= _other.m_ptr; }
		m_inline bool operator>=(ConstIterator const& _other) const { return m_ptr >= _other.m_ptr; }

	private:
		ConstIterator(ST* _p): m_ptr(_p) {}
		ST* m_ptr;
	};

	typedef Iterator iterator;
	typedef ConstIterator const_iterator;

	m_inline List(): m_data(0), m_count(0), m_reserved(0) {}
	List(List<T> const& _other);
	List(Box const& _returned): m_data(0), m_count(0), m_reserved(0) { operator=(_returned); }
	m_inline ~List() { clear(); }

	List<T>& operator=(Box const& _returned);

	void append(T const& _value);
	m_inline T const& at(int _i) const { AssertNR(_i >= 0 && _i < m_count); return star(m_data[_i]); }
	m_inline Iterator begin() { return Iterator(m_data); }
	m_inline ConstIterator begin() const { return ConstIterator(m_data); }
	void clear();
	m_inline ConstIterator constBegin() const { return ConstIterator(m_data); }
	m_inline ConstIterator constEnd() const { return ConstIterator(m_data + m_count); }
	bool contains(T const& _value) const;
	int count(T const& _value) const;
	m_inline int count() const { return m_count; }
	m_inline Iterator end() { return Iterator(m_data + m_count); }
	m_inline ConstIterator end() const { return ConstIterator(m_data + m_count); }
	Iterator erase(Iterator _pos);
	Iterator erase(Iterator _begin, Iterator _end);
	m_inline T& first() { AssertNR(m_count); return star(m_data[0]); }
	m_inline T const& first() const { AssertNR(m_count); return star(m_data[0]); }
	int indexOf(const T& _value, int _from = 0) const;
	void insert(int _i, const T& _value);
	m_inline Iterator insert(Iterator _before, const T& _value);
	m_inline bool isEmpty() const { return !m_count; }
	m_inline T& last() { AssertNR(m_count); return star(m_data[m_count - 1]); }
	m_inline T const& last() const { AssertNR(m_count); return star(m_data[m_count - 1]); }
	int lastIndexOf(const T& _value, int _from = -1) const;
	List<T> mid(int _pos, int _length = -1) const;
	void move(int _from, int _to);
	void prepend(const T& _value);
	int removeAll(const T& _value);
	m_inline void removeAt(int _i) { AssertNR(_i >= 0 && _i < m_count); erase(Iterator(m_data + _i)); }
	void removeFirst() { AssertNR(m_count); erase(Iterator(m_data)); }
	void removeLast() { AssertNR(m_count); erase(Iterator(m_data + m_count - 1)); }
	bool removeOne(T const& _value);
	m_inline void replace(int _i, T const& _value) { AssertNR(_i >= 0 && _i < m_count); star(m_data[_i]) = _value; }
	m_inline void replace(T const& _from, T const& _to) { int i = indexOf(_from); if (i > -1) star(m_data[i]) = _to; }
	void reserve(int _n);
	m_inline List<int> reversed() const { return List<int>(*this).reverse(); }
	List<int>& reverse();
	m_inline int size() const { return count(); }
	m_inline void swap(int _i, int _j);
	m_inline T takeAt(int _i);
	m_inline T takeFirst() { AssertNR(m_count); return takeAt(0); }
	m_inline T takeLast() { AssertNR(m_count); return takeAt(m_count - 1); }
	m_inline T value(int _i) const { if(_i >= 0 && _i < m_count) return star(m_data[_i]); else return T(); }
	m_inline T value(int _i, T const& _defaultValue) const { if(_i >= 0 && _i < m_count) return star(m_data[_i]); else return _defaultValue; }
	m_inline bool operator!=(List<T> const& _other) const { return !operator==(_other); }
	m_inline List<T> operator+(const List<T>& _other) const { return List<T>(*this) += _other; }
	List<T>& operator+=(List<T> const& _other);
	m_inline List<T>& operator+=(T const& _value) { append(_value); return *this; }
	m_inline List<T>& operator<<(List<T> const& _other) { return operator+=(_other); }
	m_inline List<T>& operator<<(T const& _value) { return operator+=(_value); }
	m_inline List<T>& operator=(List<T> const& _other);
	bool operator==(List<T> const& _other) const;
	m_inline T& operator[](int _i) { AssertNR(_i >= 0 && _i < m_count); return star(m_data[_i]); }
	m_inline T const& operator[](int _i) const { return at(_i); }

private:
	/// Makes the list a bit bigger.
	void grow(int _minimum = 0);

#if defined(QT_DEBUG) //|| defined(QT_NO_DEBUG)
	QDebug streamToDebug(QDebug _stream) const;
#endif
	TextStream& streamToDebug(TextStream& _stream) const;

	ST* m_data;
	int m_count;
	int m_reserved;
};

template<class T, class F> m_inline List<T> list_cast(List<F> _f)
{
	List<T> ret;
	foreach (F i, _f)
		ret << static_cast<T>(i);
	return ret;
}

}

#include "msList.inl"
