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

#include "ValueDefiner.h"
#include "Declaration.h"

#ifndef M_API_Types
#define M_API_Types M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Types BuiltinDeclaration: public_super Declaration, public_interface ValueDefiner
{
	MARTTA_PROPER(Declaration)
	MARTTA_ALSO_INHERITS(ValueDefiner, 0)

public:
	virtual Type						type() const { return *childAs<TypeConcept>(0); }

	// Use this instead of deleting it or you'll have to unregister them explicitly.
	virtual void						destruct();
	virtual Kinds						allowedKinds(int _i) const;

	virtual void						apresLoad() { Declaration::apresLoad(); ValueDefiner::apresLoad(); }

protected:
	void								construct(TypeConcept const* _scope, int _id, bool _isConst, Type const& _returns, Types const& _args, char const* _key);

	String								m_key;
	int									m_myId;
};

}
