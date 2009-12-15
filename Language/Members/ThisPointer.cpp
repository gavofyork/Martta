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
#include "Type.h"
#include "MemberLambda.h"
#include "ThisPointer.h"

namespace Martta
{

MARTTA_PROPER_CPP(ThisPointer);

class ThisPointerSet: public IdentifierSet
{
public:
	ThisPointerSet(): m_ourNamed(L"this") {}
	virtual String						setId() const { return L"Martta::ThisPointer"; }
	virtual List<Named*>				identifiableAt(Position const& _p)
	{
		if (_p.exists() && _p->hasAncestor<MemberLambda>())
		{
			m_lastType = _p->ancestor<MemberLambda>()->thisType();
			return List<Named*>() << &m_ourNamed;
		}
		return List<Named*>();
	}
	virtual void						acceptAt(Position const& _pos, Named*, CodeScene* _cs)
	{
		_cs->setCurrent(_pos.place(new ThisPointer));
	}
	virtual String						defineEditHtml(Named*, String const& _mid)
	{
		return L"<span class=\"keyword\">" + m_lastType->typeHtml(_mid) + L"</span>";
	}
	Type m_lastType;
	SimpleNamed m_ourNamed;
};

static ThisPointerSet s_thisPointerSet;

Type ThisPointer::type() const
{
	if (!hasAncestor<MemberLambda>())
		return Type();
	return ancestor<MemberLambda>()->thisType();
}

}
