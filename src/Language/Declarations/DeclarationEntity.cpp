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

#include <QtCore>
#include <QtXml>

#include "Common.h"
#include "RootEntity.h"
#include "TextLabel.h"
#include "ValueDefiner.h"
#include "DeclarationEntity.h"

namespace Martta
{

MARTTA_OBJECT_CPP(DeclarationEntity);

DeclarationEntity::~DeclarationEntity()
{
	if (rootEntity() && rootEntity() != this)
		rootEntity()->noteDeletion(this);
	else
		qInformation() << "Deleting" << this << "with no rootEntity!";
}

QList<ValueDefiner*> DeclarationEntity::valuesKnown() const
{
	QList<ValueDefiner*> ret = contextIs<DeclarationEntity>() ? contextAs<DeclarationEntity>()->valuesKnown() : QList<ValueDefiner*>();
	foreach (DeclarationEntity* d, siblingsOf<DeclarationEntity>())
		ret += d->valuesAdded();
	return ret;
}

QString DeclarationEntity::name() const
{
	if (!entitiesOf<TextLabel>().size())
		return QString();
	return entitiesOf<TextLabel>()[0]->name();
}

QString DeclarationEntity::codeName() const
{
	if (!entitiesOf<TextLabel>().size())
		return QString();
	return entitiesOf<TextLabel>()[0]->asKind<Label>()->code();
}

Kinds DeclarationEntity::allowedKinds(int _i) const
{
	return _i ? Kinds() : Kind::of<TextLabel>();
}

void DeclarationEntity::onLeaveScene(RootEntity* _new, RootEntity* _old)
{
	Super::onLeaveScene(_new, _old);
	if (_old && _old != _new)
		_old->noteDeletion(this);
}

SubAddressable* DeclarationEntity::addressableContext() const
{
	return contextIs<SubAddressable>() ? contextAs<SubAddressable>() : 0;
}

QString DeclarationEntity::reference() const
{
	return addressableContext() ? addressableContext()->reference() + "::" + codeName() : codeName();
}

QString DeclarationEntity::key() const
{
	if (addressableContext())
		return addressableContext()->key() + "::" + identity();
	else
	{
		return ancestor<DeclarationEntity>()->key() + "::" + QString::number(ancestor<DeclarationEntity>()->registerAnonymous(this));
	}
}

DeclarationEntity* DeclarationEntity::lookupChild(QString const& _key) const
{
	bool ok;
	int k = _key.toInt(&ok);
	if (ok && m_anonyma.size() > k)
		return const_cast<DeclarationEntity*>(m_anonyma[k]);
//	qDebug() << "Matching for " << _key;
	foreach (DeclarationEntity* e, entitiesOf<DeclarationEntity>())
	{	
//		if (entitiesOf<DeclarationEntity>().size() < 10)
//			qDebug() << "    " << e->identity();
		if (e->identity() == _key)
			return e;
	}
	return 0;
}

QList<DeclarationEntity*> DeclarationEntity::utilisedSiblings() const
{
	QList<DeclarationEntity*> ret;
	foreach (DeclarationEntity* i, utilised())
		if (i->hasAncestor(context()))
		{
			DeclarationEntity* e = parentsChild(i->ancestorIndex(context()))->asKind<DeclarationEntity>();
			if (e && !ret.contains(e))
				ret << e;
		}
	return ret;
}

QList<DeclarationEntity*> DeclarationEntity::utilised() const
{
	QList<DeclarationEntity*> ret;
	foreach (DeclarationEntity* i, entitiesOf<DeclarationEntity>())
		ret << i->utilised();
	qDebug() << name() << "(" << kind().name() << ") utilises:";
	foreach (DeclarationEntity* i, ret)
		qDebug() << "    " << i->name() << "(" << i->kind().name() << ")";
	return ret;
}

void DeclarationEntity::importDom(QDomElement const& _element)
{
	if (_element.hasAttribute("index"))
		ancestor<DeclarationEntity>()->registerAnonymous(this, _element.attribute("index").toInt());

	Super::importDom(_element);
}

void DeclarationEntity::exportDom(QDomElement& _element) const
{
	if (!addressableContext())
		_element.setAttribute("index", ancestor<DeclarationEntity>()->registerAnonymous(this));

	Super::exportDom(_element);
}

}
