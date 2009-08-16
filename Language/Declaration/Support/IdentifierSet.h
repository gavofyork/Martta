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

#include <msSupport.h>
#include <msList.h>
using namespace MarttaSupport;

#include "Named.h"

namespace Martta
{

class Position;

// Declare a single static instance of the derived class in its CPP file to register it.
class IdentifierSet
{
public:
	IdentifierSet();
	virtual ~IdentifierSet();

	virtual List<Named*>				identifiableAt(Position const&) { return List<Named*>(); }
	virtual void						acceptAt(Position const&, Named*) {}
	virtual String						defineEditHtml(Named*, String const& _mid) { return _mid; }
};

template<class T>
class SimpleIdentifierSet: public IdentifierSet
{
public:
	SimpleIdentifierSet(wchar_t const* _name, wchar_t const* _preHtml = L"<^span class=\"keyword\">", wchar_t const* _postHtml = L"</span>"):
		m_ourNamed	(_name),
		m_preHtml	(_preHtml),
		m_postHtml	(_postHtml)
	{}
	virtual List<Named*>				identifiableAt(Position const& _p)
	{
		if (_p.allowedToBeKind<T>())
			return List<Named*>() << &m_ourNamed;
		return List<Named*>();
	}
	virtual void						acceptAt(Position const& _pos, Named*)
	{
		_pos.place((new T)->prepareChildren())->dropCursor();
	}
	virtual String						defineEditHtml(Named*, String const& _mid)
	{
		return m_preHtml + _mid + m_postHtml;
	}
	SimpleNamed m_ourNamed;
	String m_preHtml;
	String m_postHtml;
};

}
