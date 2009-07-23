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

#include <msList.h>
using namespace MarttaSupport;

#include "TypeEntity.h"
#include "Identifiable.h"

namespace Martta
{

class ValueDefiner;
class Declaration;
class Type;
typedef List<Type> Types;

/**
 * Class for permanent types.
 * Types may be named or anonymous.
 * This includes typedefs, unions, structs and enums.
 */
class TypeDefinition: public_interface Identifiable
{
	MARTTA_INTERFACE
	MARTTA_INHERITS(Identifiable, 0)

public:
	virtual String						code() const = 0;
	virtual Types						assignableTypes() const = 0;
	virtual List<Declaration*>			utilisedInUse() const = 0;
	virtual bool						hasDefaultConstructor() const = 0;
	virtual bool						hasSingleCastOperator(TypeEntity const* _t, bool _const) const;
	virtual inline bool					hasSingleConversionConstructor(TypeEntity const*) const { return false; }
	virtual bool						defineSimilarityTo(TypeEntity const* _t, TypeEntity::Castability _c) const;
	virtual List<ValueDefiner*>			applicableMembers(Entity const* _s, bool _isConst) const;
	virtual inline bool					canStandAlone() const { return true; }

	virtual ~TypeDefinition() {}
};

}