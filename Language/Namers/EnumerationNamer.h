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
#include "ValueDefiner.h"

namespace Martta
{

class EnumerationNamer: public_interface TypeDefinition
{
	MARTTA_INTERFACE
	MARTTA_INHERITS(TypeDefinition, 0)
	
public:
	void								updateStem();
	virtual void						setUnnamed();
	virtual void						setNamed();
	bool								isNamed() const;

protected:
	virtual bool						hasDefaultConstructor() const { return true; }
	
	String								defineLayout(ViewKeys const& _k) const;
	String								interfaceCode() const;
	bool								keyPressed(KeyEvent const* _e);
	List<ValueDefiner*>					valuesAdded() const { return self()->cardinalChildrenOf<ValueDefiner>(); }
	
	void								onDependencyAdded(Entity* _e);
	void								onDependencyChanged(Entity* _e);
	void								onDependencyRemoved(Entity* _e);
	Entity*								isExpander() const;
	
	String								m_stem;
};

}
