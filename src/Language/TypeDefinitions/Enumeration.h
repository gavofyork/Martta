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

namespace Martta
{

class EnumValue;
class EnumerationResolver;

class Enumeration: public TypeDefinition
{
	MARTTA_OBJECT(TypeDefinition)

public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);

	// Population methods.
	void								updateStem();

	virtual Access						access() const;
	virtual QString						code() const { return isHidden() ? "enum ["+m_stem+"*]" : codeName(); }
	
protected:
	virtual QString						defineLayout(const ViewKeys&) const;
	virtual int							minimumRequired() const { return 3; }
	virtual Kinds						allowedKinds(int) const;
	virtual QString						interfaceCode() const;
	virtual bool						hasDefaultConstructor() const { return true; }
	virtual Types						assignableTypes() const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual Entity*						isExpander() const { return entity(1); }
	virtual void						decorate(DecorationContext const& _p) const;
	
	virtual int							familyDependencies() { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { updateStem(); }

private:
	QString								m_stem;
};

}
