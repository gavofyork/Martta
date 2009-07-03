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
#include "CodeScene.h"
#include "Class.h"
#include "TextLabel.h"
#include "EnumValue.h"
#include "Enumeration.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(EnumerationNamer);

String EnumerationNamer::interfaceCode() const
{
	String ret;
	ret += "enum " + codeName() + "\n";
	ret += "{\n";
	foreach (EnumValue* f, self()->cardinalChildrenOf<EnumValue>())
		ret += f->code() + ",\n";
	if (ret.endsWith(",\n"))
		ret.chop(2), ret += "\n";
	ret += "};\n";
	return ret;
}

void EnumerationNamer::updateStem()
{
	String oldStem = m_stem;
	m_stem = String();
	foreach (EnumValue* i, self()->cardinalChildrenOf<EnumValue>())
		if (m_stem.isEmpty())
			m_stem = i->codeName();
		else if (!i->codeName().isEmpty())
		{
			while (m_stem != i->codeName().left(m_stem.length()))
				m_stem.chop(1);
			if (m_stem.isEmpty())
				break;
		}
	if (oldStem != m_stem)
		self()->changed();
}

bool EnumerationNamer::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->key() == Qt::Key_Return && _e->codeScene()->viewKeys(self())["expanded"].toBool())
	{
		Position p = (_e->isFocused() || _e->focalIndex() == 0) ?
		(/*_e->inserting() || */_e->modifiers() & Qt::ShiftModifier) ?
		self()->front() :
		self()->back() :
		self()->middle(_e->focalIndex() + ((/*_e->inserting() || */_e->modifiers() & Qt::ShiftModifier) ? 0 : 1));
		EnumValue* s = new EnumValue;
		s->prepareChildren();
		p.place(s);
		s->child(Identity)->setCurrent();
	}
	else if (_e->key() == Qt::Key_Home && _e->focalIndex() != UndefinedIndex)
	{
		self()->child(_e->focalIndex())->setCurrent();
	}
	else if (_e->text() == "H")
	{
		self()->setCurrent();
	}
	else if (QRegExp("[a-z]").exactMatch(_e->text()) && !isNamed())
	{
		setNamed();
		_e->codeScene()->setCurrent(self()->child(Identity));
		_e->reinterpretLater();
	}
	else
		return false;
	return true;
}

String EnumerationNamer::defineLayout(ViewKeys const& _viewKeys) const
{
	String ret;
	String name;
	if (isNamed())
		name = (";Mo;fb;cblack;s" + Type(const_cast<TypeDefinition*>(asKind<TypeDefinition>()))->idColour() + ";!%1").arg(Identity);
	else
		name = ";Mo;c#777;yminor;'[" + m_stem + "...]'";
	if (_viewKeys["expanded"].toBool())
	{
		ret += "ycode;'enum'" + name + ";s;ycode;n;'{'";
		foreach (EnumValue* f, self()->cardinalChildrenOf<EnumValue>())
			ret += String(";n;i;%1").arg(f->index());
		ret += ";n;'}'";
	}
	else
	{
		ret += "ycode;'enum'" + name + ";s;yminor;' (";
		int n = self()->cardinalChildCountOf<EnumValue>();
		if (n > 1 || !self()->cardinalChildOf<EnumValue>()->codeName().isEmpty())
			ret += String::number(n) + " entr" + (n > 1 ? "ies" : "y");
		if (ret.endsWith("("))
			ret += "empty";
		ret += ")'";
	}
	return ret;
}

Entity* EnumerationNamer::isExpander() const
{
	return self()->isComplete() ? self()->cardinalChildrenOf<EnumValue>()[0] : 0;
}

void EnumerationNamer::onDependencyRemoved(Entity* _e)
{
	if (_e->isKind<EnumValue>())
		updateStem();
}

void EnumerationNamer::onDependencyAdded(Entity* _e)
{
	if (_e->isKind<EnumValue>())
		updateStem();
	else
		self()->changed();
}

void EnumerationNamer::onDependencyChanged(Entity* _e)
{
	if (_e->isKind<TextLabel>())
	{
		if (!_e->asKind<TextLabel>()->isNamed())
			setUnnamed();
		else
			self()->changed();
	}
	else
		updateStem();
}

void EnumerationNamer::setUnnamed()
{
	if (isNamed())
		self()->child(Identity)->killAndDelete();
	self()->changed();
}

void EnumerationNamer::setNamed()
{
	if (!isNamed())
		self()->middle(Identity).place(new TextLabel);
}

bool EnumerationNamer::isNamed() const
{
	return self()->child(Identity);
}

}
