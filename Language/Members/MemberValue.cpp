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

#include "Const.h"
#include "Reference.h"
#include "GenericMemberOperation.h"
#include "Memberify.h"
#include "MemberValue.h"
#include "MemberLambda.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(MemberValue);

Type MemberValue::memberifiedType(Type const& _t) const
{
	Type ret = _t;
	if (!typeDefinition() || !ret->isType<Reference>() || !ret->asType<Reference>()->original())
		return Type();
	Memberify* m = new Memberify(typeDefinition(), isConst());
	ret->asType<Reference>()->original()->knitIn(m);
	return ret;
}

bool MemberValue::isAccessibleAt(Position const& _p) const
{
	if (GenericMemberOperation* gmo = _p.parent()->tryKind<GenericMemberOperation>())
		return gmo->scope()->applicableMembers(gmo->parent()).contains(const_cast<MemberValue*>(this));
	if (MemberLambda* ml = _p->ancestor<LambdaNamer>()->tryKind<MemberLambda>())
		return ml->typeDefinition()->applicableMembers(_p.parent(), ml->isConst()).contains(const_cast<MemberValue*>(this));
	return false;
}

}
