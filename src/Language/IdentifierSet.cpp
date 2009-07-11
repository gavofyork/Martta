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

#include "IdentifierSetRegistrar.h"
#include "IdentifierSet.h"

namespace Martta
{

IdentifierSet::IdentifierSet()
{
	IdentifierSetRegistrar::get()->registerSet(this);
}

IdentifierSet::~IdentifierSet()
{
	IdentifierSetRegistrar::get()->unregisterSet(this);
}

/*		Entity* e = 0;
		if (m_subject->over().allowedToBeKind<DefaultConstructedVariable>())
			e = new DefaultConstructedVariable;
		else if (m_subject->over().allowedToBeKind<AssignedVariable>())
			e = new AssignedVariable;
		if (e)
		{
			e->prepareChildren();
			int x;
			if ((x = BuiltinType::id(m_entityName)) != -1)
			{
				e->childOf<TypeEntity>()->replace(new BuiltinType(x));
				codeScene()->silentlySetCurrent(e->child(Identifiable::Identity));	// set to the place where the user expects the cursor to be (silently, sicne we might (possibly) already be in a setCurrent!).
			}
			else if (m_entityName == "string")
			{
				e->childOf<TypeEntity>()->replace(new StringType);
				codeScene()->silentlySetCurrent(e->child(Identifiable::Identity));	// set to the place where the user expects the cursor to be (silently, sicne we might (possibly) already be in a setCurrent!).
			}
			else
			{
				AssertNR(e->childIs<TextLabel>(Identifiable::Identity));
				e->childAs<TextLabel>(Identifiable::Identity)->setText(m_entityName);
				codeScene()->silentlySetCurrent(e->childOf<TypeEntity>());							// set to the place where the user expects the cursor to be (silently, sicne we might (possibly) already be in a setCurrent!).
			}
			
			// set subject to zero so we don't go through this again when the kill()ed subject gets removed from the scene.
			Entity* s = m_subject;
			m_subject = 0;
			s->replace(e);
			// We have now been deleted.
		}*/


}
