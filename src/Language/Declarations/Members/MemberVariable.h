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

#include "Variable.h"
#include "MemberValue.h"

namespace Martta
{

class MemberVariable: public MemberValue
{
	MARTTA_OBJECT(MemberValue)
	
public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);
	
protected:
	virtual Kinds						memberAllowedKinds(int _i) const { if (_i == 0) return Kind::of<Variable>(); else return Kinds(); }
	
	virtual Type						type() const;
	virtual QString						name() const { return localAs<DeclarationEntity>(0)->name(); }
	virtual QString						codeName() const { return localAs<DeclarationEntity>(0)->codeName(); }
	virtual bool						usurpsChild(Entity const* _e) const { return _e == local(0); }
};

}