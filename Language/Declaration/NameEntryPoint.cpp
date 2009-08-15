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
#include "CodeScene.h"
#include "Named.h"
#include "Entity.h"
#include "NameEntryPoint.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(NameEntryPoint);

List<Named*> NameEntryPoint::possibilities()
{
	List<Named*> ret;
	foreach (IdentifierSet* i, IdentifierSetRegistrar::get()->allSets())
		ret << i->identifiableAt(self()->over());
	return ret;
}

String NameEntryPoint::defineEditHtml(CodeScene* _cs) const
{
	if (EditDelegateFace* d = self()->editDelegate(_cs))
	{
		String ret = d->real() + L"<span class=\"unreal\">" + d->unreal() + L"</span>";
		Named* n = static_cast<CompletionDelegate<Entity, Named*>*>(d)->selection();
		if (n)
			foreach (IdentifierSet* i, IdentifierSetRegistrar::get()->allSets())
				if (i->identifiableAt(self()->over()).contains(n))
					return i->defineEditHtml(n, ret);
		return ret;
	}
	return String::null;
}

void NameEntryPoint::committed(Named* _i)
{
	if (_i)
		foreach (IdentifierSet* i, IdentifierSetRegistrar::get()->allSets())
			if (i->identifiableAt(self()->over()).contains(_i))
			{
				Position p = self()->over();
				i->acceptAt(self()->over(), _i);
				// NOTE: We may not exist at this point!
				return;
			}
}

bool NameEntryPoint::keyPressed(KeyEvent const* _e)
{
	if (self()->isPlaceholder() && _e->text().length() == 1 && _e->text()[0].isLower())
	{
		self()->setEditing(_e->codeScene());
		if (self()->isEditing(_e->codeScene()))
			_e->reinterpretLater();
		return true;
	}
	return false;
}

}
