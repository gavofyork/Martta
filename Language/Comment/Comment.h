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

#include <msString.h>
using namespace MarttaSupport;

#include "Statement.h"

#ifndef M_API_Comment
#define M_API_Comment M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Comment Comment: public_super Statement
{
	MARTTA_PROPER(Statement)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

	String								value() const { return m_value; }
	void								setValue(String const& _s) { m_value = _s; changed(); }

protected:
	virtual String						code() const { return String::null; }
	virtual bool						requiresSemicolon() const { return false; }
	virtual String						defineHtml() const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"value"] = m_value; }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_value = _p[L"value"]; }

private:
	String								m_value;
};

}
