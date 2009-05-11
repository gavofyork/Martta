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

#include "ValueDefinition.h"

namespace Martta
{

class Enumeration;

class EnumValue: public ValueDefinition
{
	MARTTA_OBJECT(ValueDefinition)

	friend class Entity;
	friend class EnumValueResolver;

public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);

	// Accessor methods.
	// From ValueDefinition from TypeNamer.
	virtual Type						type() const { return Type(Int); }	// TODO Not true - only when it's an anonymous enum.
	virtual QString						code() const { return codeName(); }

protected:
	virtual QString						defineLayout(ViewKeys&) const;
	virtual int							minimumRequired() const { return 1; }
	virtual Kinds						allowedKinds(int) const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual bool						isChildInValidState(int _i) const;

	virtual void						nameChanged();
	// TODO: make work with new system.
	virtual void						onContextChanged(Entity* _old, Entity* _new);
};

}
