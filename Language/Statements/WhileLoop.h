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

#include "Conditional.h"
#include "Loop.h"

#ifndef M_API_Statements
#define M_API_Statements M_OUTAPI
#endif

namespace Martta
{

class M_API_Statements WhileLoop: public_super Loop, public_interface Conditional
{
	MARTTA_PROPER(Loop)
	MARTTA_ALSO_INHERITS(Conditional, 0)

public:
	inline static bool					keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePlaceholderKeyPressHandler<WhileLoop>(_p, _e, "W"); }

protected:
	virtual Kinds						allowedKinds(int _index) const;

private:
	virtual int							minRequired(int _i) const { return _i == Condition ? 1 : Super::minRequired(_i); }
	virtual Types						allowedTypes(int _index) const;
	virtual String						code() const;
	virtual String						defineHtml() const;
	virtual List<int> const&			defineDeclarationOrder() const { static const List<int> r = List<int>() << Condition << Body; return r; }
};

class M_API_Statements UntilLoop: public_super WhileLoop
{
	MARTTA_PROPER(WhileLoop)

public:
	inline static bool					keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePlaceholderKeyPressHandler<UntilLoop>(_p, _e, "U"); }

private:
	virtual String						code() const;
	virtual String						defineHtml() const;
	virtual Kinds						allowedKinds(int _index) const;
};

}
