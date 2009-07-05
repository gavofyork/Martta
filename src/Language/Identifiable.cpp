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

#include <QtXml>

#include "ModelPtrRegistrar.h"
#include "IdLabel.h"
#include "TextLabel.h"
#include "Entity.h"
#include "Declaration.h"
#include "Identifiable.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(Identifiable);
MARTTA_NAMED_CPP(Identifiable, Identity);

Identifiable::~Identifiable()
{
}

String Identifiable::name() const
{
	if (TextLabel* l = self()->tryChild<TextLabel>(Identity))
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
	AssertNR(self()->hasAncestor<Declaration>());
	return self()->ancestor<Declaration>()->key() + "::" + String::number(self()->ancestor<Declaration>()->registerAnonymous(this));
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

void Identifiable::properties(Hash<String, String>& _p) const
{
	if (!addressableContext())
		_p["identity"] = String::number(self()->ancestor<Declaration>()->registerAnonymous(this));
	_p["generalkey"] = key();
}

void Identifiable::setProperties(Hash<String, String> const& _p)
{
	if (_p.contains("identity"))
		self()->ancestor<Declaration>()->registerAnonymous(this, _p["identity"].toInt());
	ModelPtrRegistrar::get()->registerTemp(this, _p["generalkey"]);
}

}
