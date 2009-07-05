/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "Common.h"

namespace Martta
{

bool s_testing = false;
char const* s_asserted = 0;

const Precedence NoPrecedence = log(0.0);

String code(Qualifiers _q, WhitespacePosition _p)
{
	String ret;
	for (uint i = 1; (uint)QualifierMask > i; i <<= 1)
		if (_q & i)
			ret += ((!ret.isEmpty() && _p == NoWhitespace || _p == AtStart) ? " " : "") + String(code((Qualifier)i)) + ((_p == AtEnd) ? " " : "");
	return ret;
}

}
