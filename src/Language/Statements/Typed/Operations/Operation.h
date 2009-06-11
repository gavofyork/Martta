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

#include "Operator.h"
#include "Evaluation.h"

namespace Martta
{

class Callable;

class Operation: public Evaluation
{
	MARTTA_PLACEHOLDER(Evaluation)

public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const&, EntityKeyEvent const*);
	bool								keyPressed(EntityKeyEvent const*);
	
	virtual Precedence					precedence() const { return id().precedence(); }
	virtual Operator					id() const { return Operator(); }

	static void							registerOperator(Operator _o, ValueDefiner* _v, RootEntity*) { s_operatorCatalogue.insert(_o, _v); }
	static void							unregisterOperator(Operator _o, ValueDefiner* _v, RootEntity*) { s_operatorCatalogue.remove(_o, _v); }

protected:
	virtual bool						isSlidable(int _i) const { return _i != entities().size() - 1; }
	static InsertionPoint				slideOnPrecedence(InsertionPoint _p, Precedence _d, Associativity _a, InsertionPoint const& _block);
	
	// Must return all entities that are LambdaNamer-derived and whose id() is operator _o.
	static QList<ValueDefiner*>			allOperators(Operator _o);
	
	/// The argument type of the method or function type _t. argument -1 is the return type.
	static Type							prototypeOf(Type const& _t, int _index = -1);
	static bool							prototypeHasArgumentAt(Type const& _t, int _index);
	static QList<ValueDefiner*>			findBestOverload(Types const& _actual, QList<ValueDefiner*> const _candidates);
	
	// TODO: Should really be indexed on RootEntity too, but this won't matter until there are multiple RootEntities at once, 
	// and I doubt that's going to happen any time soon.
	static QMultiHash<Operator, ValueDefiner*>	s_operatorCatalogue;
};

}
