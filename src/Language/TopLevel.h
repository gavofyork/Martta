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

#include "TypeDefinition.h"
#include "SubAddressable.h"

namespace Martta
{

class TypeResolver;

class TopLevel: public SubAddressable, public_interface TypeDefinition
{
	MARTTA_PLACEHOLDER(SubAddressable)
	MARTTA_INHERITS(TypeDefinition, 0)

	friend class TypeResolver;
	
public:
	bool								isHidden() const { return codeName().startsWith("."); }				///< true for anonymous enums.
	
protected:
	// From SubAddressable
	virtual int							familyDependencies() const { return DependsOnChildren; }
	
	// From TypeDefinition
	virtual QString						code() const { return codeName(); }
	virtual bool						hasDefaultConstructor() const { return false; }
	virtual Types						assignableTypes() const;
	virtual QList<DeclarationEntity*>	utilised() const { return QList<DeclarationEntity*>() << const_cast<TopLevel*>(this); }
	
	Location							m_location;
};

}
