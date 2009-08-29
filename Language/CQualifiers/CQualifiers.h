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

#include <cmath>

#include <msSupport.h>
#include <msString.h>
#include <msFlags.h>
using namespace MarttaSupport;

#ifndef M_API_CQualifiers
#define M_API_CQualifiers M_OUTAPI
#endif

namespace Martta
{

enum Qualifier
{
	NoQualifiers = 0,
	Static = 0x0002,	// Variable/Function
	Extern = 0x0004,	// Variable/Function
	Mutable = 0x0010,	// Variable
	Volatile = 0x0020,	// Variable
	Restrict = 0x0100,	// Function
	Inline = 0x0200,	// Function
	Explicit = 0x0400,	// Constructor
	VariableMask = Static|Extern|Mutable|Volatile,
	FunctionMask = Static|Extern|Restrict|Inline|Explicit,
	QualifierMask = Static|Extern|Mutable|Volatile|Restrict|Inline|Explicit
};
M_DECLARE_FLAGS(Qualifier);

inline char const* code(Qualifier _q)
{
	switch (_q)
	{
		case Static: return "static";
		case Extern: return "extern";
		case Mutable: return "mutable";
		case Volatile: return "volatile";
		case Restrict: return "restrict";
		case Inline: return "inline";
		case Explicit: return "explicit";
		default: return "";
	}
}

enum WhitespacePosition
{
	NoWhitespace = 0,
	AtStart,
	AtEnd
};

inline String code(Qualifiers _q, WhitespacePosition _p = AtEnd)
{
	String ret;
	for (uint i = 1; (uint)QualifierMask > i; i <<= 1)
		if (_q & i)
			ret += (((!ret.isEmpty() && _p == NoWhitespace) || _p == AtStart) ? " " : "") + String(code((Qualifier)i)) + ((_p == AtEnd) ? " " : "");
	return ret;
}

}
