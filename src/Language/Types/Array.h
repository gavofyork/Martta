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

#include "AddressType.h"

namespace Martta
{

class Array: public AddressType
{
	MARTTA_OBJECT(AddressType)

public:
	inline static bool					keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e) { return simpleInsertionPointKeyPressHandler<Array>(_p, _e, "["); }
	
protected:
	virtual bool						hasDefaultConstructor() const { return true; }
	virtual QString						code(QString const& _middle) const;
	virtual int							minimumRequired() const { return 2; }
	virtual Kinds						allowedKinds(int _i) const;
	virtual Types						allowedTypes(int _i) const;
	virtual TypeEntity*					newClone() const;
	virtual QString						defineLayout(ViewKeys&) const { return "ycode;0;^;'[';1;']'"; }
	virtual QList<DeclarationEntity*>	utilised() const;
};

}
