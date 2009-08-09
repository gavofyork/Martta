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

namespace Martta
{

class BoolLiteral: public Literal
{
	MARTTA_OBJECT(Literal)

public:
	BoolLiteral(bool _value = false): m_value(_value) {}

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

	void								setValue(bool _v) { m_value = _v; changed(); }

protected:
	virtual Type						type() const { return Type(Bool); }
	virtual String						code() const { return m_value ? "true" : "false"; }
	virtual String						defineLayout(ViewKeys const&) const;
	virtual String						defineHtml() const;
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"value"] = String::number(m_value); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_value = _p[L"value"].toBool(); }

private:
	bool								m_value;
};

}
