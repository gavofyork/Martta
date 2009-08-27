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
#include "Compound.h"
#include "LambdaNamer.h"
#include "AccessLabel.h"
#include "CodeScene.h"
#include "VirtualOverload.h"
#include "CompletionDelegate.h"

namespace Martta
{

MARTTA_OBJECT_CPP(VirtualOverload);

Type VirtualOverload::returns() const
{
	return m_base.isUsable() ? m_base->returns() : Type();
}

Kinds VirtualOverload::allowedKinds(int _i) const
{
	if (_i == Identity || _i == Constness || _i == Returned)
		return Kinds();
	return Super::allowedKinds(_i);
}

bool VirtualOverload::keyPressed(KeyEvent const* _e)
{
	if (_e->text().length() == 1 && _e->text()[0].isLower())
	{
		_e->codeScene()->setEditing(this);
		if (_e->codeScene()->isEditing(this))
			_e->reinterpretLater();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

List<VirtualMethod*> VirtualOverload::possibilities() const
{
	List<VirtualMethod*> ret;
	foreach (VirtualMethod* i, parentAs<Class>()->membersOf<VirtualMethod>())
		if (i->parent() != parent())
			ret << i;
	return ret;
}
/*
String VirtualOverload::defineEditLayout(ViewKeys const& _viewKeys, VirtualMethod*) const
{
	// having the margin here is horrible, but it'll do for now
	return "m24,0,0,0;^;ycode;'virtual';Mo;>name;ynormal;%1;Mo" + String(_viewKeys["expanded"].toBool() ? String(body()->cardinalChildCount() ? ";n;i;%1" : ";%1").arg(Body) : "");
}
*/
String VirtualOverload::defineEditHtml(CodeScene*) const
{
	return String::null;
}

EditDelegateFace* VirtualOverload::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<VirtualOverload, VirtualMethod*>(this, _s);
}

}
