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

MARTTA_OBJECT_CPP(Statement);	
	
bool Statement::isTyped(int _i) const
{
	return entityIs<Typed>(_i);
}

Typed* Statement::asTyped(int _i) const
{
	return entityAs<Typed>(_i);
}

Type Statement::typeOf(int _i) const
{
	return entityIs<Typed>(_i) ? entityAs<Typed>(_i)->type() : Type();
}

QList<Typed*> Statement::typeds() const
{
	return entitiesOf<Typed>();
}

QList<ValueDefiner*> Statement::valuesInLocalScope() const
{
	if (!contextIs<Statement>())
		return QList<ValueDefiner*>();
	QList<ValueDefiner*> ret = contextAs<Statement>()->valuesInLocalScope();
	for(int i = 0; i < contextIndex(); ++i)
		if (parentsChildIs<ValueDefiner>(i))
			ret += parentsChildAs<ValueDefiner>(i);
	return ret;
}

}
