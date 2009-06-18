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

#include "BinaryOperation.h"

namespace Martta
{

class AssignmentOperation: public BinaryOperation
{
	MARTTA_OBJECT(BinaryOperation)

public:
	inline static bool					keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e) { return simpleKeyPressedOnInsertionPointHandler<AssignmentOperation>(_p, _e, "=", Operator(Operator::Equals).precedence(), Operator(Operator::Equals).associativity()); }

private:
	virtual Types						allowedTypes(int _index) const;
	virtual Type						type() const;
	virtual QString						code() const;
	virtual QString						operatorLayout() const { return "ycode;':='"; }
	virtual Operator					id() const { return Operator::Equals; }
	virtual int							familyDependencies() const { return Super::familyDependencies() | DependsOnChildren; }
	virtual void						onDependencyChanged(Entity* _e) { if (allEntities().contains(_e)) changed(); Super::onDependencyChanged(_e); }
};

}
