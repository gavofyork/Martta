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
#include "EnumValue.h"
#include "Enumeration.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Enumeration);	
	
bool Enumeration::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<Enumeration>(_p, _e, "E");
}

Types Enumeration::assignableTypes() const
{
	return Type(const_cast<Enumeration*>(this));
}

Kinds Enumeration::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<TextLabel>();
	else
		return Kind::of<EnumValue>();
}

QString Enumeration::interfaceCode() const
{
	QString ret;
	ret += "enum " + codeName() + "\n";
	ret += "{\n";
	foreach (EnumValue* f, entitiesOf<EnumValue>())
		ret += f->code() + ",\n";
	if (ret.endsWith(",\n"))
		ret.chop(2), ret += "\n";
	ret += "};\n";
	return ret;
}

void Enumeration::updateStem()
{
	m_stem = QString();
	foreach (EnumValue* i, entitiesOf<EnumValue>())
		if (m_stem.isEmpty())
			m_stem = i->codeName();
		else
			while (m_stem != i->codeName().left(m_stem.length()))
				m_stem.chop(1);
	changed();
}

bool Enumeration::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == "H")
	{
		setCurrent();
	}
	else if (attemptAppend(_e))
	{}
	else
		return Super::keyPressed(_e);
	return true;
}

QString Enumeration::defineLayout(ViewKeys&) const
{
	QString ret = "^;ycode;'enum ';fb;cblack;s" + Type(const_cast<Enumeration*>(this))->idColour() + ";!0;s;ycode;n;'{'";
	foreach (EnumValue* f, entitiesOf<EnumValue>())
		ret += QString(";n;i;%1").arg(f->contextIndex());
	return ret + ";n;'}'";
}


}
