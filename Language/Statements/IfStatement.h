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

#include "Corporal.h"
#include "Conditional.h"
#include "Untyped.h"

namespace Martta
{

class IfStatement: public Untyped, public_interface Corporal, public_interface Conditional
{
	MARTTA_OBJECT(Untyped)

public:
	enum { AltBody = FirstNamed, EndOfNamed };

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual String						code() const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual String						defineLayout(ViewKeys const&) const;
	virtual String						defineHtml() const;
	virtual int							minRequired(int _i) const { return _i == Condition || _i == Body ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _index) const;
	virtual Types						allowedTypes(int _index) const;
	virtual List<int> const&			defineDeclarationOrder() const { static const List<int> r = List<int>() << Condition << Body << AltBody; return r; }
	virtual bool						requiresSemicolon() const { return false; }
};

class UnlessStatement: public IfStatement
{
	MARTTA_OBJECT(IfStatement)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual String						code() const;
	virtual String						defineLayout(ViewKeys const&) const;
	virtual String						defineHtml() const;
};

}
