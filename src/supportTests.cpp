/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include <QDebug>

#include "msList.h"
#include "msString.h"
#include "msHash.h"
#include "msSupport.h"
using namespace MarttaSupport;

#include "TestHelper.h"
using namespace Martta;

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Box returnMeBox(GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey> const& _me)
{
	return _me;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey> const returnMe(GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey> const& _me)
{
	return _me;
}

template<typename T>
typename List<T>::Box returnMeBox(List<T> const& _me)
{
	return _me;
}

template<typename T>
List<T> returnMe(List<T> const& _me)
{
	return _me;
}

String::Box returnMeBox(String const& _me) { return _me; }
String returnMe(String const& _me) { return _me; }

namespace MST
{
typedef unsigned char uchar;
class Rgb
{
public:
	inline Rgb(): m_value(0xff000000) {}
	inline Rgb(t::uint _v, uchar _a = 255): m_value((_v & 0xffffff) + (t::uint)(((t::uint)_a) << 24ul)) {}
	inline Rgb(uchar _r, uchar _g, uchar _b, uchar _a = 255): m_value(fromRgb(_r, _g, _b, _a)) {}
	inline Rgb(Rgb _colour, uchar _a): m_value(fromRgb(_colour.r(), _colour.g(), _colour.b(), _a)) {}
	inline explicit Rgb(String const& _name)
	{
		if (_name.startsWith(L'#') && _name.length() == 5)
			m_value = fromRgb(_name[1].toInt(0, 16), _name[2].toInt(0, 16), _name[3].toInt(0, 16), _name[4].toInt(0, 16)) * 0x11;
		else if (_name.startsWith(L'#') && _name.length() == 9)
			m_value = _name.mid(1, 6).toUint(0, 16) + _name.mid(7, 2).toUint(0, 16) << 24;
		else
			m_value = 0;
	}
	
	inline uchar r() const { return toRed(m_value); }
	inline uchar g() const { return toGreen(m_value); }
	inline uchar b() const { return toBlue(m_value); }
	inline uchar a() const { return toAlpha(m_value); }
	
	inline void setR(uchar _v) { m_value = fromRgb(_v, g(), b(), a()); }
	inline void setG(uchar _v) { m_value = fromRgb(r(), _v, b(), a()); }
	inline void setB(uchar _v) { m_value = fromRgb(r(), g(), _v, a()); }
	inline void setA(uchar _v) { m_value = fromRgb(r(), g(), b(), _v); }
	
	String name() const { return String(L"#%1").arg((t::uint)(((t::uint)m_value << 8ul) + a()), 8, 16, L'0'); }
	String name(uchar _alpha) const { return String(L"#%1").arg((t::uint)(((t::uint)m_value << 8ul) + (t::uint)_alpha), 8, 16, L'0'); }
	
	inline Rgb interpolated(int _percent, Rgb _c = black) const
	{
		return fromRgb(	(r() * (100 - _percent) + _c.r() * _percent) / 100,
						(g() * (100 - _percent) + _c.g() * _percent) / 100,
						(b() * (100 - _percent) + _c.b() * _percent) / 100,
						(a() * (100 - _percent) + _c.a() * _percent) / 100);
	}
	inline Rgb interpolated(int _percent, uchar _alpha, Rgb _c = black) const
	{
		return fromRgb(	(r() * (100 - _percent) + _c.r() * _percent) / 100,
						(g() * (100 - _percent) + _c.g() * _percent) / 100,
						(b() * (100 - _percent) + _c.b() * _percent) / 100, _alpha);
	}
	
	static const Rgb black;
	static const Rgb white;
	static const Rgb red;
	static const Rgb green;
	static const Rgb blue;
	static const Rgb yellow;
	static const Rgb magenta;
	static const Rgb cyan;

public:
	static inline t::uint fromRgb(uchar _r, uchar _g, uchar _b, uchar _a) { t::uint ret = _a; return (((((ret <<= 8) += _r) <<= 8) += _g) <<= 8) += _b; }
	static inline uchar toRed(t::uint _v) { return (_v >> 16) & 255; }
	static inline uchar toGreen(t::uint _v) { return (_v >> 8) & 255; }
	static inline uchar toBlue(t::uint _v) { return _v & 255; }
	static inline uchar toAlpha(t::uint _v) { return (_v >> 24) & 255; }
	
	t::uint m_value;
};
}

int supportTests()
{
	int failed = 0;
	
	TEST("Rgb")
	{
		FAILED_IF(MST::Rgb(0xff0000).name() != "#ff0000ff");
		FAILED_IF(MST::Rgb(255, 255, 0, 0).name() != "#ffff0000");
		MST::Rgb g(L"#0f0f");
		qDebug("%x", g.m_value);
		FAILED_IF(g.r() != 0x00);
		FAILED_IF(g.g() != 0xff);
		FAILED_IF(g.b() != 0x00);
		FAILED_IF(g.a() != 0xff);
		qDebug() << g.name();
		FAILED_IF(g.name() != "#00ff00ff");
		g = MST::Rgb(0x0, 0xff, 0x0, 0xff);
		FAILED_IF(g.r() != 0x00);
		FAILED_IF(g.g() != 0xff);
		FAILED_IF(g.b() != 0x00);
		FAILED_IF(g.a() != 0xff);
		qDebug() << g.name();
		FAILED_IF(g.name() != "#00ff00ff");
		g = MST::Rgb(0x44bb77, 0xff);
		FAILED_IF(g.r() != 0x44);
		FAILED_IF(g.g() != 0xbb);
		FAILED_IF(g.b() != 0x77);
		FAILED_IF(g.a() != 0xff);
		FAILED_IF(g.name() != "#44bb77ff");
	}
	
	List<int> l;
	TEST("List append(), iterators")
	{
		for (int i = 0; i < 6; i++)
			l.append(i);
		int x = 0;
		for(List<int>::ConstIterator i = l.constBegin(); i != l.constEnd(); ++i)
			FAILED_IF(*i != x++);
		FAILED_IF(x != 6);
	}
	TEST("List count()...")
	{
		FAILED_IF(l.count() != 6);
		FAILED_IF(l.count(0) != 1);
		FAILED_IF(l.count(-1) != 0);
	}
	TEST("List at()")
	{
		for (int i = 0; i < 6; i++)
			FAILED_IF(l.at(i) != i);
	}
	TEST("List operator==, operator<<")
	{
		FAILED_IF(l == (List<int>() << 0 << 1 << 2 << 3 << 4));
		FAILED_IF(l == (List<int>() << 0 << 1 << 2 << 3 << 4 << 69));
		FAILED_IF(!(l == (List<int>() << 0 << 1 << 2 << 3 << 4 << 5)));
	}
	TEST("List operator!=")
	{
		FAILED_IF(!(l != (List<int>() << 0 << 1 << 2 << 3 << 4)));
		FAILED_IF(!(l != (List<int>() << 0 << 1 << 2 << 3 << 4 << 69)));
		FAILED_IF(l != (List<int>() << 0 << 1 << 2 << 3 << 4 << 5));
	}
	TEST("List List(List&)()")
	{
		List<int> m(l);
		FAILED_IF(m != l);
	}
	TEST("List swap()")
	{
		l.swap(2, 0);
		FAILED_IF(l != List<int>() << 2 << 1 << 0 << 3 << 4 << 5);
	}
	TEST("List move()")
	{
		l.move(0, 4);
		FAILED_IF(l != List<int>() << 1 << 0 << 3 << 4 << 2 << 5);
	}
	TEST("List operator=()")
	{
		l = (List<int>() << 0 << 1 << 2 << 3 << 4 << 5);
		FAILED_IF(l != List<int>() << 0 << 1 << 2 << 3 << 4 << 5);
	}
	TEST("List reversed()")
	{
		l = l.reversed();
		FAILED_IF(l != (List<int>() << 5 << 4 << 3 << 2 << 1 << 0));
	}
	TEST("List reverse()")
	{
		l.reverse();
		FAILED_IF(l != List<int>() << 0 << 1 << 2 << 3 << 4 << 5);
	}
	TEST("List operator+()")
	{
		l = l + l.reversed();
		FAILED_IF(l != l.reversed());
	}
	TEST("List indexOf()")
	{
		FAILED_IF(l.indexOf(4) != 4);
		FAILED_IF(l.indexOf(2, 3) != 9);
	}
	TEST("List lastIndexOf()")
	{
		FAILED_IF(l.lastIndexOf(2) != 9);
		FAILED_IF(l.lastIndexOf(2, 3) != 2);
	}
	TEST("List erase()")
	{
		l.erase(l.begin() + 2);
		FAILED_IF(l.indexOf(2) != 8);
		l.erase(l.begin(), l.end());
		FAILED_IF(l != List<int>());
	}
	TEST("List isEmpty()")
	{
		FAILED_IF(!l.isEmpty());
		l << 5 << 6 << 7 << 8;
		FAILED_IF(l.isEmpty());
	}
	TEST("List mid()")
	{
		FAILED_IF(l.mid(1, 2) != (List<int>() << 6 << 7));
	}
	TEST("List last()")
	{
		FAILED_IF(l.last() != 8);
	}
	TEST("List first()")
	{
		FAILED_IF(l.first() != 5);
	}
	TEST("List takeLast()")
	{
		FAILED_IF(l.takeLast() != 8);
		FAILED_IF(l != (List<int>() << 5 << 6 << 7));
	}
	TEST("List insert()")
	{
		l.insert(1, 69);
		FAILED_IF(l != (List<int>() << 5 << 69 << 6 << 7));
	}
	TEST("List removeOne()")
	{
		l.removeOne(69);
		FAILED_IF(l != (List<int>() << 5 << 6 << 7));
	}
	TEST("List prepend()")
	{
		l.prepend(4);
		FAILED_IF(l != (List<int>() << 4 << 5 << 6 << 7));
	}
	TEST("List takeFirst()")
	{
		FAILED_IF(l.takeFirst() != 4);
		FAILED_IF(l != (List<int>() << 5 << 6 << 7));
	}
	TEST("List clear()")
	{
		l.clear();
		FAILED_IF(!l.isEmpty());
	}
	TEST("List operator<<(), count()")
	{
		List<int> t;
		for (int i = 0; i < 10000; ++i)
		{
			t << i;
			if (!(i % 37))
				for (int j = 0; j <= i; j += 109)
					FAILED_IF(t.count(j) != 1);
		}
	}
	TEST("List Box")
	{
		List<int> x;
		for (int i = 0; i < 16; ++i)
			x << i;
		int count = 2048;
		List<List<int> > t1;
		for (int i = 0; i < count; ++i)
			t1 << x;
		List<List<int> > t2;
		t2 = returnMeBox(t1);
		FAILED_IF(!t1.isEmpty());
		FAILED_IF(t2.count() != count);
		List<List<int> > t3(returnMeBox(t2));
		FAILED_IF(!t2.isEmpty());
		FAILED_IF(t3.count() != count);
	}

	TEST("String default construction")
	{
		String s;
		FAILED_IF(s.length() != 0);
		FAILED_IF(strcmp(s.toCString(), ""));
	}
	TEST("String filled construction")
	{
		String s(3, L'.');
		FAILED_IF(s.length() != 3);
		FAILED_IF(strcmp(s.toCString(), "..."));
	}
	TEST("String char const* construction")
	{
		String s("Hello world!");
		FAILED_IF(s.length() != 12);
		FAILED_IF(strlen(s.toCString()) != 12);
		FAILED_IF(strcmp(s.toCString(), "Hello world!"));
	}
	TEST("String copy construction")
	{
		String* s = new String("Hello world!");
		String t(*s);
		delete s;
		FAILED_IF(t.length() != 12);
		FAILED_IF(strlen(t.toCString()) != 12);
		FAILED_IF(strcmp(t.toCString(), "Hello world!"));
	}
	TEST("String assignment")
	{
		String* s = new String("Hello world!");
		String t;
		t = *s;
		delete s;
		FAILED_IF(t.length() != 12);
		FAILED_IF(strlen(t.toCString()) != 12);
		FAILED_IF(strcmp(t.toCString(), "Hello world!"));
	}
	TEST("String assignment (char const*)")
	{
		String t;
		t = "Hello world!";
		FAILED_IF(t.length() != 12);
		FAILED_IF(strlen(t.toCString()) != 12);
		FAILED_IF(strcmp(t.toCString(), "Hello world!"));
	}
	TEST("String isEmpty()")
	{
		String s;
		String t("X");
		FAILED_IF(!s.isEmpty());
		FAILED_IF(t.isEmpty());
	}
	TEST("String clear()")
	{
		String s("Hello world!");
		s.clear();
		FAILED_IF(!s.isEmpty());
	}
	TEST("String operator==()")
	{
		String s("Hello world!");
		String t;
		FAILED_IF(t == s);
		FAILED_IF(s == t);
		FAILED_IF(t != "");
		t = s;
		FAILED_IF(t != s);
		FAILED_IF(s != t);
		FAILED_IF(s != "Hello world!");
	}
	TEST("String mid(), left(), right()")
	{
		String s("0123456789");
		FAILED_IF(s.mid(2, 6) != "234567");
		FAILED_IF(s.mid(6) != "6789");
		FAILED_IF(s.left(6) != "012345");
		FAILED_IF(s.right(6) != "456789");
	}
	TEST("String chop()")
	{
		String s("0123456789");
		s.chop(7);
		FAILED_IF(s != "012");
	}
	TEST("String trimmed()")
	{
		String s = "  lots\t of\nwhitespace\r\n ";
		s = s.trimmed();
		FAILED_IF(s != "lots\t of\nwhitespace");
	}
	TEST("String simplified()")
	{
		String s = "  lots\t of\nwhitespace\r\n ";
		s = s.simplified();
		FAILED_IF(s != "lots of whitespace");
	}
	TEST("String append()")
	{
		String s = "free";
		String t = "dom";
		s.append(t);
		FAILED_IF(s != "freedom");
		s.append(" is best");
		FAILED_IF(s != "freedom is best");
		s.append(L'!');
		FAILED_IF(s != "freedom is best!");
		s.append('!');
		FAILED_IF(s != "freedom is best!!");
	}
	TEST("String contains()")
	{
		FAILED_IF(String("Freedom").contains("free"));
		FAILED_IF(!String("Freedom").contains("Free"));
		FAILED_IF(String("Freedom").contains(L'f'));
		FAILED_IF(!String("Freedom").contains(L'F'));
	}
	TEST("String count()")
	{
		String s = "Free as in speech, not as in beer!";
		FAILED_IF(s.count("ee") != 3);
		FAILED_IF(s.count(L' ') != 7);
	}
	TEST("String startsWith(), endsWith()")
	{
		String s = "Free as in speech, not as in beer!";
		FAILED_IF(!s.startsWith("Free"));
		FAILED_IF(s.startsWith("beer!"));
		FAILED_IF(!s.endsWith("beer!"));
		FAILED_IF(s.endsWith("Free"));
	}
	TEST("String fill()")
	{
		String s;
		s.fill(L'$', 5);
		FAILED_IF(s != "$$$$$");
		s.fill(L'!', 0);
		FAILED_IF(!s.isEmpty());
	}
	TEST("String indexOf()")
	{
		String s = "sticky question stitch-up";
		FAILED_IF(s.indexOf(s) != 0);
		FAILED_IF(s.indexOf(s, -1) != 0);
		FAILED_IF(s.indexOf(s, 1) != -1);
		FAILED_IF(s.indexOf(s, -2) != 0);
		String t = "sti";
		FAILED_IF(s.indexOf(t) != 0);
		FAILED_IF(s.indexOf(t, 1) != 10);
		FAILED_IF(s.indexOf(t, 10) != 10);
		FAILED_IF(s.indexOf(t, 11) != 16);
		FAILED_IF(s.indexOf(t, 17) != -1);
		FAILED_IF(s.indexOf(s.right(3), -1) != s.length() - 3);
		t.clear();
		FAILED_IF(t.lastIndexOf(s) != -1);
		FAILED_IF(s.lastIndexOf(t) != -1);
		String u = "Charming or tedious?";
		wchar_t v = L'o';
		FAILED_IF(u.indexOf(v, -2) != -1);
		FAILED_IF(u.indexOf(v, -4) != 16);
		FAILED_IF(u.indexOf(v, 9) != 9);
		FAILED_IF(u.indexOf(v, 10) != 16);
		FAILED_IF(u.indexOf(v) != 9);
	}
	TEST("String lastIndexOf()")
	{
		String s = "crazy azimuths";
		String t = "az";
		FAILED_IF(s.lastIndexOf(t) != 6);
		FAILED_IF(s.lastIndexOf(t, 6) != 6);
		FAILED_IF(s.lastIndexOf(t, 5) != 2);
		FAILED_IF(s.lastIndexOf(t, 1) != -1);
		t.clear();
		FAILED_IF(t.lastIndexOf(s) != -1);
		FAILED_IF(s.lastIndexOf(t) != -1);
		t = "z";
		FAILED_IF(s.lastIndexOf(t, 14) != 7);
		FAILED_IF(s.lastIndexOf(t, -13) != -1);
		FAILED_IF(s.lastIndexOf(t, -14) != -1);
		FAILED_IF(s.lastIndexOf(t, -15) != -1);
		FAILED_IF(s.lastIndexOf(L'z', 14) != 7);
		FAILED_IF(s.lastIndexOf(L'z', -13) != -1);
		FAILED_IF(s.lastIndexOf(L'z', -14) != -1);
		FAILED_IF(s.lastIndexOf(L'z', -15) != -1);
	}
	TEST("String split()")
	{
		String s = ",x,y,z,";
		FAILED_IF(s.split(L',') != StringList() << "" << "x" << "y" << "z" << "");
		FAILED_IF(s.mid(1, 5).split(L',') != StringList() << "x" << "y" << "z");
		FAILED_IF(s.split(",") != StringList() << "" << "x" << "y" << "z" << "");
		FAILED_IF(s.mid(1, 5).split(",") != StringList() << "x" << "y" << "z");
		String t = "M...A...R...T..T...A";
		FAILED_IF(t.split("...") != StringList() << "M" << "A" << "R" << "T..T" << "A");
		FAILED_IF(t.mid(1, 18).split("..") != StringList() << "" << ".A" << ".R" << ".T" << "T" << ".");
	}
	TEST("String indexOfNth(wchar_t)")
	{
		String s = "";
		FAILED_IF(s.indexOfNth(L'.', 0) != -1);
		FAILED_IF(s.indexOfNth(L'.', 1) != 0);
		FAILED_IF(s.indexOfNth(L'.', 2) != -1);
		s = ".";
		FAILED_IF(s.indexOfNth(L'.', 0) != -1);
		FAILED_IF(s.indexOfNth(L'.', 1) != 0);
		FAILED_IF(s.indexOfNth(L'.', 2) != 1);
		FAILED_IF(s.indexOfNth(L'.', 3) != -1);
		s = "-";
		FAILED_IF(s.indexOfNth(L'.', 0) != -1);
		FAILED_IF(s.indexOfNth(L'.', 1) != 1);
		FAILED_IF(s.indexOfNth(L'.', 2) != -1);
		s = ".x.yy..";
		FAILED_IF(s.indexOfNth(L'.', 0) != -1);
		FAILED_IF(s.indexOfNth(L'.', 1) != 0);
		FAILED_IF(s.indexOfNth(L'.', 2) != 2);
		FAILED_IF(s.indexOfNth(L'.', 3) != 5);
		FAILED_IF(s.indexOfNth(L'.', 4) != 6);
		FAILED_IF(s.indexOfNth(L'.', 5) != 7);
		FAILED_IF(s.indexOfNth(L'.', 6) != -1);
	}
	TEST("String indexOfNth(String)")
	{
		String s = "";
		String t = ".";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != -1);
		s = ".";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != 1);
		FAILED_IF(s.indexOfNth(t, 3) != -1);
		s = "-";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 1);
		FAILED_IF(s.indexOfNth(t, 2) != -1);
		s = ".x.yy..";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != 2);
		FAILED_IF(s.indexOfNth(t, 3) != 5);
		FAILED_IF(s.indexOfNth(t, 4) != 6);
		FAILED_IF(s.indexOfNth(t, 5) != 7);
		FAILED_IF(s.indexOfNth(t, 6) != -1);
		t = "..";
		s = "";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != -1);
		s = "..";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != 2);
		FAILED_IF(s.indexOfNth(t, 3) != -1);
		s = "-";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 1);
		FAILED_IF(s.indexOfNth(t, 2) != -1);
		s = "..x..yy....";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != 3);
		FAILED_IF(s.indexOfNth(t, 3) != 7);
		FAILED_IF(s.indexOfNth(t, 4) != 9);
		FAILED_IF(s.indexOfNth(t, 5) != 11);
		FAILED_IF(s.indexOfNth(t, 6) != -1);
	}
	TEST("String lastIndexOfNth(wchar_t)")
	{
		String s = "";
		FAILED_IF(s.lastIndexOfNth(L'.', 0) != 0);
		FAILED_IF(s.lastIndexOfNth(L'.', 1) != -1);
		s = ".";
		FAILED_IF(s.lastIndexOfNth(L'.', 0) != 1);
		FAILED_IF(s.lastIndexOfNth(L'.', 1) != 0);
		FAILED_IF(s.lastIndexOfNth(L'.', 2) != -1);
		s = "-";
		FAILED_IF(s.lastIndexOfNth(L'.', 0) != 1);
		FAILED_IF(s.lastIndexOfNth(L'.', 1) != -1);
		s = ".x.yy..";
		FAILED_IF(s.lastIndexOfNth(L'.', 0) != 7);
		FAILED_IF(s.lastIndexOfNth(L'.', 1) != 6);
		FAILED_IF(s.lastIndexOfNth(L'.', 2) != 5);
		FAILED_IF(s.lastIndexOfNth(L'.', 3) != 2);
		FAILED_IF(s.lastIndexOfNth(L'.', 4) != 0);
		FAILED_IF(s.lastIndexOfNth(L'.', 5) != -1);
	}
	TEST("String lastIndexOfNth(String)")
	{
		String s = "";
		String t = ".";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 1) != -1);
		s = ".";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 1);
		FAILED_IF(s.lastIndexOfNth(t, 1) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 2) != -1);
		s = "-";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 1);
		FAILED_IF(s.lastIndexOfNth(t, 1) != -1);
		s = ".x.yy..";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 7);
		FAILED_IF(s.lastIndexOfNth(t, 1) != 6);
		FAILED_IF(s.lastIndexOfNth(t, 2) != 5);
		FAILED_IF(s.lastIndexOfNth(t, 3) != 2);
		FAILED_IF(s.lastIndexOfNth(t, 4) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 5) != -1);
		t = "..";
		s = "";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 1) != -1);
		s = "..";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 2);
		FAILED_IF(s.lastIndexOfNth(t, 1) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 2) != -1);
		s = "-";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 1);
		FAILED_IF(s.lastIndexOfNth(t, 1) != -1);
		s = "..x..yy....";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 11);
		FAILED_IF(s.lastIndexOfNth(t, 1) != 9);
		FAILED_IF(s.lastIndexOfNth(t, 2) != 7);
		FAILED_IF(s.lastIndexOfNth(t, 3) != 3);
		FAILED_IF(s.lastIndexOfNth(t, 4) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 5) != -1);
	}
	TEST("String section()")
	{
		String s = "apples,oranges,pears,grapes";
		FAILED_IF(s.section(L',', 0, 0) != "apples");
		FAILED_IF(s.section(L',', 1, 3) != "oranges,pears,grapes");
		FAILED_IF(s.section(L',', 2, -2) != "pears");
		FAILED_IF(s.section(L',', -4, -3) != "apples,oranges");
		FAILED_IF(s.section(L',', -1, -1) != "grapes");
		FAILED_IF(s.section(L',', 3, 3) != "grapes");
		String t = ",";
		FAILED_IF(s.section(t, 0, 0) != "apples");
		FAILED_IF(s.section(t, 1, 3) != "oranges,pears,grapes");
		FAILED_IF(s.section(t, 2, -2) != "pears");
		FAILED_IF(s.section(t, -4, -3) != "apples,oranges");
		FAILED_IF(s.section(t, -1, -1) != "grapes");
		FAILED_IF(s.section(t, 3, 3) != "grapes");
		s = "X....Y";
		t = "..";
		FAILED_IF(s.section(t, 0, 0) != "X");
		FAILED_IF(s.section(t, 1, 1) != "");
		FAILED_IF(s.section(t, 2, 2) != "Y");
		FAILED_IF(s.section(t, 0, 1) != "X..");
		FAILED_IF(s.section(t, 1, 2) != "..Y");
		FAILED_IF(s.section(t, -3, -1) != "X....Y");
	}
	TEST("String toUpper(), toLower()")
	{
		FAILED_IF(String("Hello world!").toUpper() != String("HELLO WORLD!"));
		FAILED_IF(String("EbenEEZER GOOD!").toLower() != String("ebeneezer good!"));
	}
	TEST("String replace()")
	{
		String str = "rock and roll";
		str.replace(5, 3, L'&');
		FAILED_IF(str != "rock & roll");
		str.replace(5, 1, "and");
		FAILED_IF(str != "rock and roll");
		str.replace("and", "&&&");
		FAILED_IF(str != "rock &&& roll");
		str.replace(L'&', L'+');
		FAILED_IF(str != "rock +++ roll");
		str.replace(L'+', "'");
		FAILED_IF(str != "rock ''' roll");
	}
	TEST("String prepend(), remove(), insert()")
	{
		String str = "XaXaaaX";
		str.remove(L'X');
		FAILED_IF(str != "aaaa");
		str.prepend("ock");
		FAILED_IF(str != "ockaaaa");
		str.prepend(L'R');
		FAILED_IF(str != "Rockaaaa");
		str.insert(6, " and rol");
		FAILED_IF(str != "Rockaa and rolaa");
		str.remove("aa");
		FAILED_IF(str != "Rock and rol");
		str.insert(11, L'l');
		FAILED_IF(str != "Rock and roll");
	}
	TEST("String toInt(), toUint(), toDouble(), toFloat()")
	{
		bool ok;
		FAILED_IF(String("-1").toInt() != -1);
		FAILED_IF(String("0x7d4a").toInt(&ok, 16) != 0x7d4a);
		FAILED_IF(!ok);
		String().toInt(&ok);
		FAILED_IF(ok);
		FAILED_IF(String("69").toUint() != 69u);
		FAILED_IF(String("101010").toUint(&ok, 2) != 42u);
		FAILED_IF(!ok);
		String().toUint(&ok);
		FAILED_IF(ok);
		FAILED_IF(String("56.2").toDouble() != 56.2);
		FAILED_IF(String("0.5e-9").toDouble(&ok) != 0.5e-9);
		FAILED_IF(!ok);
		String().toDouble(&ok);
		FAILED_IF(ok);
		FAILED_IF(String("69.69").toFloat() != 69.69f);
		FAILED_IF(String("-96").toFloat(&ok) != -96.f);
		FAILED_IF(!ok);
		String().toFloat(&ok);
		FAILED_IF(ok);
	}
	TEST("String operator<, >, <=, >=")
	{
		String n;
		String ab("ab");
		String aa("aa");
		String zz("zz");
		FAILED_IF(n < n);
		FAILED_IF(n > n);
		FAILED_IF(!(n <= n));
		FAILED_IF(!(n >= n));
		FAILED_IF(!(n < ab));
		FAILED_IF(n > ab);
		FAILED_IF(!(n <= ab));
		FAILED_IF(n >= ab);
		FAILED_IF(ab < aa);
		FAILED_IF(ab > zz);
		FAILED_IF(!(ab < "ac"));
		FAILED_IF(zz > "zz");
		FAILED_IF(zz < "zz");
		FAILED_IF(zz >= "zzz");
		FAILED_IF(zz <= "zy");
		FAILED_IF("zz" > zz);
		FAILED_IF("zz" < zz);
		FAILED_IF("zy" >= zz);
		FAILED_IF("zzz" <= zz);
	}
	TEST("String operator+")
	{
		String a("foo");
		String b("bar");
		FAILED_IF(a + ' ' != "foo ");
		FAILED_IF(' ' + b != " bar");
		FAILED_IF(a + b != "foobar");
		FAILED_IF("foo " + b != "foo bar");
		FAILED_IF(a + " bar" != "foo bar");
	}
	TEST("String number()")
	{
		FAILED_IF(String::number(0.696969) != "0.696969");
		FAILED_IF(String::number(-69) != "-69");
		FAILED_IF(String::number(42u) != "42");
		FAILED_IF(String::number(42u, 16) != "2a");
		FAILED_IF(String::number(42u, 8) != "52");
	}
	TEST("String arg()")
	{
		FAILED_IF(String("%2%3%1%4").arg("world").arg("Hello").arg(" ").arg("!") != "Hello world!");
		FAILED_IF(String("%4 %3 %1%2").arg(69u, 0, 10).arg('!').arg(L'a', -6, L'.').arg("Make mine") != "Make mine a..... 69!");
		FAILED_IF(String("%4 %3 %1%2").arg(69).arg(L'!').arg('a').arg("Make mine") != "Make mine a 69!");
		String s = String("%1 / %2 = %3").arg(69.f, 0, 'g', 4).arg(999999u, 0, 16).arg(69.f/999999, 0, 'e', 1);
		FAILED_IF(s != "69 / f423f = 6.9e-05" && s != "69 / f423f = 6.9e-005");
	}
	TEST("String hashOf()")
	{
		FAILED_IF(hashOf("BBC") == hashOf("CBB"));
		FAILED_IF(hashOf("CGHQ") == hashOf("GCHQ"));
		FAILED_IF(hashOf("XYZ") != hashOf("XYZ"));
	}
	TEST("String Box")
	{
		int count = 65536;
		String s1(count, 'x');
		String s2;
		s2 = returnMeBox(s1);
		FAILED_IF(s2.length() != count);
		FAILED_IF(!s1.isEmpty());
		String s3(returnMeBox(s2));
		FAILED_IF(s3.length() != count);
		FAILED_IF(!s2.isEmpty());
	}
	
	TEST("Hash construction")
	{
		Hash<int, int> h;
		FAILED_IF(!h.isEmpty());
	}
	TEST("HashOf int")
	{
		char const* x = "Hello world!";
		uint i = (uint)x;
		FAILED_IF(hashOf(i) != i);
		int const j = (int)x;
		FAILED_IF((int)hashOf(j) != j);
	}
	TEST("Hash insert()")
	{
		Hash<int, int> h;
		h.setAutoResize(false);
		for (int i = 0; i < 1024; ++i)
			h.insert(i, i + 1);
		for (int i = 0; i < 1024; ++i)
			FAILED_IF(h.value(i) != i + 1);
	}
	TEST("Hash isEmpty()")
	{
		Hash<String, int> h;
		FAILED_IF(!h.isEmpty());
		h.insert("0", 0);
		FAILED_IF(h.isEmpty());
	}
	TEST("Hash resize(), Iterator")
	{
		MultiHash<int, int> h;
		int count = 16384;
		int b = max(5u, floorLog2(count) + 1);
		int resizes = 15;
		for (int i = 0; i < count; ++i)
			h.insert(i % (count / 8), i);
		for (int i = 0; i < resizes * (b - 5) * 2; ++i)
			h.resize(1 << (abs(b - 6 - i % ((b - 5) * 2)) + 5));
		for (int i = count / 8 - 1; i >= 0; --i)
		{
			int k = count - count / 8 + i;
			for (MultiHash<int, int>::Iterator j = h.find(i); j != h.end() && j.key() == i; ++j, k -= count / 8)
				FAILED_IF(j.value() != k);
			FAILED_IF(k >= 0);
		}
	}
	TEST("Hash values()")
	{
		Hash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h.insert(i % 10, i);
		FAILED_IF(h.count() != 10);
		for (int i = 0; i < 10; ++i)
			FAILED_IF(h.values(i) != List<int>() << 90 + i);
	}
	TEST("Hash insertMulti()")
	{
		MultiHash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h.insert(i % 10, i);
		FAILED_IF(h.count() != 100);
		for (int i = 0; i < 10; ++i)
			FAILED_IF(h.values(i) != List<int>() << (90 + i) << (80 + i) << (70 + i) << (60 + i) << (50 + i) << (40 + i) << (30 + i) << (20 + i) << (10 + i) << i);
	}
	TEST("Hash keys(), uniqueKeys()")
	{
		MultiHash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h.insert(i % 10, i);
		List<int> k = h.keys();
		List<int> u = h.uniqueKeys();
		FAILED_IF(k.count() != 100);
		FAILED_IF(u.count() != 10);
		for (int i = 0; i < 10; ++i)
		{
			FAILED_IF(k.count(i) != 10);
			FAILED_IF(u.count(i) != 1);
		}
	}
	TEST("Hash remove(), removeOne(), take()")
	{
		MultiHash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h.insert(i % 10, i);
		for (int i = 80; i >= 0; i -= 10)
		{
			h.removeOne(0);
			FAILED_IF(h[0] != i);
		}
		FAILED_IF(h.take(0) != 0);
		for (int i = 1; i < 10; ++i)
			h.remove(i);
		FAILED_IF(!h.isEmpty());
	}
	TEST("Hash keys(T)")
	{
		Hash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h[i] = i % 10;
		for (int i = 0; i < 10; ++i)
		{
			List<int> l = h.keys(i);
			FAILED_IF(l.count() != 10);
			for (int j = 0; j < 10; ++j)
				FAILED_IF(!l.contains(j * 10 + i));
		}
	}
	{
	Hash<int, int> h;
	h[0] = 69;
	h[1] = 42;
	h[32] = 1;
	h[64] = 2;
	h[96] = 3;
	{
	int ks[] = {96, 64, 32, 0, 1};
	int vs[] = {3, 2, 1, 69, 42};
	TEST("Hash constBegin(), constEnd(), ConstIterator::operator++, ConstIterator::operator--")
	{
		int j = 0;
		for (Hash<int, int>::ConstIterator i = h.constBegin(); i != h.constEnd(); ++i, ++j)
		{
			FAILED_IF(i.key() != ks[j]);
			FAILED_IF(i.value() != vs[j]);
		}
		for (Hash<int, int>::ConstIterator i = h.constEnd(); i != h.constBegin();)
		{
			--i;
			--j;
			FAILED_IF(i.key() != ks[j]);
			FAILED_IF(i.value() != vs[j]);
		}
	}
	TEST("Hash begin(), end(), Iterator::operator++, Iterator::operator--")
	{
		int j = 0;
		for (Hash<int, int>::Iterator i = h.begin(); i != h.end(); ++i, ++j)
		{
			FAILED_IF(i.key() != ks[j]);
			FAILED_IF(i.value() != vs[j]);
		}
		for (Hash<int, int>::Iterator i = h.end(); i != h.begin();)
		{
			--i;
			--j;
			FAILED_IF(i.key() != ks[j]);
			FAILED_IF(i.value() != vs[j]);
		}
	}
	}
	TEST("Hash Iterator erase()")
	{
		FAILED_IF(*h.erase(h.find(0)) != 42);		// SbNa, = 42
		FAILED_IF(*h.erase(h.find(64)) != 1);		// SbSa, = 1
		FAILED_IF(*h.erase(h.find(96)) != 1);		// NbSa, = 1
		FAILED_IF(*h.erase(h.find(32)) != 42);		// NbNa, = 42
		FAILED_IF(h.erase(h.find(1)) != h.end());	// NbNa, end().
	}
	}
	TEST("Hash operator==(), operator!=()")
	{
		MultiHash<int, int> h1;
		MultiHash<int, int> h2;
		h1.insert(0, 0);
		h1.insert(1, 1);
		h1.insert(32, 2);
		h1.insert(0, 3);
		h2.insert(0, 4);
		h2.insert(1, 1);
		h2.insert(32, 2);
		h2.insert(0, 3);
		FAILED_IF(h1 == h2);
		FAILED_IF(!(h1 != h2));
		h2.clear();
		h2.insert(0, 0);
		h2.insert(1, 1);
		h2.insert(32, 2);
		h2.insert(0, 3);
		FAILED_IF(h1 != h2);
		FAILED_IF(!(h1 == h2));
	}
	TEST("Hash unite(), values(), operator=(), Hash(Hash)")
	{
		MultiHash<int, int> h1;
		MultiHash<int, int> h2;
		h1.insert(0, 0);
		h1.insert(0, 1);
		h1.insert(32, 0);
		h1.insert(32, 1);
		h2.insert(0, 2);
		h2.insert(0, 3);
		h2.insert(64, 0);
		h2.insert(64, 1);
		h1.unite(h2);
		FAILED_IF(h1.values(0) != List<int>() << 3 << 2 << 1 << 0);
		FAILED_IF(h1.values(32) != List<int>() << 1 << 0);
		FAILED_IF(h1.values(64) != List<int>() << 1 << 0);
		h2 = h1;
		FAILED_IF(h2 != h1);
		Hash<int, int, 16> h3(h1);
		FAILED_IF(h3 != h1);
		Hash<int, int, 16> h4;
		h4 = h1;
		FAILED_IF(h4 != h1);
	}
	TEST("Hash Box")
	{
		int count = 16384;
		MultiHash<int, int> h1;
		for (int i = 0; i < count; ++i)
			h1.insert(i * 8 % (count / 128), i + 1);
		MultiHash<int, int> h2;
		h2 = returnMeBox(h1);
		FAILED_IF(!h1.isEmpty());
		FAILED_IF(h2.count() != count);
		MultiHash<int, int> h3(returnMeBox(h2));
		FAILED_IF(!h2.isEmpty());
		FAILED_IF(h3.count() != count);
	}
	
	return failed;
}