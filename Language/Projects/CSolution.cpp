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

#include "GccXml.h"

#include "ModelPtrRegistrar.h"

#include "DeclarationsHandler.h"
#include "CTypes.h"
#include "CDependency.h"
#include "CProject.h"
#include "CSolution.h"

namespace Martta
{

MARTTA_OBJECT_CPP(CSolution);

Kinds CSolution::allowedKinds(int _i) const
{
	if (_i >= 0)
		return Kind::of<CProject>();
	return Super::allowedKinds(_i);
}

String CSolution::includeCode() const
{
	String ret;
	foreach (CProject* p, cardinalChildrenAs<CProject>())
		ret += p->includeCode();
	return ret;
}

void CSolution::initialiseNew()
{
	clearEntities();
	back().place(Entity::evaluate(String("CProject{TextLabel[text=project]}{MainFunction{TextLabel[text=main]}{BuiltinType[id=%1]}{Argument{BuiltinType[id=%1]}{TextLabel[text=argc]}}{Argument{Pointer{Pointer{BuiltinType[id=%2]}}}{TextLabel[text=argv]}}{Compound{ReturnStatement{IntegerLiteral[value=0][signed=true]}}}}{CDependency[libs=][includes=/usr/include/stdlib.h*/usr/include/stdio.h][name=Standard C]}").arg(Int).arg(Char)));
	rejigIncludes();
}

void CSolution::initWithProjects(List<Project*> const& _ps)
{
	foreach (Project* p, _ps)
		p->self()->move(back());

	ChangeMan::get()->sleep();
	GccXml::extractHeaders(qs(includeCode()), GccXml::declarationsHandler(this));
	ModelPtrRegistrar::get()->restorePtrs(this);
	ChangeMan::get()->wake();
	apresLoad();
}

void CSolution::addProject(Project* _p)
{
	ChangeMan::get()->sleep();
	archiveModel();
	killIncludeds();

	_p->self()->move(back());

	GccXml::extractHeaders(qs(includeCode()), GccXml::declarationsHandler(this));
	ModelPtrRegistrar::get()->restorePtrs(this);
	ChangeMan::get()->wake();
	apresLoad(_p);
}

void CSolution::removeProject(Project* _p)
{
	ChangeMan::get()->sleep();
	archiveModel();
	killIncludeds();

	_p->self()->killAndDeleteWithNotification();

	GccXml::extractHeaders(qs(includeCode()), GccXml::declarationsHandler(this));
	ModelPtrRegistrar::get()->restorePtrs(this);
	ChangeMan::get()->wake();
}

void CSolution::archiveModel()
{
	// Archive all model pointers
	List<Entity*> es = cardinalChildren();
	while (!es.isEmpty())
	{
		es.last()->archive();
		es << es.takeLast()->children();
	}
}

void CSolution::killIncludeds()
{
	// Kill old declarations.
	// May fsck up in here? (Had an alteringDepends flag protecting it originally.)
	foreach (Entity* e, children(Included))
		e->killAndDelete();
}

void CSolution::apresLoad(Project* _p)
{
	List<SafePointer<Entity> > uplist;
	if (_p)
		uplist << _p->self();
	else
		foreach (Project* p, cardinalChildrenAs<Project>())
			uplist << p->self();
	while (uplist.size())
		if (Entity* e = uplist.takeLast())
		{
			foreach (Entity* i, e->children())
				uplist << i;
			e->apresLoad();
		}
}

void CSolution::rejigIncludes()
{
	archiveModel();
	killIncludeds();
	// Import declarations.
	GccXml::extractHeaders(qs(includeCode()), GccXml::declarationsHandler(this));
	// De-archive pointers;
	ModelPtrRegistrar::get()->restorePtrs(this);
}

}
