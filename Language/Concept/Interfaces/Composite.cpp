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

#include "Composite.h"

namespace Martta
{

MARTTA_NOTION_CPP(Composite);

void Composite::compose()
{
	killAndDelete();
}

Concept* copyTree(Concept const* _original, Position const& _position = Nowhere)
{
	// Hidden in this file because it won't work generally - if the _original has a ModelPtr in one of its
	// nodes but doesn't have the accompanying Identifiable it won't know to tell the Identifiable to apresLoad()
	// itself (i.e. call loadFinished()) after copying.
	// XXX: This should fsck up now with a program involving printf.
	// FIX: Call copyTree for the entire Program, not per-module and have a specialised copyTree() for in-model copying.

	Concept* ret = _original->kind().spawn();
	if (_position != Nowhere)
		ret->silentMove(_position);
	ret->copyProperties(_original);
	foreach (Concept* c, _original->children())
		copyTree(c, ret->middle(c->index()));
	if (_position == Nowhere)
	{
		ret->loadFinished();
		_original->root()->apresLoad();	// TODO: replace a postCopy() const method that calls restorePtrs
	}
	return ret;
}

Concept* Composite::composeTree(Concept const* _original)
{
	Concept* ret = copyTree(_original);

	for (int composed = 1; composed;)
	{
		composed = 0;
		List<Concept*> cs = ret->children();
		while (cs.size())
		{
			Concept* c = cs.takeLast();
			if (Composite* m = c->tryKind<Composite>())
			{
				composed++;
				m->compose();
			}
			else
				cs << c->children();
		}
	}
	return ret;
}

}
