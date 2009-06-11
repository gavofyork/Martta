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

class QDomElement;

namespace Martta
{

class OperatorLabel: public IdLabel
{
	MARTTA_OBJECT(IdLabel)
	
public:
	OperatorLabel() {}
	OperatorLabel(Operator _o): m_operator(_o) {}
	OperatorLabel(Operator::Symbol _s): m_operator(_s) {}
	OperatorLabel(QString const& _key): m_operator(_key) {}

	Operator							id() const { return m_operator; }
	virtual QString						code() const { return "operator" + symbolCode(); }
	inline QString						symbolCode() const { return m_operator.code(); }
	Operator::Symbol					symbol() const { return m_operator.symbol(); }
	void								setSymbol(Operator::Symbol _s) { m_operator.setSymbol(_s); }
	
protected:
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual QString						defineLayout(ViewKeys&) const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);

private:
	Operator							m_operator;
};

}
