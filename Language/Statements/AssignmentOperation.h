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

#ifndef M_API_Statements
#define M_API_Statements M_OUTAPI
#endif

namespace Martta
{

class M_API_Statements AssignmentOperation: public_super BinaryOperation
{
	MARTTA_PROPER(BinaryOperation)

public:
	inline static bool					keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simpleKeyPressedOnPositionHandler<AssignmentOperation>(_p, _e, "=", Operator(Operator::Equals).precedence(), Operator(Operator::Equals).associativity()); }

private:
	virtual Types						allowedTypes(int _index) const;
	virtual Type						type() const;
	virtual String						code() const;
	virtual String						operatorHtml() const { return "<span class=\"symbol\">:=</span>"; }
	virtual Operator					id() const { return Operator::Equals; }
	virtual int							familyDependencies() const { return Super::familyDependencies() | DependsOnChildren; }
	virtual void						onDependencyChanged(int _a, Concept* _e) { if (_e->parent() == this) changed(Logically); Super::onDependencyChanged(_a, _e); }
};

}
