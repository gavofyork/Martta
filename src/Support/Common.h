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

#include <QDebug>
#include <QStringList>
#include <QVariant>
#include <QTime>
#include <QColor>

void assertFailed(char const* _c);

#define M_ASSERT(T) if (!(T)) assertFailed(#T);
#define qInformation qDebug

#if _MSC_VER
template<class T> inline T round(T x) { return ((x)>=0?(long)((x)+0.5):(long)((x)-0.5)); }
#endif

namespace Martta
{

extern bool s_testing;
extern char const* s_asserted;

class Timer
{
public:
	Timer(QString const& _name): m_name(_name), m_time(QTime::currentTime()) {}
	~Timer() { qInformation() << m_name << ":" << m_time.elapsed() << "ms"; }
	QString m_name;
	QTime m_time;
};

#define TIME_START(i) s_timeTimers[i].start()
#define TIME_STOP(i) if (true) { s_timeTotals[i] += s_timeTimers[i].elapsed(); s_timeCount[i]++; } else void(0)
#define TIME_TOTAL(i) s_timeTotals[i]
#define TIME_COUNT(i) s_timeCount[i]
#define TIME_FUNCTION Timer __x(__FUNCTION__)
#define TIME_STATEMENT(N) for (int __i = 0; !__i;) for (Timer __x(#N); !__i; ++__i)

extern int s_timeTotals[16];
extern int s_timeCount[16];
extern QTime s_timeTimers[16];

enum Access
{
	Public = 0,
	Protected,
	Private,
	AccessCount,
	NoAccess
};

typedef QHash<QString, QVariant> const ViewKeys;

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
Q_DECLARE_FLAGS(Qualifiers, Qualifier)

inline QString code(Access _i)
{
	switch (_i)
	{
		case Public: return "public";
		case Protected: return "protected";
		case Private: return "private";
		default: return QString();
	}
}

inline QString code(Qualifier _q)
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
		default: return QString();
	}
}

enum WhitespacePosition
{
	NoWhitespace = 0,
	AtStart,
	AtEnd
};

QString code(Qualifiers _q, WhitespacePosition _p = AtEnd);

template<class T> inline void setFlag(QFlags<T>& _s, T _f, bool _v = true)
{
	_s = (_s.testFlag(_f) != _v) ? _s ^ _f : _s;
}

template<class T> inline QList<T> reversed(QList<T> const& _l)
{
	QList<T> ret;
	foreach (T i, _l) ret.prepend(i);
	return ret;
}

QColor colourByName(QString const& _n);

template<class T>
class NameTrait
{
public:
	static QString name(T _val) { return _val ? _val->name() : QString(); }
};

template<class S> inline QList<S> nameStarts(QList<S> const& _l, QString const& _s)
{
	QList<S> ret;
	foreach (S i, _l)
		if (NameTrait<S>::name(i).toLower().startsWith(_s.toLower()))
			ret << i;
	return ret;
}

inline QString times(int _s, int _omte, QString const& _btwn)
{
	QString r;
	if (_omte <= _s)
		return r;
	r = QString::number(_s);
	for (int i = _s + 1; i < _omte; i++)
		r += _btwn + QString::number(i);
	return r;
}

inline QString camelCase(QString const& _t)
{
	if (!_t.size())
		return _t;
	QString t = _t.simplified();
	QString ret;
	ret.reserve(t.size());
	for(int i = 0; i < t.size(); i++)
		if (t[i] == ' ')
			ret += t[++i].toUpper();
		else
			ret += t[i];
	return ret;
}

inline QStringList startsWith(QStringList const& _l, QString const& _s)
{
	QStringList ret;
	foreach (QString i, _l)
		if (i.toLower().startsWith(_s.toLower()))
			ret << i;
	return ret;
}

template<class T, class F> inline QList<T> qlist_cast(QList<F> _f)
{
	QList<T> ret;
	foreach (F i, _f)
		ret << static_cast<T>(i);
	return ret;
}

template<class T, class F> inline QList<T*> castEntities(QList<F*> _f)
{
	QList<T*> ret;
	foreach (F* i, _f)
		ret << static_cast<T*>(i);
	return ret;
}

template<class T, class F> inline QList<T*> filterEntities(QList<F*> _f)
{
	QList<T*> ret;
	foreach (F* i, _f)
		if (i->template isKind<T>())
			ret << i->template asKind<T>();
	return ret;
}

template<class T, class S> inline QMultiMap<S, T> inverted(QMultiMap<T, S> const& _m)
{
	QMultiMap<S, T> ret;
	for (QMapIterator<T, S> i(_m); i.hasNext();)
	{
		i.next();
		ret.insert(i.value(), i.key());
	}
	return ret;
}

template<class T> QList<T> solve(QList<T> const& _q, QMultiMap<T, T> const& _deps)
{
	QList<T> q = _q;
	QList<T> ret;
	QMultiMap<T, T> deps = _deps;
	QMultiMap<T, T> depees = inverted(_deps);
	while (!q.empty())
	{
		T n = q.takeLast();
		ret << n;
		foreach (T m, depees.values(n))	// go through all that depend on n
		{
			depees.remove(n, m);
			deps.remove(m, n);
			if (!deps.count(m))			// if they themselves don't depend on anything else, whack them in q.
				q << m;
		}
	}
	
	if (deps.size())
		return QList<T>();
	return ret;
}

}
