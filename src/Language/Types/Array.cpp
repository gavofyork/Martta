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

#include "Typed.h"
#include "UndefinedArray.h"
#include "Array.h"
#include "Type.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Array);

TypeEntity* Array::newClone() const
{
	return new UndefinedArray;
}
	
Kinds Array::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<TypeEntity>();
	if (_i == 1)
		return Kind::of<Typed>();
	return Kinds();
}

Types Array::allowedTypes(int _i) const
{
	if (_i == 1)
		return Type(Int);
	else
		return Types();
}

QList<DeclarationEntity*> Array::utilised() const
{
	if (entityIs<Typed>(1))
		return Super::utilised() + entityAs<Typed>(1)->type()->utilised();
	return Super::utilised();
}

QString Array::code(QString const& _middle) const
{
	return entityAs<TypeEntity>(0)->code(_middle + "[" + entityAs<Typed>(1)->code() + "]");
}

}
