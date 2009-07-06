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

#include "AccessLabel.h"
#include "TopLevelType.h"

namespace Martta
{

class Class: public TopLevelType
{
	MARTTA_OBJECT(TopLevelType)

public:
	enum { Artificials = FirstNamed, EndOfNamed };

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

	List<Declaration*>					members(bool _isConst = false, Access _access = Private) const;
	template<class T> inline List<T*>	membersOf(bool _isConst = false, Access _access = Private) const { return filterEntities<T>(members(_isConst, _access)); }
	
	Access								baseAccess(Class* _c) const;
	
protected:
	virtual Kinds						allowedKinds(int) const;
	virtual String						interfaceCode() const;
	virtual String						implementationCode() const;
	virtual String						defineLayout(ViewKeys const&) const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual List<Declaration*>			utilised() const;
	virtual void						onDependencyChanged(Entity* _e);
	virtual void						apresLoad() { rejigDeps(); checkImplicitConstructors(); }
	virtual void						onDependencyAdded(Entity* _e);
	virtual void						onDependencyRemoved(Entity* _e, int);
	// A child has been added to the class somewhere in the middle, probably. Either way we need to rejig deps to get the new access label tracked.
	virtual void						onDependencySwitched(Entity* _e, Entity*) { onDependencyAdded(_e); }
	virtual void						onChildrenInitialised();
	virtual bool						hasDefaultConstructor() const;
	virtual Types						assignableTypes() const;
	virtual Entity*						isExpander() const;
	
private:
	bool								checkImplicitConstructors();
	void								rejigDeps();
};

}
