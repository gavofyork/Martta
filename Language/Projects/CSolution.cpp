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

#include "GccXML.h"

#include "ModelPtrRegistrar.h"

#include "CDependency.h"
#include "CProject.h"
#include "CSolution.h"
#undef inline

namespace Martta
{

MARTTA_OBJECT_CPP(CSolution);

static inline String qs(QString const& _qs)
{
	String ret;
	ret.resize(_qs.length());
	wchar_t* d = ret.data();
	_qs.toWCharArray(d);
	ret.dataChanged(d);
	return ret;
}

static inline QString qs(String const& _s)
{
	return QString::fromWCharArray(_s.data(), _s.length());
}

Kinds CSolution::allowedKinds(int _i) const
{
	if (_i >= 0)
		return Kind::of<CProject>();
	return Super::allowedKinds(_i);
}

void CSolution::initialiseNew()
{
	clearEntities();
	back().place(Entity::evaluate("CProject{TextLabel[text=project]}{Function{TextLabel[text=main]}}{CDependency[libs=][includes=/usr/include/stdlib.h*/usr/include/stdio.h][name=Standard C]}"));
	rejigIncludes();
}

String CSolution::includeCode() const
{
	String ret;
	foreach (CProject* p, cardinalChildrenAs<CProject>())
		ret += p->includeCode();
	return ret;
}



void CSolution::rejigIncludes()
{
	// Archive all model pointers
	List<Entity*> es = cardinalChildren();
	while (!es.isEmpty())
	{
		es.last()->archive();
		es << es.takeLast()->children();
	}

	// Kill old declarations.
	// May fsck up in here? (Had an alteringDepends flag protecting it originally.)
	foreach (Entity* e, children(Included))
		e->killAndDeleteWithNotification();

	// Import declarations.
	GccXml::extractHeaders(qs(includeCode()), GccXml::declarationsHandler(this));

	// De-archive pointers;
	ModelPtrRegistrar::get()->restorePtrs(this);

	// Call apresLoad()
#if 0
	List<SafePointer<Entity> > uplist;
	foreach (Entity* e, cardinalChildren())
		uplist << e;
	while (uplist.size())
		if (Entity* e = uplist.takeLast())
		{
			foreach (Entity* i, e->children())
				uplist << i;
			e->apresLoad();
		}
#endif
}

}
