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

#include "FunctionType.h"
#include "Memberify.h"
#include "Type.h"
#include "Reference.h"
#include "ModelPtrRegistrar.h"
#include "BuiltinDeclaration.h"

namespace Martta
{

MARTTA_OBJECT_CPP(BuiltinDeclaration);

void BuiltinDeclaration::construct(TypeEntity const* _scope, int _id, bool _isConst, Type const& _returns, Types const& _args, char const* _key)
{
	m_key = _key;
	m_myId = _id;
	
	Type t = FunctionType();
	t.place(_returns, FunctionType::Returned);
	foreach (Type i, _args)
		t.append(i);
	if (_scope)
	{
		Memberify m = Memberify(Type().topWith(*_scope));
		m.setConst(_isConst);
		t.topWith(m);
	}
	t.topWith(Reference());
	t.placeCopy(back());

	ModelPtrRegistrar::get()->registerDeclaration(this);
}

void BuiltinDeclaration::destruct()
{
	ModelPtrRegistrar::get()->unregisterDeclaration(this);
	delete this;
}

Kinds BuiltinDeclaration::allowedKinds(int _i) const
{
	if (_i >= 0)
		return Kind::of<TypeEntity>();
	return Super::allowedKinds(_i);
}

}
