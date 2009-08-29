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

#ifndef M_API_CTypes
#define M_API_CTypes M_OUTAPI
#endif

namespace Martta
{

/**
 * Enumeration of fundamental types.
 *
 * Because any (non-const) type is implicitly castable to Void, we can use this as the left argument in the ',' operator.
 * The Ptr value exists only for referencing into m_fundamentalOperators the fundamental pointer operations.
 * In these cases the operators in question use a null QualifiedType value to signify a placeholder for any type.
 */
static const uint
	Void = 0x0000, Ptr = Void,
	Signed = 0x0000, Unsigned = 0x0001, BuiltinDeclaration = 0x0000, Complex = 0x0001,
	Natural = 0x0000, Short = 0x0002, Long = 0x0004, Longlong = 0x0006,
	Bool = 0x0008, Int = 0x0010, Char = 0x0020, Float = 0x0040, Double = 0x0080,
	Wchar = Char|Long;

}
