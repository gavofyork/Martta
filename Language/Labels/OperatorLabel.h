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

#include "Operator.h"
#include "IdLabel.h"

#ifndef M_API_Labels
#define M_API_Labels M_OUTAPI
#endif

namespace Martta
{

class M_API_Labels OperatorLabel: public_super IdLabel
{
	MARTTA_OBJECT(IdLabel)

public:
	OperatorLabel() {}
	OperatorLabel(Operator _o): m_operator(_o) {}
	OperatorLabel(Operator::Symbol _s): m_operator(_s) {}
	OperatorLabel(String const& _key): m_operator(_key) {}

	Operator							id() const { return m_operator; }
	virtual String						code() const { return "operator" + symbolCode(); }
	inline String						symbolCode() const { return m_operator.code(); }
	Operator::Symbol					symbol() const { return m_operator.symbol(); }
	void								setSymbol(Operator::Symbol _s) { m_operator.setSymbol(_s); }

protected:
	virtual String						defineHtml() const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"symbol"] = String::number(symbol()); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); setSymbol((Operator::Symbol)_p[L"symbol"].toInt()); }

private:
	Operator							m_operator;
};

}
