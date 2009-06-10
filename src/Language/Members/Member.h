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

#include "DeclarationEntity.h"

namespace Martta
{

/**
 * Class for any entity that can be found inside a class (i.e. with an access attribute).
 */
class Member: public DeclarationEntity
{
	MARTTA_PLACEHOLDER(DeclarationEntity)
	
public:
	static const int OffsetForDerivatives = 1;

	virtual Access						access() const;
	
protected:
	// New virtuals.
	virtual int							memberMinimumRequired() const { return 1; }
	virtual Kinds						memberAllowedKinds(int _i) const { if (_i == 0) return Kind::of<DeclarationEntity>(); else return Kinds(); }
	virtual QString						memberInterfaceCode() const { return entityAs<DeclarationEntity>(1)->interfaceCode(); }
	virtual QString						memberImplementationCode() const { return entityAs<DeclarationEntity>(1)->implementationCode(); }
	virtual void						memberDecorate(DecorationContext const& _p) const { entity(0)->decorate(_p); }
	virtual QString						memberDefineLayout(ViewKeys&) const { return "1"; }	// don't forget that the numbers are offset! use fromLocal here. 
	
	// Old virtuals.
	virtual int							minimumRequired() const { return OffsetForDerivatives + memberMinimumRequired(); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual QString 					interfaceCode() const;
	virtual QString 					implementationCode() const { return memberImplementationCode(); }
	virtual void						decorate(DecorationContext const& _p) const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); }
	virtual QString						defineLayout(ViewKeys&) const;
};

}
