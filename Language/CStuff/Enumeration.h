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

#include "Type.h"
#include "EnumerationNamer.h"
#include "TopLevelType.h"

#ifndef M_API_CStuff
#define M_API_CStuff M_OUTAPI
#endif

namespace Martta
{

class EnumValue;
class EnumerationResolver;

class M_CLASS M_API_CStuff Enumeration: public_super TopLevelType, public_interface EnumerationNamer
{
	MARTTA_PROPER(TopLevelType)
	MARTTA_ALSO_INHERITS(EnumerationNamer, 0)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

	virtual String						code() const { return isNamed() ? codeName() : "enum ["+m_stem+"*]"; }

protected:
	virtual int							minRequired(int _i) const { return _i == Cardinals ? 1 : _i == Identity ? 0 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual String						defineHtml() const { return "<^>" + defineEnumerationHtml(); }
	virtual String						interfaceCode() const { return EnumerationNamer::interfaceCode(); }
	virtual bool						hasDefaultConstructor() const { return EnumerationNamer::hasDefaultConstructor(); }
	virtual List<ValueDefiner*>			valuesAdded() const { return EnumerationNamer::valuesAdded(); }
	virtual Types						assignableTypes() const { return Type(const_cast<Enumeration*>(this)); }

	virtual bool						keyPressed(KeyEvent const* _e) { AssertNR(isComplete()); return EnumerationNamer::keyPressed(_e) ? true : Super::keyPressed(_e); }

	virtual int							familyDependencies() { return DependsOnChildren; }
	virtual void						onDependencyAdded(Concept* _e) { EnumerationNamer::onDependencyAdded(_e); }
	virtual void						onDependencyChanged(int _a, Concept* _e) { EnumerationNamer::onDependencyChanged(_a, _e); }
	virtual void						onDependencyRemoved(Concept* _e, int) { EnumerationNamer::onDependencyRemoved(_e); }
};

}
