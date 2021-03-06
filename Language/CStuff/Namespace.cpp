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

#include <msHash.h>
using MarttaSupport::MultiHash;

#include "TextLabel.h"
#include "TopLevelType.h"
#include "Namespace.h"

namespace Martta
{

MARTTA_PROPER_CPP(Namespace);
MARTTA_REGISTER_CSS(Namespace, ".Namespace-label { color: #000; font-weight: bold; }");

bool Namespace::keyPressed(KeyEvent const* _e)
{
	if (attemptAppend(_e))
		return true;
	else
		return Super::keyPressed(_e);
}

String Namespace::finalCode() const
{
	String ic = interfaceCode();
	if (ic.isEmpty())
		return String::null;
	return ic + L"\n" + implementationCode() + L"\n";
}

String Namespace::defineHtml() const
{
	return tagOf(L"keyword", L"namespace") + L" " + toHtml(child(Identity)) + tagOf(L"minor symbol", L"{", L"div") + toHtml(cardinalChildren(), "", "div") + tagOf(L"minor symbol", L"}", L"div");
}

Kinds Namespace::allowedKinds(int _i) const
{
	if (_i >= 0)
		return Kind::of<TopLevelType>();
	return Super::allowedKinds(_i);
}

String Namespace::interfaceCode() const
{
	String ret;
	ret += "namespace " + codeName() + "\n{\n";

	List<Declaration*> q;
	MultiHash<Declaration*, Declaration*> deps;
	foreach (Declaration* i, cardinalChildrenOf<Declaration>())
	{
		foreach (Declaration* j, i->utilisedSiblings())
			deps.insert(i, j);
		if (!deps.count(i))
			q << i;
	}

	List<Declaration*> ds = solve(q, deps);
	if (!ds.size())
		return String::null;
	foreach (Declaration* i, ds)
		ret += i->interfaceCode() + "\n";

	if (ret.endsWith("\n\n"))
		ret.chop(1);
	ret += "};\n";
	return ret;
}

String Namespace::implementationCode() const
{
	String ret;
	List<Declaration*> ordered;
	foreach (Declaration* i, cardinalChildrenOf<Declaration>())
		ret += i->implementationCode() + "\n";
	if (ret.endsWith("\n\n")) ret.chop(1);
	return ret;
}

}
