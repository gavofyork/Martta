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

#include "TypedOwner.h"
#include "Type.h"

namespace Martta
{

class ValueDefiner;
class Typed;

class Statement: public TypedOwner
{
	MARTTA_PLACEHOLDER(TypedOwner)

public:
	virtual QString						code() const { return "(void)0;"; }
	virtual QString						codeAsStatement() const { return code(); }

	virtual QList<ValueDefiner*>		valuesInLocalScope() const;

	QList<Statement*>					statements() const { return cardinalChildrenOf<Statement>(); }
	bool			 					isStatement(int _i) const { return childIs<Statement>(_i); }
	Statement*		 					asStatement(int _i) const { return childAs<Statement>(_i); }

	bool								isTyped(int _i) const;
	Typed*								asTyped(int _i) const;
	Type								typeOf(int _i) const;
	
	virtual bool						onChanged() { foreach (Entity* i, children()) i->relayoutLater(); return Super::onChanged(); }

protected:
	virtual QList<int> const&			defineDeclarationOrder() const { static const QList<int> r; return r; }
	virtual void						appendDefinedUptoHere(int, QList<ValueDefiner*>*) const;
};

}
