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

#include "Typed.h"
#include "ValueDefiner.h"
#include "Statement.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(Statement);
MARTTA_REGISTER_CSS(Statement, ".Statement { background-color: transparent; padding: 0; }");

bool Statement::isTyped(int _i) const
{
	return childIs<Typed>(_i);
}

Typed* Statement::asTyped(int _i) const
{
	return childAs<Typed>(_i);
}

Type Statement::typeOf(int _i) const
{
	return childIs<Typed>(_i) ? childAs<Typed>(_i)->apparentType() : Type();
}

void Statement::appendDefinedUptoHere(int _index, List<ValueDefiner*>* _list) const
{
	List<int> const& order = defineDeclarationOrder();
	if (order.contains(_index))
		foreach (int i, order)
			if (_index == i)
				return;
			else if (ValueDefiner* v = tryChild<ValueDefiner>(i))
				*_list << v;
}

List<ValueDefiner*> Statement::valuesInLocalScope() const
{
	if (!parentIs<Statement>())
		return List<ValueDefiner*>();
	List<ValueDefiner*> ret = parentAs<Statement>()->valuesInLocalScope();
	parentAs<Statement>()->appendDefinedUptoHere(index(), &ret);
	return ret;
}

bool Statement::keyPressed(KeyEvent const* _e)
{
	if (NameEntryPoint::keyPressed(_e))
	{}
	else
		return Super::keyPressed(_e);
	return true;
}

}
