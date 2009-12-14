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

#include "Type.h"
#include "NameEntryPoint.h"
#include "WebViewable.h"
#include "TypedOwner.h"
#include "Concept.h"

#ifndef M_API_Statement
#define M_API_Statement M_OUTAPI
#endif

namespace Martta
{

class ValueDefiner;
class Typed;

class M_API_Statement Statement: public_super Concept, public_interface TypedOwner, public_interface NameEntryPoint
{
	MARTTA_PLACEHOLDER(Concept)
	MARTTA_ALSO_INHERITS(TypedOwner, 0)
	MARTTA_ALSO_INHERITS(NameEntryPoint, 1)

public:
	virtual String						code() const { return "(void)0;"; }
	virtual String						codeAsStatement() const { return code(); }
	virtual bool						requiresSemicolon() const { return true; }

	virtual List<ValueDefiner*>			valuesInLocalScope() const;

	List<Statement*>					statements() const { return cardinalChildrenOf<Statement>(); }
	bool			 					isStatement(int _i) const { return childIs<Statement>(_i); }
	Statement*		 					asStatement(int _i) const { return childAs<Statement>(_i); }

	bool								isTyped(int _i) const;
	Typed*								asTyped(int _i) const;
	Type								typeOf(int _i) const;

	void								committed(Named* _n, CodeScene* _cs) { NameEntryPoint::committed(_n, _cs); }

protected:
	virtual List<int> const&			defineDeclarationOrder() const { static const List<int> r; return r; }
	virtual void						appendDefinedUptoHere(int, List<ValueDefiner*>*) const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s) { return NameEntryPoint::newDelegate<Statement>(_s); }
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual bool						isChildInValidState(int _index) const { return Super::isChildInValidState(_index) && !child(_index)->isPlaceholder(); }

	virtual String						informationHtml() const;
};

}
