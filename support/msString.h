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

namespace MarttaSupport
{

class Char
{
public:
	m_inline Char(): m_value(0) {}
	Char(char _ch);
	m_inline Char(wchar_t _ch): m_value(_ch) {}
	m_inline Char(Char const& _ch): m_value(_ch.m_value) {}
	m_inline explicit Char(int _code): m_value(_code) {}

	Char& operator=(char _ch);
	m_inline Char& operator=(wchar_t _ch) { m_value = _ch; return *this; }
	m_inline Char& operator=(Char _ch) { m_value = _ch.m_value; return *this; }

	operator char() const;
	m_inline operator wchar_t() const { return m_value; }

	bool operator==(char _ch);
	m_inline bool operator!=(char _ch) { return !operator==(_ch); }
	m_inline bool operator==(wchar_t _ch) { return m_value == _ch; }
	m_inline bool operator!=(wchar_t _ch) { return m_value != _ch; }

	bool isAlphaNumeric() const;
	bool isNumber() const;
	bool isLetter() const;
	bool isSpace() const;
	bool isLower() const;
	bool isUpper() const;
	bool isGraph() const;

	Char toLower() const;
	Char toUpper() const;

	m_inline int toInt(bool* _ok = 0, uint _base = 10)
	{
		bool ok;
		_ok = _ok ? _ok : &ok;
		uint r = m_value - L'0';
		if (r < 10)
			ok = (r < _base);
		else if ((r = m_value - L'A') < 26u)
			ok = ((r += 10) < _base);
		else if ((r = m_value - L'a') < 26u)
			ok = ((r += 10) < _base);
		else
			ok = false;
		return ok ? r : -1;
	}
private:
	wchar_t m_value;
};

m_inline bool operator==(wchar_t _a, Char _b)
{
	return _b.operator==(_a);
}

m_inline bool operator!=(wchar_t _a, Char _b)
{
	return _b.operator!=(_a);
}

m_inline bool operator==(char _a, Char _b)
{
	return _b.operator==(_a);
}

m_inline bool operator!=(char _a, Char _b)
{
	return _b.operator!=(_a);
}

class M_EXPORT String
{
public:
	class Box;
	friend class Box;

	class Box
	{
		friend class String;

	public:
		m_inline Box(): m_data(0), m_length(0), m_allocated(0), m_cache(0) {}
		m_inline Box(String const& _return): m_data(0) { operator=(_return); }
		m_inline Box(Box const& _return): m_data(0) { operator=(_return); }
		m_inline Box& operator=(String const& _return) { delete [] m_data; m_data = _return.m_data; m_length = _return.m_length; m_allocated = _return.m_allocated; m_cache = _return.m_cache; String& s = const_cast<String&>(_return); s.m_data = 0; s.m_allocated = s.m_length = 0; s.m_cache = 0; return *this; }
		m_inline Box& operator=(Box const& _return) { delete [] m_data; m_data = _return.m_data; m_length = _return.m_length; m_allocated = _return.m_allocated; m_cache = _return.m_cache; Box& s = const_cast<Box&>(_return); s.m_data = 0; s.m_allocated = s.m_length = 0; s.m_cache = 0; return *this; }

	private:
		wchar_t*				m_data;
		uint					m_length;
		uint					m_allocated;
		mutable char*			m_cache;
	};

	m_inline String(): m_data(0), m_length(0), m_allocated(0), m_cache(0) {}
	m_inline String(Char _ch): m_data(0), m_length(0), m_allocated(0), m_cache(0) { fill(_ch, 1); }
	m_inline String(String const& _other): m_data(0), m_length(0), m_allocated(0), m_cache(0) { operator=(_other); }

//	m_inline String(wchar_t _ch): m_data(0), m_length(0), m_allocated(0), m_cache(0) { fill(_ch, 1); }
	m_inline String(char const* _latin1): m_data(0), m_length(0), m_allocated(0), m_cache(0) { operator=(_latin1); }
	m_inline String(wchar_t const* _unicode): m_data(0), m_length(0), m_allocated(0), m_cache(0) { operator=(_unicode); }
	m_inline String(int _size, wchar_t _ch): m_data(0), m_length(0), m_allocated(0), m_cache(0) { fill(_ch, _size); }
#if 0//defined(QT_DEBUG) || defined(QT_NO_DEBUG)
	m_inline String(QString const& _q): m_data(0), m_length(0), m_allocated(0), m_cache(0) { operator=(_q); }
#endif
	m_inline ~String() { delete [] m_data; }

	m_inline String(Box const& _return): m_data(0) { operator=(_return); }
	m_inline String&			operator=(Box const& _return) { delete [] m_data; m_data = _return.m_data; m_length = _return.m_length; m_allocated = _return.m_allocated; m_cache = _return.m_cache; Box& s = const_cast<Box&>(_return); s.m_data = 0; s.m_allocated = s.m_length = 0; s.m_cache = 0; return *this; }

	String&					operator=(String const& _other);
	m_inline String&			operator=(Char _ch) { reserve(1); m_data[0] = _ch; m_data[1] = 0; m_length = 1; return *this; }
	String&					operator=(char const* _latin1);
	String&					operator=(wchar_t const* _unicode);
#if 0//defined(QT_DEBUG) || defined(QT_NO_DEBUG)
	m_inline String&			operator=(QString const& _q) { sizeto(_q.length()); _q.toWCharArray(m_data); changed(); return *this; }
#endif
	m_inline					operator wchar_t const*() const { return m_data; }
	m_inline					operator char const*() const { return toCString(); }
#if 0//defined(QT_DEBUG) || defined(QT_NO_DEBUG)
	m_inline					operator QString() const { return toQString(); }
#endif
	m_inline Char&			operator[](int _i) { changed(); return *(Char*)(m_data + _i); }
	m_inline Char				operator[](int _i) const { return value(_i); }

	m_inline int				compare(String const& _other) const { return compare(_other.m_data); }
	int						compare(wchar_t const* _other) const;
	m_inline int				compare(char const* _other) const { return compare(String(_other)); }
	bool					boolCompare(String const& _other) const;
	bool					boolCompare(wchar_t const* _other) const;
	m_inline bool				boolCompare(char const* _other) const { return boolCompare(String(_other)); }

	m_inline bool				operator==(String const& _other) const { return boolCompare(_other); }
	m_inline bool				operator!=(String const& _other) const { return !boolCompare(_other); }
	m_inline bool				operator<(String const& _other) const { return compare(_other) < 0; }
	m_inline bool				operator<=(String const& _other) const { return compare(_other) <= 0; }
	m_inline bool				operator>(String const& _other) const { return compare(_other) > 0; }
	m_inline bool				operator>=(String const& _other) const { return compare(_other) >= 0; }
	m_inline bool				operator==(wchar_t const* _other) const { return boolCompare(_other); }
	m_inline bool				operator!=(wchar_t const* _other) const { return !boolCompare(_other); }
	m_inline bool				operator<(wchar_t const* _other) const { return compare(_other) < 0; }
	m_inline bool				operator<=(wchar_t const* _other) const { return compare(_other) <= 0; }
	m_inline bool				operator>(wchar_t const* _other) const { return compare(_other) > 0; }
	m_inline bool				operator>=(wchar_t const* _other) const { return compare(_other) >= 0; }
	m_inline bool				operator==(char const* _other) const { return boolCompare(_other); }
	m_inline bool				operator!=(char const* _other) const { return !boolCompare(_other); }
	m_inline bool				operator<(char const* _other) const { return compare(_other) < 0; }
	m_inline bool				operator<=(char const* _other) const { return compare(_other) <= 0; }
	m_inline bool				operator>(char const* _other) const { return compare(_other) > 0; }
	m_inline bool				operator>=(char const* _other) const { return compare(_other) >= 0; }

	m_inline String&			operator+=(String const& _other) { append(_other); return *this; }
	m_inline String&			operator+=(Char _ch) { append(_ch); return *this; }
	m_inline String&			operator+=(char const* _latin1) { append(_latin1); return *this; }

	String&					append(String const& _str);
	String&					append(char const* _str);
	String&					append(Char _ch);

	m_inline Char				at(uint _i) const { AssertNR(_i < m_length); return m_data[_i]; }
	m_inline Char				value(uint _i, Char _default = Char()) const { if (_i < m_length) return m_data[_i]; else return _default; }
	m_inline char const*		toCString() const { checkCache(); return m_cache; }
#if 0//defined(QT_DEBUG) || defined(QT_NO_DEBUG)
	m_inline QString			toQString() const { return QString::fromWCharArray(m_data, m_length); }
#endif
	m_inline wchar_t*			data() { return m_data; } // If you changed the data at the pointer returned, call dataChanged() with the pointer returned.
	m_inline void				dataChanged(wchar_t* _d) { Assert(_d == m_data, "Passed pointer that is not our own data pointer"); (void)(_d); changed(); }
	m_inline wchar_t const*	data() const { return m_data; }
	m_inline wchar_t const*	constData() const { return m_data; }

	void					reserve(uint _len);
	m_inline void				clear() { delete [] m_data; m_data = 0; m_length = m_allocated = 0; changed(); }
	m_inline void				chop(uint _i) { m_length -= min(m_length, _i); m_data[m_length] = 0; changed(); }
	m_inline void				truncate(uint _i) { m_length = min(m_length, _i); m_data[m_length] = 0; changed(); }

	m_inline bool				isEmpty() const { return !m_length; }
	m_inline int				length() const { return m_length; }
	m_inline int				size() const { return m_length; }
	m_inline void				resize(uint _nl) { sizeto(_nl); changed(); }

	String					trimmed() const;
	String					simplified() const;
	String					mid(uint _i, uint _length) const;
	m_inline String			mid(uint _i) const { return mid(_i, m_length - min(m_length, _i)); }
	m_inline String			left(uint _len) const { return mid(0, min(m_length, _len)); }
	m_inline String			right(uint _len) const { return mid(m_length - min(m_length, _len), min(m_length, _len)); }

	bool					contains(String const& _str) const;
	bool					contains(Char _ch) const;
	m_inline int				count() const { return size(); }
	int						count(String const& _str) const;
	int						count(Char _ch) const;
	int						indexOf(String const& _str, int _from = 0) const;
	int						indexOf(wchar_t const* _str, int _from = 0) const;
	int						indexOf(Char _ch, int _from = 0) const;
	int						lastIndexOf(String const& _str, int _from = -1) const;
	int						lastIndexOf(Char _ch, int _from = -1) const;
	int						indexOfNth(String const& _str, uint _th) const;
	int						indexOfNth(Char _ch, uint _th) const;
	int						lastIndexOfNth(String const& _str, uint _th) const;
	int						lastIndexOfNth(Char _ch, uint _th) const;

	bool					endsWith(String const& _str) const;
	bool					endsWith(Char _ch) const { return m_length && m_data[m_length - 1] == _ch; }
	bool					startsWith(String const& _str) const;
	bool					startsWith(Char _ch) const { return m_length && m_data[0] == _ch; }

	String&					fill(Char _ch, int _len = -1);

	StringList				split(String const& _sep) const;
	StringList				split(Char _sep) const;
	String					section(String const& _sep, int _start, int _end = -1) const;
	String					section(Char _sep, int _start, int _end = -1) const;

	String					toLower() const;
	String					toUpper() const;

	String&					replace(uint _position, uint _n, String const& _after);
	String&					replace(uint _position, uint _n, Char _after);
	String&					replace(String const& _before, String const& _after);
	String&					replace(Char _ch, String const& _after);
	String&					replace(Char _before, Char _after);
	m_inline String&			insert(uint _position, String const& _str) { return replace(_position, 0, _str); }
	m_inline String&			insert(uint _position, Char _ch) { return replace(_position, 0, _ch); }
	m_inline String&			prepend(String const& _str) { return replace(0, 0, _str); }
	m_inline String&			prepend(Char _ch) { return replace(0, 0, _ch); }
	m_inline String&			prepend(const char* _str) { return replace(0, 0, String(_str)); }
	m_inline String&			remove(int _position, int _n) { return replace(_position, _n, String()); }
	m_inline String&			remove(String const& _str) { return replace(_str, String()); }
	m_inline String&			remove(Char _ch) { return replace(_ch, String()); }

	bool					toBool(bool* _ok = 0) const { if (_ok) *_ok = true; if (operator==(L"true")) return true; else if (operator==(L"false")) return false; if (_ok) *_ok = false; return false; }
	int						toInt(bool* _ok = 0, int _base = 10) const;
	uint					toUint(bool* _ok = 0, int _base = 10) const;
	double					toDouble(bool* _ok = 0) const;
	m_inline float			toFloat(bool* _ok = 0) const { return (float)toDouble(_ok); }

	static String			number(bool _n) { return _n ? L"true" : L"false"; }
	static String			number(unsigned char _n, int _base = 10) { return number((unsigned long)_n, _base); }
	static String			number(short _n) { return number((long)_n); }
	static String			number(unsigned short _n, int _base = 10) { return number((unsigned long)_n, _base); }
	static String			number(int _n) { return number((long)_n); }
	static String			number(unsigned int _n, int _base = 10) { return number((unsigned long)_n, _base); }
	static String			number(long _n);
	static String			number(unsigned long _n, int _base = 10);
	static String			number(double _n, char _format = 'g', int _precision = 6);

	String					arg(String const& _a, int _fieldWidth = 0, Char _fillChar = L' ') const;
	m_inline String			arg(wchar_t const* _a, int _fieldWidth = 0, Char _fillChar = L' ') const { return arg(String(_a), _fieldWidth, _fillChar); }
	m_inline String			arg(char const* _a, int _fieldWidth = 0, Char _fillChar = L' ') const { return arg(String(_a), _fieldWidth, _fillChar); }
	m_inline String			arg(void* _a) const { return arg(number((uint)_a, 16)); }
	m_inline String			arg(bool _a) const { return arg(number(_a)); }
	m_inline String			arg(double _a, int _fieldWidth = 0, char _format = 'g', int _precision = -1, Char _fillChar = L' ') const { return arg(String::number(_a, _format, _precision), _fieldWidth, _fillChar); }
	m_inline String			arg(long _a, int _fieldWidth = 0, wchar_t _fillChar = L' ') const { return arg(String::number(_a), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(long _a, int _fieldWidth, char _fillChar) const { return arg(String::number(_a), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(unsigned long _a, int _fieldWidth = 0, int _base = 10, wchar_t _fillChar = L' ') const { return arg(String::number(_a, _base), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(unsigned long _a, int _fieldWidth, int _base, char _fillChar) const { return arg(String::number(_a, _base), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(int _a, int _fieldWidth = 0, wchar_t _fillChar = L' ') const { return arg(String::number(_a), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(int _a, int _fieldWidth, char _fillChar) const { return arg(String::number(_a), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(unsigned int _a, int _fieldWidth = 0, int _base = 10, wchar_t _fillChar = L' ') const { return arg(String::number(_a, _base), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(unsigned int _a, int _fieldWidth, int _base, char _fillChar) const { return arg(String::number(_a, _base), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(short _a, int _fieldWidth = 0, wchar_t _fillChar = L' ') const { return arg(String::number(_a), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(short _a, int _fieldWidth, char _fillChar) const { return arg(String::number(_a), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(unsigned short _a, int _fieldWidth = 0, int _base = 10, wchar_t _fillChar = L' ') const { return arg(String::number(_a, _base), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(unsigned short _a, int _fieldWidth, int _base, char _fillChar) const { return arg(String::number(_a, _base), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(unsigned char _a, int _fieldWidth = 0, int _base = 10, wchar_t _fillChar = L' ') const { return arg(String::number(_a, _base), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(unsigned char _a, int _fieldWidth, int _base, char _fillChar) const { return arg(String::number(_a, _base), _fieldWidth, Char(_fillChar)); }
	String					arg(Char _a, int _fieldWidth = 0, Char _fillChar = L' ') const;
	m_inline String			arg(Char _a, int _fieldWidth, wchar_t _fillChar) const { return arg(_a, _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(Char _a, int _fieldWidth, char _fillChar) const { return arg(_a, _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(char _a, int _fieldWidth = 0, wchar_t _fillChar = L' ') const { return arg(Char(_a), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(char _a, int _fieldWidth, char _fillChar) const { return arg(Char(_a), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(wchar_t _a, int _fieldWidth = 0, wchar_t _fillChar = L' ') const { return arg(Char(_a), _fieldWidth, Char(_fillChar)); }
	m_inline String			arg(wchar_t _a, int _fieldWidth, char _fillChar) const { return arg(Char(_a), _fieldWidth, Char(_fillChar)); }

	static String const		null;

private:
	m_privateinline void		sizeto(int _nl)	// DONT FORGET TO CALL CHANGED AFTER USING THIS!
	{
		reserve(_nl);
		m_length = _nl;
		if (m_data)
			m_data[_nl] = 0;
	}
	m_privateinline void		changed()
	{
		delete [] m_cache;
		m_cache = 0;
#if defined(DEBUG)
		checkCache();
#endif
	}
	void					checkCache() const;
	int						findNextPlaceholder(wchar_t const** _token) const;

	wchar_t*				m_data;
	uint					m_length;
	uint					m_allocated;

	mutable char*			m_cache;
};

m_inline uint hashOf(String const& _s)
{
	uint ret = 5381;
	wchar_t const* i = _s.constData() + _s.length();
	while (i-- != _s.constData())
		ret = ((ret << 5) + ret) ^ *i;
	return ret;
}

m_inline bool operator==(char const* _s1, String const& _s2) { return _s2.operator==(_s1); }
m_inline bool operator!=(char const* _s1, String const& _s2) { return _s2.operator!=(_s1); }
m_inline bool operator<(char const* _s1, String const& _s2) { return _s2.operator>(_s1); }
m_inline bool operator<=(char const* _s1, String const& _s2) { return _s2.operator>=(_s1); }
m_inline bool operator>(char const* _s1, String const& _s2) { return _s2.operator<(_s1); }
m_inline bool operator>=(char const* _s1, String const& _s2) { return _s2.operator<=(_s1); }

m_inline bool operator==(wchar_t const* _s1, String const& _s2) { return _s2.operator==(_s1); }
m_inline bool operator!=(wchar_t const* _s1, String const& _s2) { return _s2.operator!=(_s1); }
m_inline bool operator<(wchar_t const* _s1, String const& _s2) { return _s2.operator>(_s1); }
m_inline bool operator<=(wchar_t const* _s1, String const& _s2) { return _s2.operator>=(_s1); }
m_inline bool operator>(wchar_t const* _s1, String const& _s2) { return _s2.operator<(_s1); }
m_inline bool operator>=(wchar_t const* _s1, String const& _s2) { return _s2.operator<=(_s1); }

m_inline const String operator+(String const& _s1, String const& _s2) { return String(_s1) += _s2; }
m_inline const String operator+(String const& _s1, char const* _s2) { return operator+(_s1, String(_s2)); }
m_inline const String operator+(char const* _s1, String const& _s2) { return operator+(String(_s1), _s2); }
m_inline const String operator+(String const& _s, Char _ch) { return String(_s) += _ch; }
m_inline const String operator+(Char _ch, String const& _s) { return String(_s).prepend(_ch); }
m_inline const String operator+(String const& _s, char _ch) { return String(_s) += _ch; }
m_inline const String operator+(char _ch, String const& _s) { return String(_s).prepend(_ch); }
m_inline const String operator+(String const& _s, wchar_t _ch) { return String(_s) += _ch; }
m_inline const String operator+(wchar_t _ch, String const& _s) { return String(_s).prepend(_ch); }

String times(int _s, int _omte, String const& _btwn);
String camelCase(String const& _t, bool _upperFirst = false);

}

#if defined(QT_DEBUG) || defined(QT_NO_DEBUG)
m_inline unsigned int qHash(MarttaSupport::String const& _s)
{
	return MarttaSupport::hashOf(_s);
}
#endif
