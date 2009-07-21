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

#include "ModelPtr.h"
#include "Project.h"
#include "Namespace.h"

namespace Martta
{

class CProject;

class CProjectDependency: public Entity
{
	MARTTA_OBJECT(Entity)

private:
	ModelPtr<CProject> m_subject;
};

class CProject: public Namespace, public_interface Project
{
	MARTTA_OBJECT(Namespace)
	MARTTA_INHERITS(Project, 0)

public:
	enum { RequiredIncludes = FirstNamed, MainFunction, EndOfNamed };

	CProject();
	virtual ~CProject();

	virtual String						target() const { return m_tempPath + "/" + targetName(); }
	virtual String						targetName() const;

	virtual String						includeCode() const;
	virtual StringList					libs() const;

	virtual List<StringList>			buildCommands() const;

protected:
	virtual int							minRequired(int _i) const { return _i == 0 ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;

private:
	String								finalCode() const;

	String								m_tempPath;
	String								m_supportPath;
	String								m_tempBatName;
};

}
