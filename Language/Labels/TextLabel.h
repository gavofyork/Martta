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

#include "WebViewable.h"
#include "IdLabel.h"

namespace Martta
{

class TextLabel: public IdLabel, public_interface WebViewable
{
	MARTTA_OBJECT(IdLabel)
	MARTTA_INHERITS(WebViewable, 0)

public:
	TextLabel(String const& _t = String::null): m_text(_t) {}

	// Queries the parent's isChildInValidState() to check if we're valid - if so and we're null it makes up a name. If not it just returns "".
	// @note NEVER call them from the parent's isChildInValidState() function.
	virtual String						name() const;
	virtual String						code() const;

	// These tell the truth (i.e. independent of what parent thinks).
	bool								isNamed() const { return !m_text.isEmpty(); }
	String								text() const { return m_text; }
	void								setText(String const& _t) { m_text = _t; changed(); }

protected:
	virtual void						apresLoad();
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual String						defineHtml() const;
	virtual String						defineEditHtml(CodeScene* _cs) const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"text"] = m_text.isEmpty() ? code() : m_text; }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_text = _p[L"text"]; }

private:
	String								m_text;
};

}

