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
#include "TextLabel.h"
#include "OperatorLabel.h"
#include "ValueDefiner.h"
#include "Declaration.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Declaration);

Declaration::~Declaration()
{
}

// Identification, search & location.
// TODO: move to Identifiable, make recursive?.
Identifiable* Declaration::findEntity(String const& _key) const
{
//	mDebug() << *this << "Seaching for" << _key;
	if (_key.startsWith("::"))
	{
		Identifiable const* i = this;
		String k = _key;
		while (i && !k.isEmpty())
		{
			String s = k.section("::", 1, 1);
			k = k.mid(s.size() + 2);
			i = i->lookupChild(s);
//			mDebug() << "Key" << s << "gives" << (i ? i->self() : 0);
		}
		return const_cast<Identifiable*>(i);
	}
	return 0;
}

void Declaration::archivePtrs(bool) const
{
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

Kinds Declaration::allowedKinds(int _i) const
{
	if (_i == Identity)
		return Kind::of<TextLabel>();
	return Super::allowedKinds(_i);
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
