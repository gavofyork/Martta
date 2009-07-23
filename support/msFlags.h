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

#pragma once

#include "msSupport.h"

namespace MarttaSupport
{

template<class Flag>
class Flags
{
public:
	Flags(): m_value(0) {}
	Flags(Flag _v): m_value(_v) {}
	Flags(Flags const& _v): m_value(_v.m_value) {}
	explicit Flags(uint _v): m_value(_v) {}

	operator uint() const { return m_value; }

	m_inline Flags& set(Flag _f, bool _v = true) { m_value = (test(_f) != _v) ? m_value ^ _f : m_value; return *this; }
	m_inline bool test(Flag _f) const { return m_value & _f; }

	bool operator!() const { return !m_value; }
	Flags operator~() const { return Flags(~m_value); }
	Flags operator|(Flag _q) const { return Flags(m_value | _q); }
	Flags operator|(Flags _qs) const { return Flags(m_value | _qs.m_value); }
	Flags operator&(Flag _q) const { return Flags(m_value & _q); }
	Flags operator&(Flags _qs) const { return Flags(m_value & _qs.m_value); }
	Flags operator^(Flag _q) const { return Flags(m_value ^ _q); }
	Flags operator^(Flags _qs) const { return Flags(m_value ^ _qs.m_value); }
	Flags& operator|=(Flag _q) { m_value |= _q; return *this; }
	Flags& operator|=(Flags _qs) { m_value |= _qs; return *this; }
	Flags& operator&=(Flag _q) { m_value &= _q; return *this; }
	Flags& operator&=(Flags _qs) { m_value &= _qs; return *this; }
	Flags& operator^=(Flag _q) { m_value ^= _q; return *this; }
	Flags& operator^=(Flags _qs) { m_value ^= _qs; return *this; }

private:
	uint m_value;
};

#define MS_DECLARE_FLAGS(Flag) MS_DECLARE_FLAGS_TYPE(Flag); MS_DECLARE_FLAGS_OPERATORS(Flag)

#define MS_DECLARE_FLAGS_TYPE(Flag) \
typedef Flags<Flag> Flag ## s

#define MS_DECLARE_FLAGS_OPERATORS(Flag) \
m_inline Flag ## s operator|(Flag _a, Flag _b) { return Flag ## s(_a|_b); } \
m_inline Flag ## s operator|(Flag _a, Flag ## s _b) { return _b | _a; } \
enum{}

}
