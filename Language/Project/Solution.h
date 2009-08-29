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

#pragma once

#include <msString.h>
#include <msList.h>
using namespace MarttaSupport;

#include "Meta.h"

#ifndef M_API_Project
#define M_API_Project M_OUTAPI
#endif

namespace Martta
{

class Project;

class M_API_Project Solution
{
	MARTTA_INTERFACE

public:
	virtual ~Solution() {}

	String const&						supportPath() const { return m_supportPath; }
	void								setSupportPath(String const& _p) { m_supportPath = _p; }

	virtual void						initialiseNew() {}
	virtual void						addProject(Project*) {}
	virtual void						initWithProjects(List<Project*> const& = List<Project*>()) {}
	virtual void						removeProject(Project*) {}

protected:
	String								m_supportPath;
};

}
