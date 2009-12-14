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

#include "ModelPtrRegistrar.h"
#include "Declaration.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(Declaration);

Declaration::~Declaration()
{
}

String Declaration::superChildInformationHtml(Concept const* _e) const
{
	if (_e->ancestor<Declaration>() != this)
		return String::null;
	Pairs p(L"Declaration Context", false);
	p << kind().name() << nick();
	if (utilised().size())
	{
		Pairs up("Utilised");
		foreach (Declaration* u, utilised())
			up << (u ? u->nick() : L"n/a") << (u ? u->kind().name() : L"n/a");
		p << up;
	}
	if (utilisedSiblings().size())
	{
		Pairs up("Utilised Siblings");
		foreach (Declaration* u, utilisedSiblings())
			up << (u ? u->nick() : L"n/a") << (u ? u->kind().name() : L"n/a");
		p << up;
	}
	return p;
}

int Declaration::registerAnonymous(Identifiable const* _e, Identifiable const** _registrar) const
{
	if (_registrar)
		*_registrar = this;
	if (m_anonyma.contains(_e))
		return m_anonyma.indexOf(_e);
	m_anonyma << _e;
	return m_anonyma.size() - 1;
}

void Declaration::registerAnonymous(Identifiable const* _e, int _k)
{
	while (m_anonyma.size() <= _k)
		m_anonyma << 0;
	m_anonyma[_k] = _e;
}

void Declaration::restorePtrs() const
{
	ModelPtrRegistrar::get()->restorePtrs(this);
}

List<ValueDefiner*> Declaration::valuesKnown() const
{
	List<ValueDefiner*> ret = parentIs<Declaration>() ? parentAs<Declaration>()->valuesKnown() : List<ValueDefiner*>();
	foreach (Declaration* d, cardinalSiblingsOf<Declaration>())
		ret += d->valuesAdded();
	return ret;
}

String Declaration::key() const
{
	if (addressableContext())
		return addressableContext()->key() + "::" + identity();
	else
		return Identifiable::key();
}

Identifiable* Declaration::lookupChild(String const& _key) const
{
	bool ok;
	int k = _key.toInt(&ok);
	if (ok && m_anonyma.size() > k)
		return const_cast<Identifiable*>(m_anonyma[k]);
	return Identifiable::lookupChild(_key);
}

List<Declaration*> Declaration::utilisedSiblings() const
{
	List<Declaration*> ret;
	foreach (Declaration* i, utilised())
		if (i->hasAncestor(parent()))
		{
			Declaration* e = sibling(i->ancestorIndex(parent()))->asKind<Declaration>();
			if (e && !ret.contains(e))
				ret << e;
		}
	return ret;
}

List<Declaration*> Declaration::utilised() const
{
	List<Declaration*> ret;
	foreach (Declaration* i, cardinalChildrenOf<Declaration>())
		ret << i->utilised();
//	mDebug() << name() << "(" << kind().name() << ") utilises:";
//	foreach (Declaration* i, ret)
//		mDebug() << "    " << i->name() << "(" << i->kind().name() << ")";
	return ret;
}

}
