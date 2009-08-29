/* ***** BEGIN LICENSE BLOCK *****
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

#if __linux__
#include <cstddef>
#endif
#include <wchar.h>
#include <wctype.h>
#include <cstring>
#include <cerrno>

#include "msList.h"
#include "msString.h"
#include "msStringList.h"
#include "msDebug.h"

namespace MarttaSupport
{

String const String::null;

/*m_inline void memcpy(void* _d, void const* _s, int _c)
{
	char* d = (char*)_d + _c;
	char const* s = (char const*)_s + _c;
	while (d != _d)
		*(d--) = *(s--);
}

m_inline int strlen(char const* _s)
{
	int ret = 0;
	if (_s)
		while (*(_s++))
			++ret;
	return ret;
}*/

bool Char::operator==(char _ch)
{
	return (int)m_value == (int)btowc(_ch);
}

Char::Char(char _ch):
	m_value(btowc(_ch))
{
}

Char& Char::operator=(char _ch)
{
	m_value = btowc(_ch);
	return *this;
}

Char::operator char() const
{
	return wctob(m_value);
}

bool Char::isNumber() const
{
	return m_value != L'\x7f' && iswdigit(m_value);
}

bool Char::isLetter() const
{
	return m_value != L'\x7f' && iswalpha(m_value);
}

bool Char::isAlphaNumeric() const
{
	return m_value != L'\x7f' && iswalnum(m_value);
}

bool Char::isSpace() const
{
	return iswspace(m_value);
}

bool Char::isGraph() const
{
	return m_value != L'\x7f' && iswgraph(m_value);
}

bool Char::isLower() const
{
	return m_value != L'\x7f' && iswlower(m_value);
}

bool Char::isUpper() const
{
	return m_value != L'\x7f' && iswupper(m_value);
}

Char Char::toUpper() const
{
	return (wchar_t)towupper(m_value);
}

Char Char::toLower() const
{
	return (wchar_t)towlower(m_value);
}

String& String::operator=(char const* _latin1)
{
	uint l = _latin1 ? std::strlen(_latin1) : 0;
	reserve(l);
	m_length = l;
	if (m_data)
	{
		wchar_t* d = m_data;
		if (_latin1)
			while (*_latin1)
				*(d++) = btowc(*(_latin1++));
		*d = 0;
	}
	changed();
	return *this;
}

String& String::operator=(wchar_t const* _unicode)
{
	sizeto(_unicode ? wcslen(_unicode) : 0);
	if (m_data)
		memcpy(m_data, _unicode, m_length * sizeof(wchar_t));
	changed();
	return *this;
}

String& String::operator=(String const& _other)
{
	sizeto(_other.m_length);
	if (m_data)
		std::memcpy(m_data, _other.m_data, _other.m_length * sizeof(wchar_t));
	changed();
	return *this;
}

bool String::boolCompare(String const& _other) const
{
	return m_length == _other.m_length && !memcmp(m_data, _other.m_data, m_length * sizeof(wchar_t));
}

bool String::boolCompare(wchar_t const* _other) const
{
	return m_length == wcslen(_other) && !memcmp(m_data, _other, m_length * sizeof(wchar_t));
}

int String::compare(wchar_t const* _other) const
{
	if (!_other && !m_data)
		return 0;
	else if (!m_data)
		return wcscmp(L"", _other);
	else if (!_other)
		return wcscmp(m_data, L"");
	else
		return wcscmp(m_data, _other);
}

int String::toInt(bool* _ok, int _base) const
{
	errno = 0;
	int ret = m_data ? wcstol(m_data, 0, _base) : 0;
	if (_ok)
		*_ok = !errno && m_data;
	return ret;
}

uint String::toUint(bool* _ok, int _base) const
{
	errno = 0;
	uint ret = m_data ? wcstoul(m_data, 0, _base) : 0u;
	if (_ok)
		*_ok = !errno && m_data;
	return ret;
}

double String::toDouble(bool* _ok) const
{
	errno = 0;
	double ret = m_data ? wcstod(m_data, 0) : 0.;
	if (_ok)
		*_ok = !errno && m_data;
	return ret;
}

bool String::endsWith(String const& _str) const
{
	if (m_length < _str.m_length)
		return false;
	return !memcmp(_str.m_data, m_data + m_length - _str.m_length, _str.m_length * sizeof(wchar_t));
}

bool String::startsWith(String const& _str) const
{
	if (m_length < _str.m_length)
		return false;
	return !memcmp(_str.m_data, m_data, _str.m_length * sizeof(wchar_t));
}

String& String::fill(Char _ch, int _len)
{
	if (_len >= 0)
		sizeto(_len);
	if (m_length)
		wmemset(m_data, _ch, m_length);
	changed();
	return *this;
}

int String::count(String const& _str) const
{
	if (m_length < _str.m_length || !_str.m_length)
		return 0;
	int ret = 0;
	wchar_t* i = m_data + m_length - _str.m_length + 1;
	while (i != m_data)
		if (!memcmp(--i, _str.m_data, _str.m_length * sizeof(wchar_t)))
			ret++;
	return ret;
}

int String::count(Char _ch) const
{
	int ret = 0;
	wchar_t* i = m_data + m_length;
	while (i != m_data)
		if (*--i == _ch)
			ret++;
	return ret;
}

bool String::contains(String const& _str) const
{
	if (m_length < _str.m_length || !_str.m_length)
		return false;
	wchar_t* i = m_data + m_length - _str.m_length + 1;
	while (i != m_data)
		if (!memcmp(--i, _str.m_data, _str.m_length * sizeof(wchar_t)))
			return true;
	return false;
}

bool String::contains(Char _ch) const
{
	wchar_t* i = m_data + m_length;
	while (i != m_data)
		if (*--i == _ch)
			return true;
	return false;
}

int String::lastIndexOf(String const& _str, int _from) const
{
	if (m_length < _str.m_length || !_str.m_length)
		return -1;
	wchar_t* i;
	if (_from < 0)
		if (_from < -(int)(m_length - _str.m_length + 1))
			return -1;
		else
			i = m_data + m_length - _str.m_length + 2 + _from;
	else
		i = m_data + min<int>(m_length - _str.m_length, _from) + 1;
	while (i != m_data)
		if (!memcmp(--i, _str.m_data, _str.m_length * sizeof(wchar_t)))
			return i - m_data;
	return -1;
}

int String::lastIndexOf(Char _ch, int _from) const
{
	wchar_t* i;
	if (_from < 0)
		if (_from < -(int)(m_length))
			return -1;
		else
			i = m_data + m_length + 1 + _from;
	else
		i = m_data + min<int>(m_length, _from + 1);
	while (i != m_data)
		if (*--i == _ch)
			return i - m_data;
	return -1;
}

int String::indexOf(String const& _str, int _from) const
{
	if (m_length < _str.m_length || !_str.m_length)
		return -1;
	wchar_t* i;
	if (_from < 0)
		if (_from < -(int)(m_length - _str.m_length + 1))
			i = m_data;
		else
			i = m_data + m_length - _str.m_length + 1 + _from;
	else
		if (_from > (int)m_length - (int)_str.m_length)
			return -1;
		else
			i = m_data + _from;
	wchar_t* en = m_data + m_length - _str.m_length + 1;
	while (i != en)
		if (!memcmp(i++, _str.m_data, _str.m_length * sizeof(wchar_t)))
			return i - m_data - 1;
	return -1;
}

int String::indexOf(wchar_t const* _str, int _from) const
{
	uint sl = wcslen(_str);
	if (m_length < sl || !sl)
		return -1;
	wchar_t* i;
	if (_from < 0)
		if (_from < -(int)(m_length - sl + 1))
			i = m_data;
		else
			i = m_data + m_length - sl + 1 + _from;
	else
		if (_from > (int)m_length - (int)sl)
			return -1;
		else
			i = m_data + _from;
	wchar_t* en = m_data + m_length - sl + 1;
	while (i != en)
		if (!memcmp(i++, _str, sl * sizeof(wchar_t)))
			return i - m_data - 1;
	return -1;
}

int String::indexOf(Char _ch, int _from) const
{
	wchar_t* i;
	if (_from < 0)
		if (_from < -(int)(m_length))
			return -1;
		else
			i = m_data + m_length + _from;
	else
		i = m_data + min<int>(m_length, _from);
	wchar_t* en = m_data + m_length;
	while (i != en)
		if (*(i++) == _ch)
			return i - m_data - 1;
	return -1;
}

StringList String::split(String const& _sep) const
{
	StringList ret;
	if (!m_length)
		return ret;
	int last = 0;
	uint endIndex = m_length - _sep.m_length + 1;
	for (uint i = 0; i < endIndex;)
		if (!memcmp(m_data + i, _sep.m_data, _sep.m_length * sizeof(wchar_t)))
		{
			ret << mid(last, i - last);
			i += _sep.m_length;
			last = i;
		}
		else i++;
	ret << mid(last, m_length - last);
	return ret;
}

StringList String::split(Char _sep) const
{
	StringList ret;
	if (!m_length)
		return ret;
	int last = 0;
	for (uint i = 0; i < m_length; i++)
		if (m_data[i] == _sep)
		{
			ret << mid(last, i - last);
			last = i + 1;
		}
	ret << mid(last, m_length - last);
	return ret;
}

int String::indexOfNth(String const& _str, uint _th) const
{
	if (_th == 0)
		return -1;
	uint c = 0;
	for (uint i = 0; i < m_length && c < _th;)
		if (!memcmp(m_data + i, _str.m_data, _str.m_length * sizeof(wchar_t)))
			if (++c == _th)
				return i;
			else
				i += _str.m_length;
		else
			i++;
	if (c == _th - 1)
		return m_length;
	return -1;
}

int String::indexOfNth(Char _ch, uint _th) const
{
	// for *this == _ch:
	// _th == 0 -> -1
	// _th == 1 -> 0
	// _th == 2 -> 1
	// _th >= 3 -> -1
	if (_th == 0)
		return -1;
	uint c = 0;
	for (uint from = 0; from < m_length && c < _th; from++)
		if (m_data[from] == _ch)
			if (++c == _th)
				return from;
	if (c == _th - 1)
		return m_length;
	return -1;
}

int String::lastIndexOfNth(String const& _str, uint _th) const
{
	if (_th == 0)
		return m_length;
	uint c = 0;
	for (int i = m_length - _str.m_length; i >= 0 && c < _th;)
		if (!memcmp(m_data + i, _str.m_data, _str.m_length * sizeof(wchar_t)))
			if (++c == _th)
				return i;
			else
				i -= _str.m_length;
		else
			i--;
	return -1;
}

int String::lastIndexOfNth(Char _ch, uint _th) const
{
	// for *this == _ch:
	// _th == 0 -> 1
	// _th == 1 -> 0
	// _th >= 2 -> -1
	if (_th == 0)
		return m_length;
	uint c = 0;
	for (int from = m_length - 1; from >= 0 && c < _th; from--)
		if (m_data[from] == _ch)
			if (++c == _th)
				return from;
	return -1;
}

String String::toLower() const
{
	String ret;
	ret.sizeto(m_length);
	wchar_t* s = m_data + m_length;
	wchar_t* d = ret.m_data + m_length;
	while (s != m_data)
		*--d = towlower(*--s);
	ret.changed();
	return ret;
}

String String::toUpper() const
{
	String ret;
	ret.sizeto(m_length);
	wchar_t* s = m_data + m_length;
	wchar_t* d = ret.m_data + m_length;
	while (s != m_data)
		*--d = towupper(*--s);
	ret.changed();
	return ret;
}

String& String::replace(uint _position, uint _n, String const& _after)
{
	AssertNR(_position < m_length);
	AssertNR(_position + _n <= m_length);
	if (_n == _after.m_length)
		memcpy(m_data + _position, _after.m_data, _n * sizeof(wchar_t));
	else if (m_length - _n + _after.m_length)
	{
		wchar_t* nd = new wchar_t[m_length - _n + _after.m_length + 1];
		memcpy(nd, m_data, _position * sizeof(wchar_t));
		memcpy(nd + _position, _after.m_data, _after.m_length * sizeof(wchar_t));
		memcpy(nd + _position + _after.m_length, m_data + _position + _n, (m_length - _position - _n) * sizeof(wchar_t));
		delete [] m_data;
		m_data = nd;
		m_length += _after.m_length - _n;
		m_data[m_length] = 0;
		m_allocated = m_length + 1;
	}
	else
		sizeto(0);
	changed();
	return *this;
}

String& String::replace(uint _position, uint _n, Char _after)
{
	AssertNR(_position < m_length);
	AssertNR(_position + _n <= m_length);
	if (_n == 1)
		m_data[_position] = _after;
	else if (m_length - _n + 1)
	{
		wchar_t* nd = new wchar_t[m_length - _n + 2];
		memcpy(nd, m_data, _position * sizeof(wchar_t));
		nd[_position] = _after;
		memcpy(nd + _position + 1, m_data + _position + _n, (m_length - _position - _n) * sizeof(wchar_t));
		delete [] m_data;
		m_data = nd;
		m_length += 1 - _n;
		m_data[m_length] = 0;
		m_allocated = m_length + 1;
	}
	else
		sizeto(0);
	changed();
	return *this;
}

String& String::replace(String const& _before, String const& _after)
{
	int i = 0;
	while (i < (int)m_length && (i = indexOf(_before, i)) != -1)
	{
		replace(i, _before.m_length, _after);
		i += _after.m_length;
	}
	changed();
	return *this;
}

String& String::replace(Char _ch, String const& _after)
{
	int i = 0;
	while (i < (int)m_length && (i = indexOf(_ch, i)) != -1)
	{
		replace(i, 1, _after);
		i += _after.m_length;
	}
	changed();
	return *this;
}

String& String::replace(Char _before, Char _after)
{
	wchar_t* i = m_data + m_length;
	while (i != m_data)
		if (*--i == _before)
		{
			changed();
			*i = _after;
		}
	return *this;
}

String String::section(String const& _sep, int _start, int _end) const
{
	if (!m_length)
		return String::null;
	int f = ((_start < 0) ? lastIndexOfNth(_sep, -_start) : indexOfNth(_sep, _start));
	f += f == -1 ? 1 : _sep.length();
	int t = (_end < 0) ? lastIndexOfNth(_sep, -_end - 1) : indexOfNth(_sep, _end + 1);
	if (t == -1)
		t = m_length;
	return f < t ? mid(f, t - f) : String::null;
}

String String::section(Char _sep, int _start, int _end) const
{
	if (!m_length)
		return String::null;
	int f = ((_start < 0) ? lastIndexOfNth(_sep, -_start) : indexOfNth(_sep, _start)) + 1;
	int t = (_end < 0) ? lastIndexOfNth(_sep, -_end - 1) : indexOfNth(_sep, _end + 1);
	if (t == -1)
		t = m_length;
	return f < t ? mid(f, t - f) : String::null;
}

String& String::append(String const& _str)
{
	if (!_str.m_length)
		return *this;
	reserve(m_length + _str.m_length);
	memcpy(m_data + m_length, _str.m_data, _str.m_length * sizeof(wchar_t));
	m_length += _str.m_length;
	m_data[m_length] = 0;
	changed();
	return *this;
}

String& String::append(char const* _latin1)
{
	uint l = std::strlen(_latin1);
	if (!l)
		return *this;
	reserve(m_length + l);
	wchar_t* d = m_data + m_length;
	if (_latin1)
		while (*_latin1)
			*(d++) = btowc(*(_latin1++));
	*d = 0;
	m_length += l;
	changed();
	return *this;
}

String& String::append(Char _ch)
{
	sizeto(m_length + 1);
	m_data[m_length - 1] = _ch;
	changed();
	return *this;
}

String String::trimmed() const
{
	if (!m_length)
		return String::null;
	uint i;
	for (i = 0; i < m_length && iswspace(m_data[i]); i++) ;
	uint j;
	for (j = m_length - 1; j > i && iswspace(m_data[j]); j--) ;
	return mid(i, j - i + 1);
}

String String::simplified() const
{
	String src = trimmed();
	String ret;
	if (!src.m_length)
		return String::null;
	ret.reserve(src.length());
	wchar_t* d = ret.m_data;
	wchar_t* s = src.m_data;
	while (*s)
	{
		if (iswspace(*s))
		{
			*(d++) = L' ';
			do s++;
			while (*s && iswspace(*s));
		}
		else
			*(d++) = *(s++);
	}
	*d = 0;
	ret.m_length = d - ret.m_data;
	return ret;
}

String String::mid(uint _i, uint _length) const
{
	if (_i >= m_length)
		return null;
	if (_i + _length > m_length)
		_length = m_length - _i;
	String ret;
	ret.sizeto(_length);
	memcpy(ret.m_data, m_data + _i, _length * sizeof(wchar_t));
	ret.changed();
	return ret;
}

String String::number(long _n)
{
	static wchar_t s_format[] = L"%d";
	static wchar_t s_result[128];
	int c = swprintf(s_result, 128, s_format, _n);
	String ret;
	ret.sizeto(c);
	memcpy(ret.m_data, s_result, c * sizeof(wchar_t));
	ret.changed();
	return ret;
}

String String::number(unsigned long _n, int _base)
{
	static wchar_t s_format[] = L"%d";
	static wchar_t s_result[128];
	AssertNR(_base == 10 || _base == 8 || _base == 16);
	s_format[1] = (_base == 10) ? L'u' : (_base == 16) ? L'x' : L'o';
	int c = swprintf(s_result, 128, s_format, _n);
	String ret;
	ret.sizeto(c);
	memcpy(ret.m_data, s_result, c * sizeof(wchar_t));
	ret.changed();
	return ret;
}

String String::number(double _n, char _format, int _precision)
{
	static wchar_t s_format[] = L"%.*g";
	static wchar_t s_result[128];
	s_format[3] = _format;
	int c = swprintf(s_result, 128, s_format, _precision, _n);
	String ret;
	ret.sizeto(c);
	memcpy(ret.m_data, s_result, c * sizeof(wchar_t));
	ret.changed();
	return ret;
}

String String::arg(String const& _a, int _fieldWidth, Char _fillChar) const
{
	String ret(*this);
	wchar_t const* token;
	String const* a = &_a;
	String jic;
	if ((int)_a.m_length < abs(_fieldWidth))
	{
		if (_fieldWidth < 0)
			a = &((*new String(_a)) += String(-_fieldWidth - _a.m_length, _fillChar));
		else
			a = &((*new String(_fieldWidth - _a.m_length, _fillChar)) += _a);
	}
	for (int p = findNextPlaceholder(&token); p != -1; p = ret.indexOf(token, p + a->m_length))
		ret.replace(p, token[2] ? 3 : 2, *a);
	if (a != &_a)
		delete a;
	return ret;
}

String String::arg(Char _a, int _fieldWidth, Char _fillChar) const
{
	if (_fieldWidth > 1 || _fieldWidth < -1)
		return arg(String(1, _a), _fieldWidth, _fillChar);
	String ret(*this);
	wchar_t const* token;
	for (int p = findNextPlaceholder(&token); p != -1; p = ret.indexOf(token, p + 1))
		ret.replace(p, token[2] ? 3 : 2, _a);
	return ret;
}

int String::findNextPlaceholder(wchar_t const** _token) const
{
	// Only goes below 10 at the moment.
	// If making it go below 100, don't forget that %10 should never match for %1 (it did originally).
	static wchar_t s[3] = L"%0";
	for (uint i = 1; i < 10; i++)
	{
		s[1] = L'0' + i, s[2] = 0;
		int n;
		if ((n = indexOf(s)) != -1)
		{
			if (_token)
				*_token = s;
			return n;
		}
	}
	return -1;
}

void String::reserve(uint _len)
{
	if (m_allocated <= _len)
	{
		wchar_t* nd = new wchar_t[_len + 1];
		std::memcpy(nd, m_data, m_length * sizeof(wchar_t));
		delete [] m_data;
		m_data = nd;
		m_data[m_length] = 0;
		m_allocated = _len + 1;
	}
}

void String::checkCache() const
{
	if (!m_cache)
	{
		m_cache = new char[m_length + 1];
		wchar_t const* s = m_data;
		char* d = m_cache;
		if (s)
			while (*s)
				*(d++) = wctob(*(s++));
		*d = 0;
	}
}

String times(int _s, int _omte, String const& _btwn)
{
	String r;
	if (_omte <= _s)
		return r;
	r = String::number(_s);
	for (int i = _s + 1; i < _omte; i++)
		r += _btwn + String::number(i);
	return r;
}

String camelCase(String const& _t, bool _upperFirst)
{
	if (!_t.size())
		return _t;
	String t = _t.simplified();
	String ret;
	ret.reserve(t.size());
	for(int i = 0; i < t.size(); i++)
		if (t[i] == ' ')
			ret += String("%1").arg(t[++i]).toUpper()[0];
		else
			ret += _upperFirst && !i ? t[i].toUpper() : t[i].toLower();
	return ret;
}

}
