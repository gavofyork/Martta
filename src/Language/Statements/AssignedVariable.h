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

#include "BareTyped.h"

namespace Martta
{

class AssignedVariable: public BareTyped
{
	MARTTA_OBJECT(BareTyped)

public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);
	
protected:
	virtual int							minimumRequired() const { return 2; }
	virtual Kinds						allowedKinds(int _index) const;
	virtual Types						allowedTypes(int _index) const;
	virtual Type						type() const;
	virtual QString						code() const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual QString						defineLayout(ViewKeys&) const;
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); }
	virtual bool						usurpsChild(Entity* _e) const { return _e == entity(0); }
};

}
