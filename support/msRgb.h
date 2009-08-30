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

class M_API_support Rgb
{
public:
	m_inline Rgb(): m_value(0xff000000) {}
	m_inline Rgb(uint _v, uchar _a = 255): m_value((_v & 0xffffff) + (uint)(((uint)_a) << 24ul)) {}
	m_inline Rgb(uchar _r, uchar _g, uchar _b, uchar _a = 255): m_value(fromRgb(_r, _g, _b, _a)) {}
	m_inline Rgb(Rgb _colour, uchar _a): m_value(fromRgb(_colour.r(), _colour.g(), _colour.b(), _a)) {}
	m_inline explicit Rgb(String const& _name)
	{
		if (_name.startsWith(L'#') && _name.length() == 5)
			m_value = fromRgb(_name[1].toInt(0, 16), _name[2].toInt(0, 16), _name[3].toInt(0, 16), _name[4].toInt(0, 16)) * 0x11;
		else if (_name.startsWith(L'#') && _name.length() == 9)
			m_value = _name.mid(1, 6).toUint(0, 16) + (_name.mid(7, 2).toUint(0, 16) << 24);
		else if (_name.startsWith(L'#') && _name.length() == 4)
			m_value = fromRgb(_name[1].toInt(0, 16), _name[2].toInt(0, 16), _name[3].toInt(0, 16), 15) * 0x11;
		else if (_name.startsWith(L'#') && _name.length() == 7)
			m_value = _name.mid(1, 6).toUint(0, 16) + (0xff << 24);
		else
			m_value = fromName(_name);
	}

	m_inline uchar r() const { return toRed(m_value); }
	m_inline uchar g() const { return toGreen(m_value); }
	m_inline uchar b() const { return toBlue(m_value); }
	m_inline uchar a() const { return toAlpha(m_value); }

	m_inline void setR(uchar _v) { m_value = fromRgb(_v, g(), b(), a()); }
	m_inline void setG(uchar _v) { m_value = fromRgb(r(), _v, b(), a()); }
	m_inline void setB(uchar _v) { m_value = fromRgb(r(), g(), _v, a()); }
	m_inline void setA(uchar _v) { m_value = fromRgb(r(), g(), b(), _v); }

	m_inline String name() const { return (a() == 255) ? String(L"#%1").arg((uint)((uint)m_value & 0xffffff), 6, 16, L'0') : String(L"#%1").arg((uint)(((uint)m_value << 8ul) + a()), 8, 16, L'0'); }
	m_inline String name(uchar _alpha) const { return (_alpha == 255) ? String(L"#%1").arg((uint)((uint)m_value & 0xffffff), 6, 16, L'0') : String(L"#%1").arg((uint)(((uint)m_value << 8ul) + (uint)_alpha), 8, 16, L'0'); }

	m_inline Rgb interpolated(int _percent, Rgb _c = black) const
	{
		return fromRgb(	((int)r() * (100 - _percent) + (int)_c.r() * _percent) / 100,
						((int)g() * (100 - _percent) + (int)_c.g() * _percent) / 100,
						((int)b() * (100 - _percent) + (int)_c.b() * _percent) / 100,
						((int)a() * (100 - _percent) + (int)_c.a() * _percent) / 100);
	}
	m_inline Rgb interpolated(int _percent, uchar _alpha, Rgb _c = black) const
	{
		return fromRgb(	((int)r() * (100 - _percent) + (int)_c.r() * _percent) / 100,
						((int)g() * (100 - _percent) + (int)_c.g() * _percent) / 100,
						((int)b() * (100 - _percent) + (int)_c.b() * _percent) / 100, _alpha);
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
	static m_inline uint fromRgb(uchar _r, uchar _g, uchar _b, uchar _a) { uint ret = _a; return (((((ret <<= 8) += _r) <<= 8) += _g) <<= 8) += _b; }
	static m_inline uchar toRed(uint _v) { return (_v >> 16) & 255; }
	static m_inline uchar toGreen(uint _v) { return (_v >> 8) & 255; }
	static m_inline uchar toBlue(uint _v) { return _v & 255; }
	static m_inline uchar toAlpha(uint _v) { return (_v >> 24) & 255; }
	static m_inline uint fromName(String const& _name)
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

	uint m_value;
};

}
