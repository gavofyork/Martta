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

#include "Compound.h"
#include "Variable.h"
#include "LambdaNamer.h"
#include "DeclarationEntity.h"

namespace Martta
{

class FunctionResolver;

class Function: public TopLevel, public_interface LambdaNamer
{
	MARTTA_OBJECT(TopLevel)
	MARTTA_INHERITS(LambdaNamer, 0)
	
	friend class FunctionResolver;
	
public:
	Function(): m_qualifiers(NoQualifiers), m_ellipsis(false) {}
	
	virtual bool						ellipsis() const { return m_ellipsis; }
	void								setEllipsis(bool _e) { m_ellipsis = _e; changed(); }
	
protected:
	virtual int							minimumRequired() const { return 2; }
	virtual Kinds						allowedKinds(int _i) const;
	
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); }
	virtual void						onDependencyRemoved(Entity*, int) { changed(); }
	
	virtual Entity*						isExpander() const { return body()->entity(0); }
	virtual QString						defineLayout(ViewKeys& _v) const { return "^;" + LambdaNamer::defineLayout(_v); }
	virtual bool						keyPressed(EntityKeyEvent const* _e) { return LambdaNamer::keyPressed(_e) ? true : Super::keyPressed(_e); }
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	
private:
	Location							m_location;
	Qualifiers							m_qualifiers;
	bool								m_ellipsis;
};

}
