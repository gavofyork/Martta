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

#include "Const.h"
#include "Pointer.h"
#include "Literal.h"

namespace Martta
{

class StringLiteral: public Literal
{
	MARTTA_OBJECT(Literal)

public:
	StringLiteral(QString const& _string = QString()): m_value(_string) {}

	QString								value() const { return m_value; }
	void								setValue(QString const& _s) { m_value = _s; changed(); }
	
	static bool							keyPressedOnInsertionPoint(Position const& _p, EntityKeyEvent const* _e);

protected:
	virtual Type						type() const { return Type(Char).topWith(Const()).topWith(Pointer()); }
	virtual QString						code() const { return "\"" + QString(m_value).replace("\"", "\\\"") + "\""; }
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual QString						defineLayout(ViewKeys&) const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual bool						keyPressed(EntityKeyEvent const* _e);

private:
	QString								m_value;
};

}
