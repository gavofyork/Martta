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

#include "FunctionType.h"

namespace Martta
{

MARTTA_PROPER_CPP(InvocableType);
MARTTA_NAMED_CPP(InvocableType, Returned);
MARTTA_PROPER_CPP(LambdaType);
MARTTA_PROPER_CPP(FunctionType);

bool InvocableType::defineSimilarityTo(TypeConcept const* _t, Castability _c) const
{
	if (_t->isKind<FunctionType>())
	{
		if (_t->asKind<FunctionType>()->m_wild)
			return true;
		FunctionType const* t = _t->asKind<FunctionType const>();
		if (t->minimumArgCount() != minimumArgCount() || t->ellipsis() != ellipsis() || returnType() != t->returnType())
			goto OUT;
		for (int i = 0; i < t->minimumArgCount(); i++)
			if (argumentType(i) != t->argumentType(i))
				goto OUT;
		return true;
		OUT:;
	}
	return Super::defineSimilarityTo(_t, _c);
}

List<Declaration*> InvocableType::utilisedX() const
{
	List<Declaration*> ret;
	foreach (TypeConcept* i, childrenOf<TypeConcept>())
		ret << i->utilised();
	return ret;
}

Types InvocableType::assignableTypes() const
{
	return Type(*this);
}

String InvocableType::code(String const& _middle) const
{
	if (m_wild)
		return "#unknown-type#(" + _middle + ")(...)";

	String ret = returnType()->code() + "(" + _middle + ")(";
	foreach (Concept* e, cardinalChildren())
	{
		if (ret.right(1) != "(")
			ret += ", ";
		ret += e->asKind<TypeConcept>()->code();
	}
	if (m_ellipsis)
		ret += (cardinalChildCount()) ? ", ..." : "...";
	ret += ")";

	return ret;
}

}
