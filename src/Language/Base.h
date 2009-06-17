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

#include "Entity.h"

namespace Martta
{

class Class;
	
class Base: public Entity
{
	MARTTA_OBJECT(Entity)
	enum { Accessibility = FirstNamed, Superclass, EndOfNamed };
	
public:
	Access								access() const;
	QString								code() const;
	Class*								classType() const;
	
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);
	
protected:
	virtual QString						defineLayout(ViewKeys&) const;
	virtual Kinds						allowedKinds(int _i) const;
	virtual int							minimumRequiredNamed(int _i) const { return _i == Accessibility || _i == Superclass ? 1 : Super::minimumRequiredNamed(_i); }
	virtual bool						isChildInValidState(int _i) const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);
};

}
