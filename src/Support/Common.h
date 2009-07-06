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

#pragma once

#include <cmath>

#include <msSupport.h>
#include <msDebug.h>
#include <msString.h>
#include <msStringList.h>
#include <msHash.h>
#include <msList.h>
using namespace MarttaSupport;

#if 0&&!defined(QT_DEBUG) && !defined(QT_NO_DEBUG)
typedef unsigned int uint;
#endif

namespace Martta
{

#if _MSC_VER
template<class T> inline T round(T const& x) { return ((x)>=0?(long)((x)+0.5):(long)((x)-0.5)); }
#endif

inline String times(int _s, int _omte, String const& _btwn)
{
	String r;
	if (_omte <= _s)
		return r;
	r = String::number(_s);
	for (int i = _s + 1; i < _omte; i++)
		r += _btwn + String::number(i);
	return r;
}

inline String camelCase(String const& _t)
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
			ret += t[i];
	return ret;
}

template<class T> List<T> solve(List<T> const& _q, MultiHash<T, T> const& _deps)
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

template<class T>
class NameTrait
{
public:
	static String name(T _val) { return _val ? _val->name() : String(); }
};

template<class S> inline List<S> nameStarts(List<S> const& _l, String const& _s)
{
	List<S> ret;
	foreach (S i, _l)
		if (NameTrait<S>::name(i).toLower().startsWith(_s.toLower()))
			ret << i;
	return ret;
}

/* 
// TODO: Move to MS.
template<class E>
class Flags
{
public:
	Flags(): m_value(0) {}
	Flags(E _v): m_value(_v) {}
	
	inline Flags& set(E _f, bool _v = true) { m_value = (test(_f) != _v) ? m_value ^ _f : m_value; return *this; }
	inline bool test(E _f) const { return m_value & _f; }
	
	Qualifiers operator|(Qualifier _q) const { return m_value
	
private:
	Flags(int _f) {
	
	int m_value;
};
#define MS_DECLARE_FLAGS(Flag) \
typedef Flags<Flag> Flag ## s; \
Flag ## s operator|(Flag _a, Flag _b) { return Flag ## s(_a|_b); }
*/


// -> Various Entity class's support libs.

enum Access
{
	Public = 0,
	Protected,
	Private,
	AccessCount,
	NoAccess
};

typedef double Precedence;
extern const Precedence NoPrecedence;

enum Associativity
{
	NoAssociativity = 0,
	LeftAssociativity,
	RightAssociativity
};

enum Qualifier
{
	NoQualifiers = 0,
 	Static = 0x0002,	// Variable/Function
	Extern = 0x0004,	// Variable/Function
  	Mutable = 0x0010,	// Variable
	Volatile = 0x0020,	// Variable
 	Virtual = 0x0100,	// Method
	Restrict = 0x0200,	// Function
	Inline = 0x0400,	// Function
 	Explicit = 0x0800,	// Constructor
	VariableMask = Static|Extern|Mutable|Volatile,
	FunctionMask = Static|Extern|Restrict|Inline|Explicit,
	MethodMask = FunctionMask|Virtual,
 	QualifierMask = Static|Extern|Mutable|Volatile|Virtual|Restrict|Inline|Explicit
};
typedef int Qualifiers;
inline void setFlag(Qualifiers& _qs, Qualifier _f, bool _v = true) { _qs = ((_qs&_f) != _v) ? _qs ^ _f : _qs; }

inline char const* code(Access _i)
{
	switch (_i)
	{
		case Public: return "public";
		case Protected: return "protected";
		case Private: return "private";
		default: return "";
	}
}

inline char const* code(Qualifier _q)
{
	switch (_q)
	{
		case Static: return "static";
		case Extern: return "extern";
		case Mutable: return "mutable";
		case Volatile: return "volatile";
		case Virtual: return "virtual";
		case Restrict: return "restrict";
		case Inline: return "inline";
		case Explicit: return "explicit";
		default: return "";
	}
}

enum WhitespacePosition
{
	NoWhitespace = 0,
	AtStart,
	AtEnd
};

String code(Qualifiers _q, WhitespacePosition _p = AtEnd);



// -> Entity's support.

typedef Hash<String, String> ViewKeys;

template<class T, class F> inline List<T*> castEntities(List<F*> _f)
{
	List<T*> ret;
	foreach (F* i, _f)
		ret << static_cast<T*>(i);
	return ret;
}

template<class T, class F> inline List<T*> filterEntities(List<F*> _f)
{
	List<T*> ret;
	foreach (F* i, _f)
		if (i->template isKind<T>())
			ret << i->template asKind<T>();
	return ret;
}

}
