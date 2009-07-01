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

#include "TopLevelType.h"

namespace Martta
{

class StructResolver;

class Struct: public TopLevelType
{
	MARTTA_OBJECT(TopLevelType)

	friend class Entity;
	friend class StructResolver;
	template<class T> friend class WithFieldsSimpleResolver;

public:
	virtual int							minRequired(int _i) const { return _i == Cardinals ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int) const;
	
	virtual QString						code() const { return isHidden() ? "[struct of " + parentAs<Declaration>()->codeName() + "]" : (/*"struct " + */codeName()); }
};

}
