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

#include "ReferencedTypeMutatorRegistrar.h"
#include "ReferencedTypeMutator.h"
#include "Labelled.h"
#include "CodeScene.h"
#include "EditDelegate.h"
#include "CompletionDelegate.h"
#include "Referenced.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(Referenced);	

Referenced::Referenced(ValueDefiner* _v):
	m_subject	(0)
{
	setSubject(_v);
}

bool Referenced::isSuperfluous() const
{
	return !m_subject.isUsable() || Super::isSuperfluous();
}

String Referenced::code() const
{
	if (m_subject.isUsable())
		return m_subject->reference();
	else
		return String::null;
}

bool Referenced::isInValidState() const
{
	// If we're not referencing anything yet, return null.
	if (!m_subject.isUsable())
		return false;
	return Super::isInValidState();
}

Type Referenced::type() const
{
	// If we're not referencing anything yet, return null.
	if (!m_subject.isUsable())
		return Type();
	return m_subject->type();
}

String Referenced::defineLayout(ViewKeys const& _k) const
{
	String ret = L"^;s" + (m_subject ? m_subject->type()->idColour() : TypeEntity::null->idColour()).name() + L";c;'" + (m_subject ? m_subject->name() : String()) + L"'";
	if (m_subject)
		ret = m_subject->tryKind<Labelled>()->labelLayout(ret, _k);
	return ret;
}

String Referenced::defineEditLayout(ViewKeys const& _k, ValueDefiner* _v)
{
	String ret = L"^;s" + (_v ? _v->type()->idColour() : TypeEntity::null->idColour()).name() + L";c;%1";
	if (_v)
		ret = _v->tryKind<Labelled>()->labelLayout(ret, _k);
	return ret;
}

}
