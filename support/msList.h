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

namespace MarttaSupport
{

template<typename T> class isSimple { public: enum { value = false }; typedef T* StorageType; };
template<typename T> class isSimple<T*> { public: enum { value = true }; typedef T* StorageType; };
#define DECLARE_SIMPLE_TYPE(T) template<> class isSimple<T> { public: enum { value = true }; typedef T StorageType; };
template<> class isSimple<bool> { public: enum { value = true }; typedef bool StorageType; };
#ifndef _MSC_VER
template<> class isSimple<wchar_t> { public: enum { value = true }; typedef wchar_t StorageType; };
#endif
template<> class isSimple<signed char> { public: enum { value = true }; typedef signed char StorageType; };
template<> class isSimple<unsigned char> { public: enum { value = true }; typedef unsigned char StorageType; };
template<> class isSimple<signed short> { public: enum { value = true }; typedef signed short StorageType; };
template<> class isSimple<unsigned short> { public: enum { value = true }; typedef unsigned short StorageType; };
template<> class isSimple<signed int> { public: enum { value = true }; typedef signed int StorageType; };
template<> class isSimple<unsigned int> { public: enum { value = true }; typedef unsigned int StorageType; };
template<> class isSimple<signed long> { public: enum { value = true }; typedef signed long StorageType; };
template<> class isSimple<unsigned long> { public: enum { value = true }; typedef unsigned long StorageType; };
template<> class isSimple<signed long long> { public: enum { value = true }; typedef signed long long StorageType; };
template<> class isSimple<unsigned long long> { public: enum { value = true }; typedef unsigned long long StorageType; };
template<> class isSimple<float> { public: enum { value = true }; typedef float StorageType; };
template<> class isSimple<double> { public: enum { value = true }; typedef double StorageType; };
template<> class isSimple<long double> { public: enum { value = true }; typedef long double StorageType; };

template<typename T>
class /*MS_EXPORT*/ List
{
#if defined(QT_DEBUG) //|| defined(QT_NO_DEBUG)
	friend inline QDebug operator<<(QDebug _stream, ::MarttaSupport::List<T> const& _me)
	{
		return _me.streamToDebug(_stream);
	}
#endif
	friend inline std::ostream& operator<<(std::ostream& _stream, ::MarttaSupport::List<T> const& _me)
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
	inline static T& star(ST& _d) { if (!isTrivial) return **(T**)&_d; else return *(T*)&_d; }
	inline static T const& star(ST const& _d) { if (!isTrivial) return **(T const**)&_d; else return *(T const*)&_d; }
	
	class Box
	{
		template<typename T2> friend class List;
	
	public:
		inline Box(): m_data(0), m_count(0), m_reserved(0) {}
		inline Box(List<T> const& _return): m_data(0), m_count(0), m_reserved(0) { operator=(_return); }
		inline Box(Box const& _move): m_data(0), m_count(0), m_reserved(0) { operator=(_move); }
		Box& operator=(List<T> const& _return);
		Box& operator=(Box const& _move);
		~Box();
	
	private:
		ST* m_data;
		int m_count;
		int m_reserved;
	};
	
	class /*MS_EXPORT*/ Iterator
	{
		friend class List;
		class ConstIterator;

	public:
		Iterator(): m_ptr(0) {}
		Iterator(Iterator const& _i): m_ptr(_i.m_ptr) {}
		inline T const& operator*() const { return star(*m_ptr); }
		inline T const* operator->() const { return &star(*m_ptr); }
		inline Iterator operator+(int _n) const { return Iterator(m_ptr + _n); }
		inline Iterator& operator++(int) { Iterator ret(*this); m_ptr++; return ret; }
		inline Iterator& operator++() { m_ptr++; return *this; }
		inline Iterator& operator+=(int _n) { m_ptr += _n; return *this; }
		inline int operator-(Iterator _i) const { return m_ptr - _i.m_ptr; }
		inline Iterator operator-(int _n) const { return Iterator(m_ptr - _n); }
		inline Iterator& operator--(int) { Iterator ret(*this); m_ptr--; return ret; }
		inline Iterator& operator--() { m_ptr--; return *this; }
		inline Iterator& operator-=(int _n) { m_ptr -= _n; return *this; }
		inline bool operator==(Iterator const& _i) { return m_ptr == _i.m_ptr; }
		inline bool operator==(ConstIterator const& _i);
		inline bool operator!=(Iterator const& _i) { return m_ptr != _i.m_ptr; }
		inline bool operator!=(ConstIterator const& _i);
		inline bool operator<(Iterator const& _other) const { return m_ptr < _other.m_ptr; }
		inline bool operator<(ConstIterator const& _other) const;
		inline bool operator<=(Iterator const& _other) const { return m_ptr <= _other.m_ptr; }
		inline bool operator<=(ConstIterator const& _other) const;
		inline bool operator>(Iterator const& _other) const { return m_ptr > _other.m_ptr; }
		inline bool operator>(ConstIterator const& _other) const;
		inline bool operator>=(Iterator const& _other) const { return m_ptr >= _other.m_ptr; }
		inline bool operator>=(ConstIterator const& _other) const;

	private:
		Iterator(ST* _p): m_ptr(_p) {}
		ST* m_ptr;
	};
	class /*MS_EXPORT*/ ConstIterator
	{
		friend class List;
		friend class Iterator;
		
	public:
		ConstIterator(): m_ptr(0) {}
		ConstIterator(ConstIterator const& _i): m_ptr(_i.m_ptr) {}
		inline T const& operator*() const { return star(*m_ptr); }
		inline T const* operator->() const { return &star(*m_ptr); }
		inline ConstIterator operator+(int _n) const { return ConstIterator(m_ptr + _n); }
		inline ConstIterator& operator++(int) { ConstIterator ret(*this); m_ptr++; return ret; }
		inline ConstIterator& operator++() { m_ptr++; return *this; }
		inline ConstIterator& operator+=(int _n) { m_ptr += _n; return *this; }
		inline int operator-(ConstIterator _i) const { return m_ptr - _i.m_ptr; }
		inline ConstIterator operator-(int _n) const { return ConstIterator(m_ptr - _n); }
		inline ConstIterator& operator--(int) { ConstIterator ret(*this); m_ptr--; return ret; }
		inline ConstIterator& operator--() { m_ptr--; return *this; }
		inline ConstIterator& operator-=(int _n) { m_ptr -= _n; return *this; }
		inline bool operator==(Iterator const& _i) { return m_ptr == _i.m_ptr; }
		inline bool operator==(ConstIterator const& _i) { return m_ptr == _i.m_ptr; }
		inline bool operator!=(Iterator const& _i) { return m_ptr != _i.m_ptr; }
		inline bool operator!=(ConstIterator const& _i) { return m_ptr != _i.m_ptr; }
		inline bool operator<(Iterator const& _other) const { return m_ptr < _other.m_ptr; }
		inline bool operator<(ConstIterator const& _other) const { return m_ptr < _other.m_ptr; }
		inline bool operator<=(Iterator const& _other) const { return m_ptr <= _other.m_ptr; }
		inline bool operator<=(ConstIterator const& _other) const { return m_ptr <= _other.m_ptr; }
		inline bool operator>(Iterator const& _other) const { return m_ptr > _other.m_ptr; }
		inline bool operator>(ConstIterator const& _other) const { return m_ptr > _other.m_ptr; }
		inline bool operator>=(Iterator const& _other) const { return m_ptr >= _other.m_ptr; }
		inline bool operator>=(ConstIterator const& _other) const { return m_ptr >= _other.m_ptr; }

	private:
		ConstIterator(ST* _p): m_ptr(_p) {}
		ST* m_ptr;
	};
	
	typedef Iterator iterator;
	typedef ConstIterator const_iterator;
	
	inline List(): m_data(0), m_count(0), m_reserved(0) {}
	List(List<T> const& _other);
	List(Box const& _returned): m_data(0), m_count(0), m_reserved(0) { operator=(_returned); }
	inline ~List() { clear(); }
	
	List<T>& operator=(Box const& _returned);
	
	void append(T const& _value);
	inline T const& at(int _i) const { ASSERT(_i >= 0 && _i < m_count); return star(m_data[_i]); }
	inline Iterator begin() { return Iterator(m_data); }
	inline ConstIterator begin() const { return ConstIterator(m_data); }
	void clear();
	inline ConstIterator constBegin() const { return ConstIterator(m_data); }
	inline ConstIterator constEnd() const { return ConstIterator(m_data + m_count); }
	bool contains(T const& _value) const;
	int count(T const& _value) const;
	inline int count() const { return m_count; }
	inline Iterator end() { return Iterator(m_data + m_count); }
	inline ConstIterator end() const { return ConstIterator(m_data + m_count); }
	Iterator erase(Iterator _pos);
	Iterator erase(Iterator _begin, Iterator _end);
	inline T& first() { ASSERT(m_count); return star(m_data[0]); }
	inline T const& first() const { ASSERT(m_count); return star(m_data[0]); }
	int indexOf(const T& _value, int _from = 0) const;
	void insert(int _i, const T& _value);
	inline Iterator insert(Iterator _before, const T& _value);
	inline bool isEmpty() const { return !m_count; }
	inline T& last() { ASSERT(m_count); return star(m_data[m_count - 1]); }
	inline T const& last() const { ASSERT(m_count); return star(m_data[m_count - 1]); }
	int lastIndexOf(const T& _value, int _from = -1) const;
	List<T> mid(int _pos, int _length = -1) const;
	void move(int _from, int _to);
	void prepend(const T& _value);
	int removeAll(const T& _value);
	inline void removeAt(int _i) { ASSERT(_i >= 0 && _i < m_count); erase(Iterator(m_data + _i)); }
	void removeFirst() { ASSERT(m_count); erase(Iterator(m_data)); }
	void removeLast() { ASSERT(m_count); erase(Iterator(m_data + m_count - 1)); }
	bool removeOne(T const& _value);
	inline void replace(int _i, T const& _value) { ASSERT(_i >= 0 && _i < m_count); star(m_data[_i]) = _value; }
	void reserve(int _n);
	inline List<int> reversed() const { return List<int>(*this).reverse(); }
	List<int>& reverse();
	inline int size() const { return count(); }
	inline void swap(int _i, int _j);
	inline T takeAt(int _i);
	inline T takeFirst() { ASSERT(m_count); return takeAt(0); }
	inline T takeLast() { ASSERT(m_count); return takeAt(m_count - 1); }
	inline T value(int _i) const { if(_i >= 0 && _i < m_count) return star(m_data[_i]); else return T(); }
	inline T value(int _i, T const& _defaultValue) const { if(_i >= 0 && _i < m_count) return star(m_data[_i]); else return _defaultValue; }
	inline bool operator!=(List<T> const& _other) const { return !operator==(_other); }
	inline List<T> operator+(const List<T>& _other) const { return List<T>(*this) += _other; }
	List<T>& operator+=(List<T> const& _other);
	inline List<T>& operator+=(T const& _value) { append(_value); return *this; }
	inline List<T>& operator<<(List<T> const& _other) { return operator+=(_other); }
	inline List<T>& operator<<(T const& _value) { return operator+=(_value); }
	inline List<T>& operator=(List<T> const& _other);
	bool operator==(List<T> const& _other) const;
	inline T& operator[](int _i) { ASSERT(_i >= 0 && _i < m_count); return star(m_data[_i]); }
	inline T const& operator[](int _i) const { return at(_i); }
	
private:
	/// Makes the list a bit bigger.
	void grow(int _minimum = 0);
	
#if defined(QT_DEBUG) //|| defined(QT_NO_DEBUG)
	QDebug streamToDebug(QDebug _stream) const;
#endif
	std::ostream& streamToDebug(std::ostream& _stream) const;
	
	ST* m_data;
	int m_count;
	int m_reserved;
};

}

#include "msList.inl"
