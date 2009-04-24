/**
 * @filename msString.h
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

#include "msSupport.h"
#include "msList.h"

namespace MarttaSupport
{


class String;
typedef List<String> StringList;

class /*MS_EXPORT*/ String
{
public:
	class Box;
	friend class Box;
	
	class Box
	{
		friend class String;
		
	public:
		inline Box(): m_data(0), m_length(0), m_allocated(0), m_cache(0) {}
		inline Box(String const& _return): m_data(0) { operator=(_return); }
		inline Box(Box const& _return): m_data(0) { operator=(_return); }
		inline Box& operator=(String const& _return) { delete [] m_data; m_data = _return.m_data; m_length = _return.m_length; m_allocated = _return.m_allocated; m_cache = _return.m_cache; String& s = const_cast<String&>(_return); s.m_data = 0; s.m_allocated = s.m_length = 0; s.m_cache = 0; return *this; }
		inline Box& operator=(Box const& _return) { delete [] m_data; m_data = _return.m_data; m_length = _return.m_length; m_allocated = _return.m_allocated; m_cache = _return.m_cache; Box& s = const_cast<Box&>(_return); s.m_data = 0; s.m_allocated = s.m_length = 0; s.m_cache = 0; return *this; }
		
	private:
		wchar_t*				m_data;
		t::uint					m_length;
		t::uint					m_allocated;
		mutable char*			m_cache;
	};
	
	inline String(): m_data(0), m_length(0), m_allocated(0), m_cache(0) {}
	inline String(String const& _other): m_data(0), m_length(0), m_allocated(0), m_cache(0) { operator=(_other); }
	inline String(char const* _latin1): m_data(0), m_length(0), m_allocated(0), m_cache(0) { operator=(_latin1); }
	inline String(int _size, wchar_t _ch): m_data(0), m_length(0), m_allocated(0), m_cache(0) { fill(_ch, _size); }
	inline ~String() { delete [] m_data; }

	inline String(Box const& _return): m_data(0) { operator=(_return); }
	inline String&			operator=(Box const& _return) { delete [] m_data; m_data = _return.m_data; m_length = _return.m_length; m_allocated = _return.m_allocated; m_cache = _return.m_cache; Box& s = const_cast<Box&>(_return); s.m_data = 0; s.m_allocated = s.m_length = 0; s.m_cache = 0; return *this; }
	
	String&					operator=(String const& _other);
	String&					operator=(char const* _latin1);
	inline String&			operator=(char _ch) { reserve(1); m_data[0] = _ch; m_data[1] = 0; m_length = 1; return *this; }
	inline String&			operator=(wchar_t _ch) { reserve(1); m_data[0] = _ch; m_data[1] = 0; m_length = 1; return *this; }
	inline					operator wchar_t const*() const { return m_data; }
	inline					operator char const*() const { return toCString(); }
	inline wchar_t&			operator[](int _i) { changed(); return m_data[_i]; }
	inline wchar_t			operator[](int _i) const { return value(_i); }
	
	bool					operator==(String const& _other) const;
	inline bool				operator!=(String const& _other) const { return !operator==(_other); }
	inline bool				operator==(char const* _latin1) const { return operator==(String(_latin1)); }
	inline bool				operator!=(char const* _latin1) const { return !operator==(_latin1); }
	bool					operator<(String const& _other) const;
	inline bool				operator<(char const* _other) const { return operator<(String(_other)); }
	bool					operator<=(String const& _other) const;
	inline bool				operator<=(char const* _other) const { return operator<=(String(_other)); }
	bool					operator>(String const& _other) const;
	inline bool				operator>(char const* _other) const { return operator>(String(_other)); }
	bool					operator>=(String const& _other) const;
	inline bool				operator>=(char const* _other) const { return operator>=(String(_other)); }
	inline String&			operator+=(String const& _other) { append(_other); return *this; }
	inline String&			operator+=(char const* _latin1) { append(_latin1); return *this; }
	inline String&			operator+=(char _ch) { append(_ch); return *this; }
	inline String&			operator+=(wchar_t _ch) { append(_ch); return *this; }

	String&					append(String const& _str);
	String&					append(char const* _str);
	String&					append(wchar_t _ch);
	String&					append(char _ch);
	
	inline wchar_t			at(t::uint _i) const { ASSERT(_i < m_length); return m_data[_i]; }
	inline wchar_t			value(t::uint _i, wchar_t _default = 0) const { if (_i < m_length) return m_data[_i]; else return _default; }
	inline char const*		toCString() const { checkCache(); return m_cache; }
	inline wchar_t*			data() { changed(); return m_data; }
	inline wchar_t const*	data() const { return m_data; }
	inline wchar_t const*	constData() const { return m_data; }
	
	void					reserve(t::uint _len);
	inline void				clear() { delete [] m_data; m_data = 0; m_length = m_allocated = 0; changed(); }
	inline void				chop(t::uint _i) { m_length -= min(m_length, _i); m_data[m_length] = 0; changed(); }
	inline void				truncate(t::uint _i) { m_length = min(m_length, _i); m_data[m_length] = 0; changed(); }
	
	inline bool				isEmpty() const { return !m_length; }
	inline int				length() const { return m_length; }
	inline int				size() const { return m_length; }
	inline void				resize(t::uint _nl) { reserve(_nl); m_length = _nl; if (m_data) m_data[_nl] = 0; changed(); }
	
	String					trimmed() const;
	String					simplified() const;
	String					mid(t::uint _i, t::uint _length) const;
	inline String			mid(t::uint _i) const { ASSERT(_i >= 0); ASSERT(_i < m_length); return mid(_i, m_length - _i); }
	inline String			left(t::uint _len) const { ASSERT(_len <= m_length); return mid(0, _len); }
	inline String			right(t::uint _len) const { ASSERT(_len <= m_length); return mid(m_length - _len, _len); }

	bool					contains(String const& _str) const;
	bool					contains(wchar_t _ch) const;
	inline int				count() const { return size(); }
	int						count(String const& _str) const;
	int						count(wchar_t _ch) const;
	int						indexOf(String const& _str, int _from = 0) const;
	int						indexOf(wchar_t const* _str, int _from = 0) const;
	int						indexOf(wchar_t _ch, int _from = 0) const;
	int						lastIndexOf(String const& _str, int _from = -1) const;
	int						lastIndexOf(wchar_t _ch, int _from = -1) const;
	int						indexOfNth(String const& _str, t::uint _th) const;
	int						indexOfNth(wchar_t _ch, t::uint _th) const;
	int						lastIndexOfNth(String const& _str, t::uint _th) const;
	int						lastIndexOfNth(wchar_t _ch, t::uint _th) const;

	bool					endsWith(String const& _str) const;
	bool					endsWith(wchar_t _ch) const { return m_length && m_data[m_length - 1] == _ch; }
	bool					startsWith(String const& _str) const;
	bool					startsWith(wchar_t _ch) const { return m_length && m_data[0] == _ch; }

	String&					fill(wchar_t _ch, int _len = -1);

	StringList				split(String const& _sep) const;
	StringList				split(wchar_t _sep) const;
	String					section(String const& _sep, int _start, int _end = -1) const;
	String					section(wchar_t _sep, int _start, int _end = -1) const;

	String					toLower() const;
	String					toUpper() const;

	String&					replace(t::uint _position, t::uint _n, String const& _after);
	String&					replace(t::uint _position, t::uint _n, wchar_t _after);
	String&					replace(String const& _before, String const& _after);
	String&					replace(wchar_t _ch, String const& _after);
	String&					replace(wchar_t _before, wchar_t _after);
	inline String&			insert(t::uint _position, String const& _str) { return replace(_position, 0, _str); }
	inline String&			insert(t::uint _position, wchar_t _ch) { return replace(_position, 0, _ch); }
	inline String&			prepend(String const& _str) { return replace(0, 0, _str); }
	inline String&			prepend(const char* _str) { return replace(0, 0, String(_str)); }
	inline String&			prepend(wchar_t _ch) { return replace(0, 0, _ch); }
	inline String&			remove(int _position, int _n) { return replace(_position, _n, String()); }
	inline String&			remove(String const& _str) { return replace(_str, String()); }
	inline String&			remove(wchar_t _ch) { return replace(_ch, String()); }

	int						toInt(bool* _ok = 0, int _base = 10) const;
	t::uint					toUint(bool* _ok = 0, int _base = 10) const;
	double					toDouble(bool* _ok = 0) const;
	inline float			toFloat(bool* _ok = 0) const { return (float)toDouble(_ok); }

	static String			number(int _n);
	static String			number(t::uint _n, int _base = 10);
	static String			number(double _n, char _format = 'g', int _precision = 6);

	String					arg(String const& _a, int _fieldWidth = 0, wchar_t _fillChar = L' ') const;
	String					arg(int _a, int _fieldWidth = 0, wchar_t _fillChar = L' ') const { return arg(String::number(_a), _fieldWidth, _fillChar); }
	String					arg(t::uint _a, int _fieldWidth = 0, int _base = 10, wchar_t _fillChar = L' ') const { return arg(String::number(_a, _base), _fieldWidth, _fillChar); }
	String					arg(wchar_t _a, int _fieldWidth = 0, wchar_t _fillChar = L' ') const;
	String					arg(char _a, int _fieldWidth = 0, wchar_t _fillChar = L' ') const;
	String					arg(double _a, int _fieldWidth = 0, char _format = 'g', int _precision = -1, wchar_t _fillChar = L' ') const { return arg(String::number(_a, _format, _precision), _fieldWidth, _fillChar); }

private:
	privateinline void		changed() { delete [] m_cache; m_cache = 0; }
	void					checkCache() const;
	int						findNextPlaceholder(wchar_t const** _token) const;
		
	wchar_t*				m_data;
	t::uint					m_length;
	t::uint					m_allocated;
	
	mutable char*			m_cache;
};

inline t::uint hashOf(String const& _s)
{
	t::uint ret = 5381;
	wchar_t const* i = _s.constData() + _s.length();
	while (i-- != _s.constData())
		ret = ((ret << 5) + ret) ^ *i;
	return ret;
}

inline bool operator==(char const* _s1, String const& _s2) { return _s2.operator==(_s1); }
inline bool operator!=(char const* _s1, String const& _s2) { return _s2.operator!=(_s1); }
inline bool operator<(char const* _s1, String const& _s2) { return _s2.operator>(_s1); }
inline bool operator<=(char const* _s1, String const& _s2) { return _s2.operator>=(_s1); }
inline bool operator>(char const* _s1, String const& _s2) { return _s2.operator<(_s1); }
inline bool operator>=(char const* _s1, String const& _s2) { return _s2.operator<=(_s1); }

inline const String operator+(String const& _s1, String const& _s2) { return String(_s1) += _s2; }
inline const String operator+(String const& _s1, char const* _s2) { return operator+(_s1, String(_s2)); }
inline const String operator+(char const* _s1, String const& _s2) { return operator+(String(_s1), _s2); }
inline const String operator+(String const& _s, char _ch) { return String(_s) += _ch; }
inline const String operator+(char _ch, String const& _s) { return String(_s).prepend(_ch); }

}

#if defined(QT_DEBUG) || defined(QT_NO_DEBUG)
inline QDataStream& operator<<(QDataStream& _stream, MarttaSupport::String const& _string)
{
	_stream << _string.length();
	wchar_t const* d = _string.data() + _string.length();
	while (d-- != _string.data())
		if (sizeof(wchar_t) == 1)
			_stream << (quint8 const&)*d;
		else if (sizeof(wchar_t) == 2)
			_stream << (quint16 const&)*d;
		else if (sizeof(wchar_t) == 4)
			_stream << (quint32 const&)*d;
		else if (sizeof(wchar_t) == 8)
			_stream << (quint64 const&)*d;
	return _stream;
}

inline QDataStream& operator>>(QDataStream& _stream, MarttaSupport::String& _string)
{
	unsigned l;
	_stream >> l;
	_string.resize(l);
	wchar_t* d = _string.data() + _string.length();
	while (d-- != _string.data())
		if (sizeof(wchar_t) == 1)
			_stream >> (quint8&)*d;
		else if (sizeof(wchar_t) == 2)
			_stream >> (quint16&)*d;
		else if (sizeof(wchar_t) == 4)
			_stream >> (quint32&)*d;
		else if (sizeof(wchar_t) == 8)
			_stream >> (quint64&)*d;
	return _stream;
}

inline QDebug operator<<(QDebug _stream, MarttaSupport::String const& _string)
{
	return _stream << _string.toCString();
}

inline unsigned int qHash(MarttaSupport::String const& _s)
{
	return MarttaSupport::hashOf(_s);
}
#endif
