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

#include "Operator.h"
#include "MemberLambda.h"

#ifndef M_API_Members
#define M_API_Members M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Members MethodOperator: public_super MemberLambda
{
	MARTTA_PROPER(MemberLambda)

public:
	virtual Operator					id() const;

	virtual int							argumentCount() const;
	virtual Type						argumentType(int _i) const;

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual String						defineNameHtml() const { return toHtml(child(Identity)); }
	virtual String						name() const;
	virtual String						codeName() const;
	virtual int							minRequired(int _i) const { return _i == Identity ? 1 : _i == Cardinals ? isBinary() ? 1 : 0 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual void						onDependencyChanged(int, Concept* _e) { if (_e->index() == Identity) validifyChildren(); }
	
private:
	bool								isUnary() const { return id().isUnary(); }
	bool								isBinary() const { return id().isBinary(); }
	bool								isEnary() const { return id().isEnary(); }
};

}
