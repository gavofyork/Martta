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

#include "Literal.h"

namespace Martta
{

class FloatLiteral: public Literal
{
	MARTTA_OBJECT(Literal)

public:
	enum Precision { SinglePrecision, DoublePrecision, LongDoublePrecision };
	
	FloatLiteral(float _value = 0.f): m_value(_value), m_precision(SinglePrecision) {}
	FloatLiteral(double _value): m_value(_value), m_precision(DoublePrecision) {}
	FloatLiteral(long double _value): m_value(_value), m_precision(LongDoublePrecision) {}

	long double							value() const { return m_value; }
	void								setValue(long double _s) { m_value = _s; changed(); }
	Precision							precision() const { return m_precision; }
	void								setPrecision(Precision _p) { m_precision = _p; m_value = (_p == DoublePrecision) ? (double)m_value : (_p == SinglePrecision) ? (float)m_value : m_value; changed(); }
	
	static bool							keyPressedOnPosition(Position const& _p, EntityKeyEvent const* _e);

protected:
	virtual Type						type() const { return Type(m_precision == SinglePrecision ? Float : m_precision == DoublePrecision ? Double : (Long|Double)); }
	virtual String						code() const { String ret = String("%1").arg(m_value); if (!ret.contains(".")) ret += "."; ret += m_precision == SinglePrecision ? "f" : m_precision == DoublePrecision ? "" : "ld"; return ret; }
	virtual String						defineLayout(ViewKeys const&) const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);

private:
	double								m_value;
	Precision							m_precision;
};

}
