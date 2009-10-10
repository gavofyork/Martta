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

#include "Concept.h"
#include "WebInformer.h"

namespace Martta
{

MARTTA_NOTION_CPP(WebInformer);

String WebInformer::compileKinds(Kinds const& _t)
{
	String ret;
	foreach (Kind k, _t)
		ret += k.name() + L", ";
	if (ret.endsWith(L", "))
		ret.chop(2);
	return ret;
}

String WebInformer::basicInformationHtml(Concept const* _e)
{
	String ret = L"<h1>" + _e->kind().name() + L"</h1>";
	{
		Pairs p(L"Basic");
		p << "Allowed" << compileKinds(_e->parent()->allowedKinds(_e->index()));
		if (_e->parent()->deniedKinds(_e->index()).count())
			p << "Denied" << compileKinds(_e->parent()->deniedKinds(_e->index()));
		ret += p;
	}
	List<int> an;
	foreach (int i, _e->knownNames())
		if (_e->allowedKinds(i).count())
			an.append(i);
	if (an.count() || _e->minRequired() || _e->cardinalChildCount())
	{
		Pairs p(L"Children");
		foreach (int i, an)
			p << _e->indexName(i) << (compileKinds(_e->allowedKinds(i)) + L"%1: <b>%2</b>").arg(_e->minRequired(i) ? L" (req'd)" : L"").arg(_e->child(i) ? _e->child(i)->kind().name() : L"n/a");
		for (int i = 0; i < max(_e->minRequired(), _e->cardinalChildCount()); ++i)
			p << String::number(i) << (compileKinds(_e->allowedKinds(i)) + L"%1: <b>%2</b>").arg(i < _e->minRequired() ? L" (req'd)" : L"").arg(_e->child(i) ? _e->child(i)->kind().name() : L"n/a");
		if (_e->allowedKinds(max(_e->minRequired(), _e->cardinalChildCount())).count())
			p << L"..." << compileKinds(_e->allowedKinds(max(_e->minRequired(), _e->cardinalChildCount())));
		ret += p;
	}
	return ret;
}

String WebInformer::htmlFromTree(String const& _tree)
{
	return _tree;
}

}
