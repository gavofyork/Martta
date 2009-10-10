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

#include <msHash.h>
using namespace MarttaSupport;

#include "Operator.h"
#include "Evaluation.h"

#ifndef M_API_Statements
#define M_API_Statements M_OUTAPI
#endif

namespace Martta
{

class M_API_Statements Operation: public_super Evaluation
{
	MARTTA_PLACEHOLDER(Evaluation)

public:
	MARTTA_NAMED(FirstOperand)
	MARTTA_NAME_ALIAS(TheOperand)
	MARTTA_NAMED(SecondOperand)

	static bool							keyPressedOnPosition(Position const&, KeyEvent const*);
	bool								keyPressed(KeyEvent const*);

	virtual Precedence					precedence() const { return id().precedence(); }
	virtual Operator					id() const { return Operator(); }

protected:
	virtual bool						isSlidable(int) const { return false; }
	virtual Concept*						lastOperand() const { return child(SecondOperand) ? child(SecondOperand) : child(FirstOperand); }	// QUICK optimise into overrides
	static Position						slideOnPrecedence(Position _p, Precedence _d, Associativity _a, Position const& _block);

	// Must return all entities that are LambdaNamer-derived and whose id() is operator _o.
	static List<ValueDefiner*>			allOperators(Operator _o);

	/// The argument type of the method or function type _t. argument _index can be UndefinedIndex (return type), TheOperand/FirstOperand or SecondOperand.
	static Type							prototypeOf(Type const& _t, int _index = UndefinedIndex);
	static bool							prototypeHasArgumentAt(Type const& _t, int _cardinal);
	static List<ValueDefiner*>			findBestOverload(Types const& _actual, List<ValueDefiner*> const _candidates);
};

}
