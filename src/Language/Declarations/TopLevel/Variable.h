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

#include "VariableNamer.h"
#include "TopLevel.h"

namespace Martta
{

class VariableResolver;

class Variable: public TopLevel, public_interface VariableNamer
{
	MARTTA_OBJECT(TopLevel)
	MARTTA_INHERITS(VariableNamer, 0)

	friend class VariableResolver;

public:
	Qualifiers							qualifiers() const { return m_qualifiers; }
	
protected:
	virtual QString						defineLayout(ViewKeys& _k) const { return "^;" + VariableNamer::defineLayout(_k); }
	virtual QList<DeclarationEntity*>	utilised() const { return actualType()->utilised(); }
	
	virtual bool						keyPressed(EntityKeyEvent const* _e) { return VariableNamer::keyPressed(_e) ? true : Super::keyPressed(_e); }
	virtual int							minimumRequiredNamed(int _i) const { return _i == OurType ? 1 : Super::minimumRequiredNamed(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); } 
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual QString						interfaceCode() const { return Martta::code(m_qualifiers & VariableMask) + VariableNamer::interfaceCode(); }
	virtual QString						implementationCode() const { return VariableNamer::implementationCode(); }
	
private:
	Qualifiers							m_qualifiers;
	Location							m_location;
};

}
