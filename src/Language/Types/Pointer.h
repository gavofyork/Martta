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

#include <QColor>

#include "AddressType.h"

namespace Martta
{

class Pointer: public AddressType
{
	MARTTA_OBJECT(AddressType)

public:
	static bool keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);
	
protected:
	virtual Types						assignableTypes() const;
	virtual bool						hasDefaultConstructor() const { return true; }
	virtual QString						code(QString const& _middle) const { return child()->code("*" + _middle); }
	virtual QString						modifierLayout() const { return "ycode;'*'"; }
	virtual QString						idColour() const { return QColor(child() ? child()->idColour() : TypeEntity::null->idColour()).darker(120).name(); }
	virtual TypeEntity*					newClone() const { return new Pointer; }
};

}
