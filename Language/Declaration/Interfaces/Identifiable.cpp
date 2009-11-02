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
#include "IdLabel.h"
#include "Concept.h"
#include "Identifiable.h"

namespace Martta
{

MARTTA_NOTION_CPP(Identifiable);
MARTTA_NAMED_CPP(Identifiable, Identity);

Identifiable::~Identifiable()
{
}

String Identifiable::name() const
{
	if (IdLabel* l = self()->tryChild<IdLabel>(Identity))
		return l->name();
	return String::null;
}

String Identifiable::codeName() const
{
	if (IdLabel* l = self()->tryChild<IdLabel>(Identity))
		return l->code();
	return String::null;
}

String Identifiable::key() const
{
	Identifiable const* _registrar = 0;
	int i = self()->ancestor<Identifiable>()->registerAnonymous(this, &_registrar);
	AssertNR(_registrar);
	return _registrar->key() + "::" + String::number(i);
}

Identifiable* Identifiable::addressableContext() const
{
	return self()->parentIs<Identifiable>() ? self()->parentAs<Identifiable>() : 0;
}

Identifiable* Identifiable::lookupChild(String const& _key) const
{
	foreach (Identifiable* e, self()->cardinalChildrenOf<Identifiable>())
		if (e->identity() == _key)
			return e;
	return 0;
}

Identifiable* Identifiable::find(String const& _key) const
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

int Identifiable::registerAnonymous(Identifiable const* _e, Identifiable const** _registrar) const
{
	AssertNR(self()->ancestor<Identifiable>());
	return self()->ancestor<Identifiable>()->registerAnonymous(_e, _registrar);
}

void Identifiable::registerAnonymous(Identifiable const* _e, int _k)
{
	AssertNR(self()->ancestor<Identifiable>());
	self()->ancestor<Identifiable>()->registerAnonymous(_e, _k);
}

void Identifiable::properties(Hash<String, String>& _p) const
{
	if (!addressableContext())
		_p[L"identity"] = String::number(self()->ancestor<Identifiable>()->registerAnonymous(this));
	_p[L"generalkey"] = key();
}

void Identifiable::setProperties(Hash<String, String> const& _p)
{
	if (_p.contains(L"identity"))
		self()->ancestor<Identifiable>()->registerAnonymous(this, _p[L"identity"].toInt());
	ModelPtrRegistrar::get()->registerTemp(this, _p[L"generalkey"]);
}

void Identifiable::apresLoad()
{
	ModelPtrRegistrar::get()->restorePtrsOf(this);
}

}
