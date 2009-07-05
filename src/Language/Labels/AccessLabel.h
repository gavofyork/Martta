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

#include <msRgb.h>
using MarttaSupport::Rgb;

#include "Label.h"

namespace Martta
{

class AccessLabel: public Label
{
	MARTTA_OBJECT(Label)
	
public:
	AccessLabel(Access _a = Public): m_access(_a) {}
	
	inline Rgb							idColour() const { return idColour(m_access); }
	inline Access						access() const { return m_access; }
	inline void							setAccess(Access _a) { m_access = _a; changed(); }
	
	inline static Rgb					idColour(Access _a) { return _a == Public ? Rgb(192, 192, 0) : _a == Protected ? Rgb(255, 127, 0) : _a == Private ? Rgb(255, 0, 0) : Rgb(0, 0, 0); }
	
protected:
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual String						defineLayout(ViewKeys const&) const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual String						code() const { return Martta::code(m_access); }
	virtual bool						onActivated(CodeScene*) { setAccess((Access)(((int)m_access + 1) % (int)AccessCount)); return true; }
	
private:
	Access								m_access;
};

}
