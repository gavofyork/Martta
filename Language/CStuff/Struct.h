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

#include "ConceptSupport.h"
#include "TopLevelType.h"

#ifndef M_API_CStuff
#define M_API_CStuff M_OUTAPI
#endif

namespace Martta
{

class StructResolver;

class M_CLASS M_API_CStuff Struct: public_super TopLevelType
{
	MARTTA_PROPER(TopLevelType)

	friend class Concept;
	friend class StructResolver;
	template<class T> friend class WithFieldsSimpleResolver;

public:
	virtual int							minRequired(int _i) const { return _i == Cardinals ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int) const;

	virtual String						code() const { return isHidden() ? "[struct of " + parentAs<Declaration>()->codeName() + "]" : (/*"struct " + */codeName()); }
	virtual void						apresLoad() { TopLevelType::apresLoad(); }
};

}
