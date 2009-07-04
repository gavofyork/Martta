/**
 * @filename msList.inl
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

#include <cstring>

namespace MarttaSupport
{

template<typename T> List<T>::Box::~Box()
{
	if (!isTrivial)
	{
		ST* i = m_data + m_count;
		while (i != m_data)
			delete (T*)*--i;
	}
	delete [] m_data;
}

template<typename T> typename List<T>::Box& List<T>::Box::operator=(List<T> const& _return)
{
	this->~Box();
	m_data = _return.m_data;
	m_count = _return.m_count;
	m_reserved = _return.m_reserved;
	List<T>& r = const_cast<List<T>&>(_return);
	r.m_data = 0;
	r.m_count = r.m_reserved = 0;
	return *this;
}
	
template<typename T> typename List<T>::Box& List<T>::Box::operator=(Box const& _move)
{
	this->~Box();
	m_data = _move.m_data;
	m_count = _move.m_count;
	m_reserved = _move.m_reserved;
	Box& r = const_cast<Box&>(_move);
	r.m_data = 0;
	r.m_count = r.m_reserved = 0;
	return *this;
}

template<class T> List<T>::List(List<T> const& _other):
	m_data		(0),
	m_count		(0),
	m_reserved	(0)
{
	reserve(_other.m_count);
	m_count = _other.m_count;
	ST* d = m_data + m_count;
	ST* s = _other.m_data + _other.m_count;
	while (s != _other.m_data)
		if (!isTrivial)
			*(T**)--d = new T(**(T**)--s);
		else
			*--d = *--s;
}

template<class T> List<T>& List<T>::operator=(Box const& _returned)
{
	clear();
	m_data = _returned.m_data;
	m_count = _returned.m_count;
	m_reserved = _returned.m_reserved;
	Box& r = const_cast<Box&>(_returned);
	r.m_data = 0;
	r.m_count = r.m_reserved = 0;
	return *this;
}

template<class T> void List<T>::append(T const& _value)
{
	if (m_reserved == m_count)
		grow();
	if (!isTrivial)
		*(T**)(m_data + m_count) = new T(_value);
	else
		star(m_data[m_count]) = _value;
	m_count++;
}
	
template<class T> void List<T>::clear()
{
	if (!isTrivial)
	{
		ST* i = m_data + m_count;
		while (i != m_data)
			delete *(T**)--i;
	}
	delete [] m_data;
	m_data = 0;
	m_count = 0;
	m_reserved = 0;
}
	
template<class T> bool List<T>::contains(T const& _value) const
{
	ST* i = m_data + m_count;
	while (i != m_data)
		if (star(*--i) == _value)
			return true;
	return false;
}
	
template<class T> int List<T>::count(T const& _value) const
{
	int ret = 0;
	ST* i = m_data + m_count;
	while (i != m_data)
		if (star(*--i) == _value)
			ret++;
	return ret;
}

template<class T> List<T> List<T>::mid(int _pos, int _length) const
{
	_length = (_length < 0) ? m_count - _pos : _length;
	List<T> ret;
	ret.reserve(_length);
	ret.m_count = _length;
	ST* s = m_data + _pos;
	ST* d = ret.m_data;
	ST* se = m_data + _pos + _length;
	while (s != se)
		if (!isTrivial)
			*(T**)(d++) = new T(**((T**)s++));
		else
			*(d++) = *(s++);
	return ret;
}

template<class T> typename List<T>::Iterator List<T>::erase(typename List<T>::Iterator _pos)
{
	ASSERT(_pos.m_ptr >= m_data);
	ASSERT(_pos.m_ptr < m_data + m_count);
	if (!isTrivial)
		delete *(T**)_pos.m_ptr;
	m_count--;
	memmove(_pos.m_ptr, _pos.m_ptr + 1, (m_count - (_pos.m_ptr - m_data)) * sizeof(ST)); 
	return _pos;
}

template<class T> typename List<T>::Iterator List<T>::erase(typename List<T>::Iterator _begin, typename List<T>::Iterator _end) 
{
	ASSERT(_end.m_ptr > _begin.m_ptr);
	ASSERT(_begin.m_ptr >= m_data);
	ASSERT(_end.m_ptr < m_data + m_count);
	if (!isTrivial)
		for (ST* i = _begin.m_ptr; i != _end.m_ptr; i++)
			delete *(T**)i;
	memmove(_begin.m_ptr, _end.m_ptr, (m_count - (_end.m_ptr - m_data)) * sizeof(ST));
	m_count -= _end.m_ptr - _begin.m_ptr;
	return _begin;
}

template<class T> int List<T>::indexOf(const T& _value, int _from) const
{
	ST* e = m_data + m_count;
	ST* i = m_data + _from;
	for (; i < e; i++)
		if (star(*i) == _value)
			return i - m_data;
	return -1;
}

template<class T> void List<T>::insert(int _i, const T& _value)
{
	ASSERT(_i < m_count);
	if (m_reserved == m_count)
		grow();
	memmove(m_data + _i + 1, m_data + _i, (m_count - _i) * sizeof(ST)); 
	m_count++;
	if (!isTrivial)
		*(T**)(m_data + _i) = new T(_value);
	else
		star(m_data[_i]) = _value;
}

template<class T> inline typename List<T>::Iterator List<T>::insert(typename List<T>::Iterator _before, const T& _value)
{
	ASSERT(_before.m_ptr >= m_data);
	ASSERT(_before.m_ptr < m_data + m_count);
	int i = _before.m_ptr - m_data;
	insert(i, _value);
	return m_data + i;
}

template<class T> int List<T>::lastIndexOf(const T& _value, int _from) const
{
	ASSERT(_from >= -1 && _from < m_count);
	ST* i = m_data + (_from < 0 ? m_count : (_from + 1));
	while (i != m_data)
		if (star(*--i) == _value)
			return i - m_data;
	return -1;
}

template<class T> void List<T>::move(int _from, int _to)
{
	ASSERT(_from >= 0 && _from < m_count);
	ASSERT(_to >= 0 && _to < m_count);
	if (_from == _to)
		return;
	ST x = m_data[_from];
	if (_from < _to)
		memmove(m_data + _from, m_data + _from + 1, (_to - _from) * sizeof(ST));
	else
		memmove(m_data + _to + 1, m_data + _to, (_from - _to) * sizeof(ST));
	m_data[_to] = x;
}

template<class T> void List<T>::prepend(const T& _value)
{
	if (m_reserved == m_count)
		grow();
	memmove(m_data + 1, m_data, m_count * sizeof(ST));
	if (!isTrivial)
		*(T**)m_data = new T(_value);
	else
		star(m_data[0]) = _value;
	m_count++;
}

template<class T> int List<T>::removeAll(const T& _value)
{
	int ret = 0;
	ST* i = m_data + m_count;
	while (i != m_data)
		if (star(*--i) == _value)
		{
			i = erase(Iterator(i)).m_ptr;
			++ret;
		}
	return ret;
}

template<class T> bool List<T>::removeOne(T const& _value)
{
	ST* i = m_data + m_count;
	while (i != m_data)
		if (star(*--i) == _value)
		{
			erase(Iterator(i));
			return true;
		}
	return false;
}

template<class T> void List<T>::reserve(int _n)
{
	if (m_reserved >= _n)
		return;
	if (!_n)
	{
		clear();
		return;
	}
	ST* d = new ST[_n];
	memcpy(d, m_data, m_count * sizeof(ST));
	delete [] m_data;
	m_data = d;
	m_reserved = _n;
}

template<class T> List<int>& List<T>::reverse()
{
	ST* i = m_data - 1;
	ST* j = m_data + m_count;
	while (++i < --j)
		MarttaSupport::swap(*i, *j);
	return *this;
}

template<class T> inline void List<T>::swap(int _i, int _j)
{
	ASSERT(_i >= 0 && _i < m_count);
	ASSERT(_j >= 0 && _j < m_count);
	if (_i == _j)
		return;
	ST x = m_data[_i];
	m_data[_i] = m_data[_j];
	m_data[_j] = x;
}

template<class T> inline T List<T>::takeAt(int _i)
{
	ASSERT(_i >= 0 && _i < m_count);
	T ret = star(m_data[_i]);
	erase(Iterator(m_data + _i));
	return ret;
}

template<class T> List<T>& List<T>::operator=(List<T> const& _other)
{
	reserve(_other.m_count);
	if (isTrivial)
		memcpy(m_data, _other.m_data, _other.m_count * sizeof(ST));
	else
	{
		if (_other.m_count < m_count)
			for (ST* d  = m_data + m_count; d != m_data + _other.m_count;)
				delete *(T**)--d;
		else if (_other.m_count > m_count)
			for (ST* d  = m_data + _other.m_count, *s = _other.m_data + _other.m_count; d != m_data + m_count;)
				*(T**)--d = new T(star(*--s));
		ST* d = m_data + min(_other.m_count, m_count);
		ST* s = _other.m_data + min(_other.m_count, m_count);
		while (d != m_data)
			star(*--d) = star(*--s);
	}
	m_count = _other.m_count;
	return *this;
}

template<class T> List<T>& List<T>::operator+=(List<T> const& _other)
{
	if (m_reserved < m_count + _other.m_count)
		reserve(m_count + _other.m_count);
	m_count += _other.m_count;
	if (!isTrivial)
	{
		ST* d = m_data + m_count;
		ST* s = _other.m_data + _other.m_count;
		while (s != _other.m_data)
			*(T**)--d = new T(star(*--s));
	}
	else
		memcpy(m_data + m_count - _other.m_count, _other.m_data, _other.m_count * sizeof(ST));
	return *this;
}

template<class T> bool List<T>::operator==(List<T> const& _other) const
{
	if (m_count != _other.m_count)
		return false;
	ST* i = m_data + m_count;
	ST* j = _other.m_data + m_count;
	while (i != m_data)
		if (star(*--i) != star(*--j))
			return false;
	return true;
}

template<class T> void List<T>::grow(int _minimum)
{
	int r = m_reserved;
	if (r < 10)
		r += 2;
	else if (r < 2042)
		r = nextHigher(r * 2 - 1) - 12;
	else
		r += 2048;
	reserve(max(r, _minimum));
}

template<class T> inline bool List<T>::Iterator::operator==(ConstIterator const& _i)
{
	return m_ptr == _i.m_ptr;
}

template<class T> inline bool List<T>::Iterator::operator!=(ConstIterator const& _i)
{
	return m_ptr == _i.m_ptr;
}

template<class T> inline bool List<T>::Iterator::operator<(ConstIterator const& _other) const
{
	return m_ptr < _other.m_ptr;
}

template<class T> inline bool List<T>::Iterator::operator<=(ConstIterator const& _other) const
{
	return m_ptr <= _other.m_ptr;
}

template<class T> inline bool List<T>::Iterator::operator>(ConstIterator const& _other) const
{
	return m_ptr > _other.m_ptr;
}

template<class T> inline bool List<T>::Iterator::operator>=(ConstIterator const& _other) const
{
	return m_ptr >= _other.m_ptr;
}

#if defined(QT_DEBUG) //|| defined(QT_NO_DEBUG)
template<class T> QDebug List<T>::streamToDebug(QDebug _stream) const
{
	_stream << "{";
	bool first = true;
	ST* e = m_data + m_count;
	ST* i = m_data;
	for (; i < e; i++)
	{
		if (first)
			first = false;
		else
			_stream << ",";
		_stream << star(*i);
	}
	return _stream << "}";
}
#endif

template<class T> std::ostream& List<T>::streamToDebug(std::ostream& _stream) const
{
	_stream << "{";
	bool first = true;
	ST* e = m_data + m_count;
	ST* i = m_data;
	for (; i < e; i++)
	{
		if (first)
			first = false;
		else
			_stream << ",";
		_stream << star(*i);
	}
	return _stream << "}";
}

}
