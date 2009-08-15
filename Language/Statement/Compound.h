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

#include "Untyped.h"

namespace Martta
{

class Compound: public Untyped
{
	MARTTA_OBJECT(Untyped)

public:
	virtual String						code() const;

protected:
	virtual int							minRequired(int _i) const { return _i == Cardinals ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const { return _i >= 0 ? Kind::of<Statement>() : Super::allowedKinds(_i); }
	virtual Types						allowedTypes(int _i) const { return _i >= 0 ? Types(Type()) : Super::allowedTypes(_i); }
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual String						defineHtml() const;
	virtual void						appendDefinedUptoHere(int _i, List<ValueDefiner*>* _list) const;
	virtual bool						isSuperfluous() const { return cardinalChildCount() == 1 && child(0)->kind() == Kind::of<Statement>() && !isNecessary(); }
	virtual bool						requiresSemicolon() const { return false; }

	static String						statementsToHtml(List<Entity*> const& _es);
	static String						statementsToHtml(List<Statement*> const& _es);
};

class HardCompound: public Compound
{
	MARTTA_OBJECT(Compound)

protected:
	virtual String						defineHtml() const;
};

}
