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

#include "Label.h"

namespace Martta
{
	
class ConstLabel: public Label
{
	MARTTA_OBJECT(Label)
	
public:
	ConstLabel(bool _c = false): m_isConst(_c) {}
	bool								isConst() const { return m_isConst; }
	void								setIsConst(bool _c) { m_isConst = _c; changed(); }
	
protected:
	virtual void						exportDom(QDomElement&) const;
	virtual void						importDom(QDomElement const&);
	virtual QString						defineLayout(ViewKeys&) const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual QString						code() const { return m_isConst ? " const" : ""; }
	virtual bool						activated(CodeScene*) { setIsConst(!m_isConst); return true; }
	
private:
	bool								m_isConst;
};

}
