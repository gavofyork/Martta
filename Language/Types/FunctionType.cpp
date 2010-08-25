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

#include "IdentifierSet.h"
#include "VariablePlacer.h"
#include "FunctionType.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(InvocableType);
MARTTA_PROPER_CPP(WildInvocableType);
MARTTA_PLACEHOLDER_CPP(ContainedInvocableType);
MARTTA_NAMED_CPP(ContainedInvocableType, Returned);
MARTTA_PROPER_CPP(FunctorType);
MARTTA_PROPER_CPP(FunctionType);

bool WildInvocableType::defineSimilarityFrom(TypeConcept const* _f, Castability _c) const
{
	if (_f->isKind<InvocableType>())
		return true;
	return Super::defineSimilarityFrom(_f, _c);
}

bool FunctionType::defineSimilarityTo(TypeConcept const* _t, Castability _c) const
{
	if (_t->isKind<FunctionType>())
	{
		FunctionType const* t = _t->asKind<FunctionType const>();
		if (t->minimumArgCount() != minimumArgCount() || t->m_ellipsis != m_ellipsis || returnType() != t->returnType())
			goto OUT;
		for (int i = 0; i < t->minimumArgCount(); i++)
			if (argumentType(i) != t->argumentType(i))
				goto OUT;
		return true;
		OUT:;
	}
	return Super::defineSimilarityTo(_t, _c);
}

List<Declaration*> ContainedInvocableType::utilisedX() const
{
	List<Declaration*> ret;
	foreach (TypeConcept* i, childrenOf<TypeConcept>())
		ret << i->utilised();
	return ret;
}

Types ContainedInvocableType::assignableTypes() const
{
	return Type(*this);
}

bool ContainedInvocableType::canStandAlone() const
{
	if (!returnType()->canStandAlone())
		return false;
	foreach (TypeConcept* e, cardinalChildrenAs<TypeConcept>())
		if (!e->asKind<TypeConcept>()->canStandAlone())
			return false;
	return true;
}

String WildInvocableType::code(String const& _middle) const
{
	return L"??" "?(" + _middle + ")(...)";
}

String FunctionType::code(String const& _middle) const
{
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

bool FunctorType::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<FunctorType>(_p, _e, L"((", false);
}

String FunctorType::defineHtml() const
{
	return tagOf(L"minor symbol", L"((") + toHtml(cardinalChildren()) + tagOf(L"minor symbol", L")") + "<^>" + tagOf(L"symbol", L"&rarr;") + toHtml(child(Returned)) + tagOf(L"minor symbol", L")");
}

String FunctorType::code(String const& _middle) const
{
	String ret = L"((";
	foreach (Concept* e, cardinalChildren())
	{
		if (ret.right(2) != L"((")
			ret += L", ";
		ret += e->asKind<TypeConcept>()->code();
	}
	ret += L")->" + returnType()->code() + L")" + _middle;

	return ret;
}
/*
template<class R>
class Functor
{
public:
	class Executive
	{
	public:
		virtual R execute() = 0;
	};

	Functor(Executive const& _e): m_data(new Executive(_e)) {}
	Functor(Executive* _e): m_data(_e) {}
	R operator()() { return m_data->execute(); }

private:
	SafePtr<Executive> m_data;
};

class OriginalFO: public Functor<int>::Executive
{
public:
	int operator()() { return 1; }
	virtual int execute() { return operator()(); }
};
*/
}
