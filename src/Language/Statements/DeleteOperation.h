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

#include "Const.h"
#include "Pointer.h"
#include "Common.h"
#include "UnaryOperation.h"

class QSvgRenderer;

namespace Martta
{

class DeleteOperation: public UnaryOperation
{
	MARTTA_OBJECT(UnaryOperation)

public:
	inline static bool					keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePlaceholderKeyPressHandler<DeleteOperation>(_p, _e, "~"); }
	
protected:
	virtual Types						allowedTypes(int) const { return Type(Void).topWith(Const()).topWith(Pointer()); }
	virtual Type						type() const { return Type(Void); }
	virtual String						code() const { return "delete " + childAs<Statement>(TheOperand)->code(); }	
	virtual String						defineLayout(ViewKeys const&) const { return String("p:/delete.svg;^;%d;p:/delete.svg").arg(TheOperand); }
};

}
