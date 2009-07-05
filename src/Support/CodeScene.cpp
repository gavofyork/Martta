/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "Position.h"
#include "Entity.h"
#include "EditDelegate.h"
#include "CodeScene.h"

namespace Martta
{

List<CodeScene*> CodeScene::s_allScenes;
	
CodeScene::CodeScene():
	m_editDelegate	(0)
{
	s_allScenes.append(this);
}

CodeScene::~CodeScene()
{
	s_allScenes.removeAll(this);
}

void CodeScene::notifyOfStrobe(Entity* _child, Entity* _creation)
{
	_child->notifyOfStrobe(_creation);
}

Position CodeScene::nearestBracket(Position const& _p) const
{
	int n = UndefinedIndex;
	Position ret;
	foreach (Position i, m_bracketed)
		if (_p == i)
		{
			n = 0;
			return i;
		}
		else if (int d = _p->hasAncestor(i.entity()))
		{
			if (d < n)
			{
				n = d;
				ret = i;
			}
		}
	return ret;
}

}
