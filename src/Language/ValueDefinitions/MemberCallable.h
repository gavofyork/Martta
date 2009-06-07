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

#include "Variable.h"
#include "Callable.h"

namespace Martta
{

/// Base class for all callables that can contain ThisPointers.
/// @note on deriving it. It assumes that there is precisely one child of type AccessLabel, and this is the object's access
/// characteristics. If you're not going to provide that child, then make sure you redefine all of the functions that require
/// knowledge of the AccessLabel.
class MemberCallable: public Callable
{
	MARTTA_PLACEHOLDER(Callable)

public:
	virtual Access						access() const;
	virtual bool						isConst() const;
	
protected:
	virtual Type						type() const;
	virtual QString 					code(FunctionCodeScope _ref) const;
	virtual QString						interfaceCode() const;
	virtual QString						memberCode(FunctionCodeScope _ref) const { return Super::code(_ref); }
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual int							firstArgumentIndex() const { return 5; }
	virtual Kinds						allowedKinds(int _i) const;
	virtual Kinds						deniedKinds(int) const { return Kinds(); }
	virtual void						decorate(DecorationContext const& _p) const;
	virtual QString						defineLayout(ViewKeys&) const;
	virtual QString						defineMemberLayout(ViewKeys&) const;
	virtual QList<DeclarationEntity*>	utilised() const;
};

}
