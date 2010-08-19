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

#include "CQualifiers.h"
#include "WebViewable.h"
#include "Type.h"
#include "Labelled.h"
#include "ValueDefiner.h"

#ifndef M_API_Namers
#define M_API_Namers M_OUTAPI
#endif

namespace Martta
{

class Argument;
class Compound;

class M_CLASS M_API_Namers LambdaNamer: public_super_interface ValueDefiner, public_interface WebViewable, public_interface Labelled
{
	MARTTA_NOTION(ValueDefiner)
	MARTTA_ALSO_INHERITS(WebViewable, 0)
	MARTTA_ALSO_INHERITS(Labelled, 1)

public:
	MARTTA_NAMED(Body)
	MARTTA_NAMED(Name)
	MARTTA_NAMED(Returned)

	virtual inline String				defineLabelHtml(String const& _text) const { return type()->typeHtml(composedName(_text)); }

	// WHAT YOU GET:

	enum FunctionCodeScope { InsideScope = 0, OutsideScope = 1 };
	/// @note non-virtual - specifically to avoid automatic mixins. You have to mix it in explicitly.
	Types								allowedTypes(int _i) const { if (_i >= 0) return Type(); return Types(); }
	String								interfaceCode() const { return basicCode(InsideScope) + ";\n"; }
	String								implementationCode() const;
	String								defineLambdaHtml(String const& _middle = String::null) const;
	bool								keyPressed(KeyEvent const* _e);
	/// Gives a parameter list of the call. This depends on argumentType/CodeName, ellipsis, reference and codeName.
	String								callingCode(FunctionCodeScope _ref) const;

	// From ValueDefiner. Do be careful though - if you've got a mixin situation where one side is pure virtual
	// still, explicitly scope this side on the call or you'll just be calling yourself (and crash!).
	// e.g. class M_API_Namers B: public_super Concept, public_interface TypeNamer { MARTTA_PROPER(Concept) MARTTA_ALSO_INHERITS(TypeNamer, 0) };
	// class M_API_Namers X: public_super B, public_interface LambdaNamer { MARTTA_PROPER(B) MARTTA_ALSO_INHERITS(LambdaNamer, 0)
	//     virtual Type type() const { return LambdaNamer::type().topWith<Const>(); };
	//
	//                                        ^^^^^^^^^^^^^
	//                                    DO NOT LEAVE THIS OUT
	virtual Type						type() const;

	// WHAT YOU GIVE:

	/// Basic code for this function call. Is essentially the interfaceCode without the final ';'.
	/// A default implementation exists.
	virtual String						basicCode(FunctionCodeScope _ref) const;
	virtual String						summary() const { return basicCode(InsideScope); }
	virtual String						defineBorderClass() const { return L"LambdaNamer"; }
	virtual String						definePreHtml() const { return String::null; }
	virtual String						defineEnclosureHtml(String const& _part, String const& _middle) const;
	virtual String						defineMidHtml(String const& _middle) const;
	virtual String						definePostHtml() const { return String::null; }
	virtual String						defineNameHtml() const;
	virtual String						defineReturnHtml() const;
	virtual String						defineArgListHtml() const;
	virtual String						defineBodyHtml() const;

	/// Used for default implementation of implementationCode. Can be safely ignored if implementationCode is reimplemented.
	virtual Compound*					body() const;
	virtual bool						ellipsis() const { return false; }
	virtual Qualifiers					qualifiers() const { return NoQualifiers; }

	/// @note this only returns the number of nameable arguments. If ellipsis() is true, this could be different to the number of parameters that are passed to it in an invocation.
	/// @note argument() may not work for all arguments within argumentCount(); argumentType() is the only one guaranteed to work. This is the case in, e.g., postfix versions of ++ and -- unary operators.
	virtual int							argumentCount() const;
	virtual Argument*					argument(int _index) const;
	virtual Type						returns() const;
	virtual Type						argumentType(int _index) const;
	virtual String						argumentCodeName(int _index) const;

protected:
	virtual ~LambdaNamer()				{}
};

}
