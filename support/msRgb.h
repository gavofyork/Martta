/**
 * @filename msRgb.h
 * @author Gavin Wood
 * @date 04/07/2009
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

#include "msString.h"
#include "msSupport.h"

namespace MarttaSupport
{

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
			m_value = fromName(_name);
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

private:
	static inline t::uint fromRgb(uchar _r, uchar _g, uchar _b, uchar _a) { t::uint ret = _a; return (((((ret <<= 8) += _r) <<= 8) += _g) <<= 8) += _b; }
	static inline uchar toRed(t::uint _v) { return (_v >> 16) & 255; }
	static inline uchar toGreen(t::uint _v) { return (_v >> 8) & 255; }
	static inline uchar toBlue(t::uint _v) { return _v & 255; }
	static inline uchar toAlpha(t::uint _v) { return (_v >> 24) & 255; }
	static inline t::uint fromName(String const& _name)
	{
#define NOTE_NAMED(n) if (_name == L ## #n) return n.m_value
		NOTE_NAMED(black);
		NOTE_NAMED(white);
		NOTE_NAMED(red);
		NOTE_NAMED(green);
		NOTE_NAMED(blue);
		NOTE_NAMED(yellow);
		NOTE_NAMED(magenta);
		NOTE_NAMED(cyan);
#undef NOTE_NAMED
		return 0xff000000;
	}
	
	t::uint m_value;
};

}
