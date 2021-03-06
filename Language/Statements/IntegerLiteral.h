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

#include "BuiltinType.h"
#include "Literal.h"

#ifndef M_API_Statements
#define M_API_Statements M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Statements IntegerLiteral: public_super Literal
{
	MARTTA_PROPER(Literal)

public:
	enum Range { NaturalRange = Natural, ShortRange = Short, LongRange = Long, LonglongRange = Longlong };

	IntegerLiteral(int _value = 0, Range _r = NaturalRange, bool _signedness = true): m_value(_value), m_range(_r), m_signed(_signedness) {}

	double								value() const { return m_value; }
	void								setValue(double _s) { m_value = _s; changed(); }
	Range								range() const { return m_range; }
	void								setRange(Range _r) { m_range = _r; changed(); }
	bool								signedness() const { return m_signed; }
	void								setSignedness(bool _s) { m_signed = _s; changed(); }

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual Type						type() const { return Type(Int|(m_signed ? Signed : Unsigned)|((int)m_range)); }
	virtual String						code() const { return String("%1").arg(m_value, 0, 'f', 0) + (m_signed ? "" : "U") + (m_range == ShortRange ? "" : m_range == LongRange ? "L" : m_range == LonglongRange ? "LL" : ""); }
	virtual String						defineHtml() const;
	virtual String						defineEditHtml(CodeScene* _s) const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"range"] = String::number(m_range); _p[L"signed"] = String::number(m_signed); _p[L"value"] = String::number(m_value); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_range = (Range)_p[L"range"].toInt(); m_signed = _p[L"signed"].toBool(); m_value = _p[L"value"].toDouble(); }

private:
	double								m_value;
	Range								m_range;
	bool								m_signed;
};

}
