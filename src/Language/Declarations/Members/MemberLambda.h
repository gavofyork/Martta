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
#include "LambdaNamer.h"
#include "Member.h"

namespace Martta
{

// Just for non-static callables

class MemberLambda: public Member, public_interface LambdaNamer
{
	MARTTA_PLACEHOLDER(Member)
	MARTTA_INHERITS(LambdaNamer, 0)
	
public:
	virtual bool						isConst() const;
	
protected:
	virtual Type						type() const;
	
	virtual QString						memberImplementationCode() const { return LambdaNamer::implementationCode(); }
	virtual QString						memberInterfaceCode() const { return LambdaNamer::interfaceCode(); }

	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); }
	virtual void						onDependencyRemoved(Entity*) { changed(); }
	virtual QList<DeclarationEntity*>	utilised() const;
	
	virtual QString						basicCode(FunctionCodeScope _ref) const;

	virtual Entity*						isExpander() const;
	virtual QString						memberDefineLayout(ViewKeys& _v) const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual void						memberDecorate(DecorationContext const& _p) const;
};

}
