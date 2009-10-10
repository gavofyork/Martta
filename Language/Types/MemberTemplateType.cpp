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

#include "Memberify.h"
#include "MemberTemplateType.h"

namespace Martta
{

MARTTA_OBJECT_CPP(MemberTemplateType);

void MemberTemplateType::rejig()
{
	if (hasAncestor<Memberify>())
	{
		Memberify* m = ancestor<Memberify>();
		if (m->scopeType()->childIs<TypeConcept>(m_index) && !m->scopeType()->childAs<TypeConcept>(m_index)->isNull())
		{
			replace(cloneOf(m->scopeType()->childAs<TypeConcept>(m_index), owner()));
			return;
		}
	}
}

bool MemberTemplateType::defineSimilarityFrom(TypeConcept const* _f, Castability _c) const
{
	if (hasAncestor<Memberify>())
	{
		Memberify* m = ancestor<Memberify>();
		if (m->scopeType()->childIs<TypeConcept>(m_index))
		{
			if (m->scopeType()->childAs<TypeConcept>(m_index)->isNull())
			{
				// Redefine the template type if it hasn't been already
				m->scopeType()->childAs<TypeConcept>(m_index)->replace(cloneOf(_f, m->scopeType()->childAs<MemberTemplateType>(m_index)->owner()));
				return true;
			}
			else if (_f->isSimilarTo(m->scopeType()->childAs<TypeConcept>(m_index), _c))
			{
				// Otherwise compare on it (we wouldn't normally have to do this because of substitution, but the above
				// can't really substitute i nthe middle of the a similarity check!
				return true;
			}
		}
	}
	else
	{
		// ERROR CONDITION. 
		// Let it through for now; it's a known bug.
		return true;
	}
	
	return Super::defineSimilarityFrom(_f, _c);
}

bool MemberTemplateType::defineEquivalenceFrom(TypeConcept const* _f) const
{
	if (hasAncestor<Memberify>())
	{
		Memberify* m = ancestor<Memberify>();
		if (m->scopeType()->childIs<TypeConcept>(m_index))
		{
			if (m->scopeType()->childAs<TypeConcept>(m_index)->isNull())
			{
				// Redefine the template type if it hasn't been already
				m->scopeType()->childAs<TypeConcept>(m_index)->replace(cloneOf(_f, m->scopeType()->childAs<MemberTemplateType>(m_index)->owner()));
				return true;
			}
			else if (_f->isEquivalentTo(m->scopeType()->childAs<TypeConcept>(m_index)))
			{
				// Otherwise compare on it (we wouldn't normally have to do this because of substitution, but the above
				// can't really substitute i nthe middle of the a similarity check!
				return true;
			}
		}
	}
	else
	{
		// ERROR CONDITION. 
		// Let it through for now; it's a known bug.
		return true;
	}
	return Super::defineEquivalenceFrom(_f);
}

}
