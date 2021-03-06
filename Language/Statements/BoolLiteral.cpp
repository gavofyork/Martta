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

#include "IdentifierSet.h"
#include "BoolLiteral.h"

namespace Martta
{

MARTTA_PROPER_CPP(BoolLiteral);

class BoolIdentifierSet: public IdentifierSet
{
public:
	BoolIdentifierSet():
		m_true	(L"true"),
		m_false	(L"false")
	{}
	virtual String						setId() const { return L"Martta::BoolLiteral"; }
	virtual List<Named*>				identifiableAt(Position const& _p)
	{
		if (_p.allowedToBeKind<BoolLiteral>())
			return List<Named*>() << &m_true << &m_false;
		return List<Named*>();
	}
	virtual void						acceptAt(Position const& _pos, Named* _n, CodeScene* _cs)
	{
		_cs->setCurrent(_pos.place((new BoolLiteral(_n == &m_true))->prepareChildren()));
	}
	virtual String						defineEditHtml(Named*, String const& _mid)
	{
		return L"<span class=\"keyword\">" + Type(Bool)->typeHtml(_mid) + "</span>";
	}
private:
	SimpleNamed m_true;
	SimpleNamed m_false;
};

static BoolIdentifierSet s_boolIdentifierSet;

String BoolLiteral::defineHtml() const
{
	return L"<^><span class=\"keyword\">" + type()->typeHtml(m_value ? L"true" : L"false") + L"</span>";
}

}
