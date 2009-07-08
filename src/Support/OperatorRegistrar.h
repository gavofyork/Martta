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

namespace Martta
{

class ValueDefiner;

class OperatorRegistrar
{

public:
	static OperatorRegistrar*			get() { return s_this ? s_this : (s_this = new OperatorRegistrar); }

	inline List<ValueDefiner*>::Box		operators(Operator _o) const { return m_operatorCatalogue.values(_o); }
	
	inline void							registerOperator(Operator _o, ValueDefiner* _v) { m_operatorCatalogue.insert(_o, _v); }
	inline void							unregisterOperator(Operator _o, ValueDefiner* _v) { m_operatorCatalogue.removeOne(_o, _v); }
	
private:
	MultiHash<Operator, ValueDefiner*>	m_operatorCatalogue;
	
	static OperatorRegistrar*			s_this;
};

}
