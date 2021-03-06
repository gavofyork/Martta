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
#include "Concept.h"
#include "NameEntryPoint.h"

namespace Martta
{

MARTTA_NOTION_CPP(NameEntryPoint);

List<Named*> NameEntryPoint::possibilities() const
{
	List<Named*> ret;
	foreach (IdentifierSet* i, IdentifierSetRegistrar::get()->allSets())
		ret << i->identifiableAt(over());
	return ret;
}

IdentifierSet* NameEntryPoint::ownerOf(Named* _n) const
{
	foreach (IdentifierSet* i, IdentifierSetRegistrar::get()->allSets())
		if (i->identifiableAt(over()).contains(_n))
			return i;
	return 0;
}

String NameEntryPoint::defineEditHtml(CodeScene* _cs) const
{
	if (isPlaceholder())
		if (EditDelegateFace* d = _cs->editDelegate(this))
		{
			String ret = d->real() + tagOf(L"_ms_unreal", d->unreal());
			if (Named* n = reinterpret_cast<Named*>(d->current()))
				foreach (IdentifierSet* i, IdentifierSetRegistrar::get()->allSets())
					if (i->identifiableAt(over()).contains(n))
						return i->defineEditHtml(n, ret) + tagOf(L"_ms_minor", d->comment());
			return ret + tagOf(L"_ms_minor", d->comment());
		}
	return defineHtml();
}

void NameEntryPoint::committed(Named* _i, CodeScene* _cs)
{
	if (_i)
		foreach (IdentifierSet* i, IdentifierSetRegistrar::get()->allSets())
			if (i->identifiableAt(over()).contains(_i))
			{
				Position p = over();
				i->acceptAt(over(), _i, _cs);	// TODO!!! Actually use...
				// NOTE: We may not exist at this point!
				return;
			}
}

bool NameEntryPoint::keyPressed(KeyEvent const* _e)
{
	if (isPlaceholder() && _e->text().length() == 1 && _e->text()[0].isLower())
	{
		_e->codeScene()->setEditing(this);
		if (_e->codeScene()->isEditing(this))
			_e->reinterpretLater();
		return true;
	}
	return false;
}

}
