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

#include "RootEntity.h"
#include "IdLabel.h"
#include "TextLabel.h"
#include "Entity.h"
#include "DeclarationEntity.h"
#include "Identifiable.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(Identifiable);
MARTTA_NAMED_CPP(Identifiable, Identity);

QString Identifiable::name() const
{
	if (TextLabel* l = self()->tryChild<TextLabel>(Identity))
		return l->name();
	return QString::null;
}

QString Identifiable::codeName() const
{
	if (IdLabel* l = self()->tryChild<IdLabel>(Identity))
		return l->code();
	return QString::null;
}

QString Identifiable::key() const
{
	M_ASSERT(self()->hasAncestor<DeclarationEntity>());
	return self()->ancestor<DeclarationEntity>()->key() + "::" + QString::number(self()->ancestor<DeclarationEntity>()->registerAnonymous(this));
}

Identifiable* Identifiable::addressableContext() const
{
	return self()->parentIs<Identifiable>() ? self()->parentAs<Identifiable>() : 0;
}

void Identifiable::onLeaveScene(RootEntity* _new, RootEntity* _old)
{
	if (_old && _old != _new)
		_old->noteDeletion(this);
}

Identifiable* Identifiable::lookupChild(QString const& _key) const
{
	foreach (Identifiable* e, self()->cardinalChildrenOf<Identifiable>())
		if (e->identity() == _key)
			return e;
	return 0;
}

void Identifiable::importDom(QDomElement const& _element)
{
	if (_element.hasAttribute("index"))
		self()->ancestor<DeclarationEntity>()->registerAnonymous(this, _element.attribute("index").toInt());
}

void Identifiable::exportDom(QDomElement& _element) const
{
	if (!addressableContext())
		_element.setAttribute("index", self()->ancestor<DeclarationEntity>()->registerAnonymous(this));
}

}
