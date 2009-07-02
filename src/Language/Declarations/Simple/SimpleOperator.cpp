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

#include "FunctionType.h"
#include "Memberify.h"
#include "Type.h"
#include "Reference.h"
#include "Operation.h"
#include "SimpleOperator.h"

namespace Martta
{

MARTTA_OBJECT_CPP(SimpleOperator);

void SimpleOperator::construct(int _id, Operator _o, Type const& _returns, Types const& _args, char const* _key)
{
	m_operator = _o;
	Simple::construct(0, _id, false, _returns, _args, _key);
	Operation::registerOperator(_o, this);
}

void SimpleOperator::destruct()
{
	Operation::unregisterOperator(m_operator, this);
	Super::destruct();
}

}
