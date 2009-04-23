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

#include "Class.h"
#include "MemberCallable.h"
#include "Type.h"
#include "ExplicitType.h"
#include "Pointer.h"
#include "Const.h"
#include "ThisPointer.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ThisPointer);	

bool ThisPointer::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<ThisPointer>(_p, _e, "T");
}

Type ThisPointer::type() const
{
	if (!hasAncestor<MemberCallable>()) return Type();
	Type t(ExplicitType(ancestor<Class>()));
	if (ancestor<MemberCallable>()->isConst())
		t.topWith(Const());
	return t.topWith(Pointer());
}

QString ThisPointer::defineLayout(ViewKeys&) const
{
	return "^;s" + type()->idColour() + ";'this'";
}

}
