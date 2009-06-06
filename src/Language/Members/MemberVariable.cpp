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

#include "DecorationContext.h"
#include "CommonGraphics.h"
#include "Reference.h"
#include "Memberify.h"
#include "AccessLabel.h"
#include "Const.h"
#include "MemberVariable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(MemberVariable);

Type MemberVariable::type() const
{
	if (!isComplete())
		return Type();
	Type ret = localEntityAs<TypeNamer>(0)->type();
	if (!ancestor<Class>() || !ret->isType<Reference>() || !ret->asType<Reference>()->child())
		return Type();
	ret->asType<Reference>()->child()->knitIn(new Memberify(ancestor<Class>()));
	return ret;
}

bool MemberVariable::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<MemberVariable>(_p, _e, "A");
}

}
