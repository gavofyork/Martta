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

#include "IdLabel.h"

namespace Martta
{
	
class TextLabel: public IdLabel
{
	MARTTA_OBJECT(IdLabel)
	
public:
	TextLabel(QString const& _t = QString()): m_text(_t) {}
	
	QString								name() const;
	QString								text() const { return m_text; }
	virtual QString						code() const;
	void								setText(QString const& _t) { m_text = _t; changed(); }
	
protected:
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual QString						defineLayout(ViewKeys&) const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual void						decorate(DecorationContext const& _c) const;
	
private:
	QString								m_text;
};

}

