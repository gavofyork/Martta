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

#include "TextLabel.h"
#include "TopLevelType.h"
#include "NamespaceEntity.h"

namespace Martta
{

MARTTA_OBJECT_CPP(NamespaceEntity);	

bool NamespaceEntity::keyPressed(EntityKeyEvent const* _e)
{
	if (attemptAppend(_e))
		return true;
	else
		return Super::keyPressed(_e);
}

QString NamespaceEntity::defineLayout(ViewKeys&) const
{
	QString ret = QString("ycode;'namespace';Mi;%1;n;").arg(Identity);
	
	for(int i = 0; i < entities().size(); i++)
		ret += QString("i;%1;v8;").arg(i);
	return ret;
}

Kinds NamespaceEntity::allowedKinds(int _i) const
{
	if (_i >= 0)
		return Kind::of<TopLevelType>();
	return Super::allowedKinds(_i);
}

QString NamespaceEntity::interfaceCode() const
{
	QString ret;
	ret += "namespace " + codeName() + "\n{\n";

	QList<DeclarationEntity*> q;
	QMultiMap<DeclarationEntity*, DeclarationEntity*> deps;
	foreach (DeclarationEntity* i, allEntitiesOf<DeclarationEntity>())
	{
		foreach (DeclarationEntity* j, i->utilisedSiblings())
			deps.insert(i, j);
		if (!deps.count(i))
			q << i;
	}

	QList<DeclarationEntity*> ds = solve(q, deps);
	if (!ds.size())
		return QString();
	foreach (DeclarationEntity* i, ds)
		ret += i->interfaceCode() + "\n";
	
	if (ret.endsWith("\n\n")) ret.chop(1);
	ret += "};\n";
	return ret;
}

QString NamespaceEntity::implementationCode() const
{
	QString ret;
	QList<DeclarationEntity*> ordered;
	foreach (DeclarationEntity* i, allEntitiesOf<DeclarationEntity>())
		ret += i->implementationCode() + "\n";
	if (ret.endsWith("\n\n")) ret.chop(1);
	return ret;
}

}
