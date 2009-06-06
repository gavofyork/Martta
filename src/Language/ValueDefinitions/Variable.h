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

#include "ValueDefiner.h"
#include "DeclarationEntity.h"

namespace Martta
{

class VariableResolver;

class Variable: public DeclarationEntity, public_interface ValueDefiner
{
	MARTTA_OBJECT(DeclarationEntity)
	MARTTA_INHERITS(ValueDefiner, 0)

	friend class VariableResolver;

public:
	bool 								isHidden() const { return codeName().startsWith("."); }
	TypeEntity*							actualType() const;
	Qualifiers							qualifiers() const { return m_qualifiers; }
	virtual QString						code() const;
	
protected:
	virtual Type						type() const;
	virtual bool						isChildInValidState(int _i) const;
	virtual int							minimumRequired() const { return 2; }
	virtual Kinds						allowedKinds(int _i) const;
	virtual QString						interfaceCode() const;
	virtual QString						implementationCode() const;
	virtual QString						defineLayout(ViewKeys&) const;
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual QList<DeclarationEntity*>	utilised() const;
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); } 
	
private:
	Qualifiers							m_qualifiers;
	Location							m_location;
};

}
