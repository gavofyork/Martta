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

#include "CommonGraphics.h"
#include "DecorationContext.h"
#include "Class.h"
#include "TextLabel.h"
#include "EnumValue.h"
#include "Enumeration.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(EnumerationNamer);

QString EnumerationNamer::interfaceCode() const
{
	QString ret;
	ret += "enum " + codeName() + "\n";
	ret += "{\n";
	foreach (EnumValue* f, self()->entitiesOf<EnumValue>())
		ret += f->code() + ",\n";
	if (ret.endsWith(",\n"))
		ret.chop(2), ret += "\n";
	ret += "};\n";
	return ret;
}

void EnumerationNamer::updateStem()
{
	QString oldStem = m_stem;
	m_stem = QString();
	foreach (EnumValue* i, self()->entitiesOf<EnumValue>())
		if (m_stem.isEmpty())
			m_stem = i->codeName();
		else
			while (m_stem != i->codeName().left(m_stem.length()))
				m_stem.chop(1);
	if (oldStem != m_stem)
		self()->changed();
}

bool EnumerationNamer::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->key() == Qt::Key_Return)
	{
		InsertionPoint p = (_e->isFocused() || _e->focalIndex() == 0) ?
		(/*_e->inserting() || */_e->modifiers() & Qt::ShiftModifier) ?
		self()->middle(1) :
		self()->back() :
		self()->middle(_e->focalIndex() + ((/*_e->inserting() || */_e->modifiers() & Qt::ShiftModifier) ? 0 : 1));
		EnumValue* s = new EnumValue;
		s->prepareChildren();
		p.place(s);
		s->entity(0)->setCurrent();
	}
	else if (_e->key() == Qt::Key_Home && _e->focalIndex() > -1)
	{
		self()->entity(_e->focalIndex())->setCurrent();
	}
	else if (_e->text() == "H")
	{
		self()->setCurrent();
	}
	else
		return false;
	return true;
}

QString EnumerationNamer::defineLayout(ViewKeys& _viewKeys) const
{
	QString ret;
	if (_viewKeys["expanded"].toBool())
	{
		ret += "ycode;'enum ';fb;cblack;s" + Type(const_cast<TypeDefinition*>(asKind<TypeDefinition>()))->idColour() + ";!%1;s;ycode;n;'{'";
		foreach (EnumValue* f, self()->entitiesOf<EnumValue>())
			ret += QString(";n;i;%1").arg(f->contextIndex());
		ret += ";n;'}'";
	}
	else
	{
		ret += "ycode;'enum ';fb;cblack;s" + Type(const_cast<TypeDefinition*>(asKind<TypeDefinition>()))->idColour() + ";!%1;s;yminor;' (";
		int n = self()->entitiesOf<EnumValue>().count();
		if (n > 1 || !self()->entitiesOf<EnumValue>()[0]->codeName().isEmpty())
			ret += QString::number(n) + " entr" + (n > 1 ? "ies" : "y");
		if (ret.endsWith("("))
			ret += "empty";
		ret += ")'";
	}
	return ret.arg(self()->entityIndexOf<IdLabel>());
}

}
