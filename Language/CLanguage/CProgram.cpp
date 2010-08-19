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
#include "CModule.h"
#include "CProgram.h"

namespace Martta
{

MARTTA_PROPER_CPP(CProgram);

Kinds CProgram::allowedKinds(int _i) const
{
	if (_i >= 0)
		return Kind::of<CModule>();
	return Super::allowedKinds(_i);
}

String CProgram::includeCode() const
{
	String ret;
	foreach (CModule* p, cardinalChildrenAs<CModule>())
		ret += p->includeCode();
	return ret;
}

void CProgram::initialiseNew()
{
	clearChildren();
#ifndef M_WIN
	back().place(Concept::evaluate(String("CModule{TextLabel[text=project]}{MainFunction{TextLabel[text=main]}{BuiltinType[id=%1]}{Argument{BuiltinType[id=%1]}{TextLabel[text=argc]}}{Argument{Pointer{Pointer{BuiltinType[id=%2]}}}{TextLabel[text=argv]}}{Compound{ReturnStatement{IntegerLiteral[value=0][signed=true]}}}}{CDependency[libs=][includes=/usr/include/stdlib.h*/usr/include/stdio.h][name=Standard C]}").arg(Int).arg(Char)));
#else
	back().place(Concept::evaluate(String("CModule{TextLabel[text=project]}{MainFunction{TextLabel[text=main]}{BuiltinType[id=%1]}{Argument{BuiltinType[id=%1]}{TextLabel[text=argc]}}{Argument{Pointer{Pointer{BuiltinType[id=%2]}}}{TextLabel[text=argv]}}{Compound{ReturnStatement{IntegerLiteral[value=0][signed=true]}}}}{CDependency[libs=][includes=stdlib.h*stdio.h][name=Standard C]}").arg(Int).arg(Char)));
#endif
	rejigIncludes();
}

void CProgram::initWithModules(List<Module*> const& _ps)
{
	foreach (Module* p, _ps)
		p->self()->move(back());

	ChangeMan::get()->sleep();
	GccXml::extractHeaders(qs(includeCode()), GccXml::declarationsHandler(this));
//	ModelPtrRegistrar::get()->restorePtrs(this);
	ChangeMan::get()->wake();
	loadFinished();
}

void CProgram::addModule(Module* _p)
{
	ChangeMan::get()->sleep();
	archiveModel();
	killIncludeds();

	_p->self()->move(back());

	GccXml::extractHeaders(qs(includeCode()), GccXml::declarationsHandler(this));
//	ModelPtrRegistrar::get()->restorePtrs(this);
	ChangeMan::get()->wake();
//	apresLoad(_p);
	loadFinished();
}

void CProgram::removeModule(Module* _p)
{
	ChangeMan::get()->sleep();
	archiveModel();
	killIncludeds();

	_p->self()->killAndDeleteWithNotification();

	GccXml::extractHeaders(qs(includeCode()), GccXml::declarationsHandler(this));
//	ModelPtrRegistrar::get()->restorePtrs(this);
	ChangeMan::get()->wake();
	loadFinished();
}

void CProgram::archiveModel()
{
	// Archive all model pointers
	List<Concept*> es = cardinalChildren();
	while (!es.isEmpty())
	{
		es.last()->archive();
		es << es.takeLast()->children();
	}
}

void CProgram::killIncludeds()
{
	// Kill old declarations.
	// May fsck up in here? (Had an alteringDepends flag protecting it originally.)
	foreach (Concept* e, children(Included))
		e->killAndDelete();
}

void CProgram::apresLoad(Module* _p)
{
	List<SafePointer<Concept> > uplist;
	if (_p)
		uplist << _p->self();
	else
		foreach (Module* p, cardinalChildrenAs<Module>())
			uplist << p->self();
	while (uplist.size())
		if (Concept* e = uplist.takeLast())
		{
			foreach (Concept* i, e->children())
				uplist << i;
			e->apresLoad();
		}
}

void CProgram::rejigIncludes()
{
	archiveModel();
	killIncludeds();
	// Import declarations.
	GccXml::extractHeaders(qs(includeCode()), GccXml::declarationsHandler(this));
	// De-archive pointers;
//	ModelPtrRegistrar::get()->restorePtrs(this);
	loadFinished();
}

}
