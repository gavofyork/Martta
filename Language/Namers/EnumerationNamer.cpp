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

#include "EnumValue.h"
#include "ReferencedType.h"
#include "TextLabel.h"
#include "CodeScene.h"
#include "SpecialKeys.h"
#include "EnumerationNamer.h"

namespace Martta
{

MARTTA_NOTION_CPP(EnumerationNamer);

String EnumerationNamer::interfaceCode() const
{
	String ret;
	ret += "enum " + codeName() + "\n";
	ret += "{\n";
	foreach (EnumValue* f, cardinalChildrenOf<EnumValue>())
		ret += f->code() + ",\n";
	if (ret.endsWith(",\n"))
		ret.chop(2), ret += "\n";
	ret += "};\n";
	return ret;
}

void EnumerationNamer::updateStem()
{
	String oldStem = m_stem;
	m_stem = String::null;
	foreach (EnumValue* i, cardinalChildrenOf<EnumValue>())
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
		changed();
}

bool EnumerationNamer::keyPressed(KeyEvent const* _e)
{
	if ((_e->text() == ReturnKey && !_e->isFocused()) || (_e->text() == L"," && _e->focalIndex() >= 0))
	{
		Position p = (_e->focalIndex() == Identity) ?
		(_e->isInserting() || _e->modifiers() & ShiftModifier) ?
		front() :
		back() :
		middle(_e->focalIndex() + ((_e->isInserting() || _e->modifiers() & ShiftModifier) ? 0 : 1));
		EnumValue* s = new EnumValue;
		s->prepareChildren();
		p.place(s);
		_e->codeScene()->setCurrent(s->child(Identity));
	}
	else if (_e->text() == L"¬" && _e->focalIndex() != UndefinedIndex)
	{
		_e->codeScene()->navigateOnto(child(_e->focalIndex()));
	}
	else if (_e->text() == "H")
	{
		_e->codeScene()->setCurrent(this);
	}
	else if (_e->text().length() == 1 && _e->text()[0].isLower() && !isNamed())
	{
		setNamed();
		_e->codeScene()->setCurrent(child(Identity));
		_e->reinterpretLater();
	}
	else
		return false;
	return true;
}

String EnumerationNamer::defineEnumerationHtml() const
{
	String ret;
	String name;
	if (isNamed())
		name = Type(const_cast<TypeDefinition*>(asKind<TypeDefinition>()))->typeHtml(toHtml(child(Identity), "span class=\"TypeConcept\""));
	else if (!m_stem.isEmpty())
		name = L"<span class=\"unreal\">[" + m_stem + L"...]</span>";
	return L"<span class=\"keyword\">enum</span> " + name + L"<div class=\"minor symbol\">{</div><div class=\"block\">" + toHtml(cardinalChildren(), L"", L"div") + L"</div><div class=\"minor symbol\">}</div>";
/*
	else
	{
		ret += "ycode;'enum'" + name + ";s;yminor;' (";
		int n = cardinalChildCountOf<EnumValue>();
		if (n > 1 || !cardinalChildOf<EnumValue>()->codeName().isEmpty())
			ret += String::number(n) + " entr" + (n > 1 ? "ies" : "y");
		if (ret.endsWith("("))
			ret += "empty";
		ret += ")'";
	}
*/
}

void EnumerationNamer::onDependencyRemoved(Concept* _e)
{
	if (_e->isKind<EnumValue>())
		updateStem();
}

void EnumerationNamer::onDependencyAdded(Concept* _e)
{
	if (_e->isKind<EnumValue>())
		updateStem();
	else
		changed(Dependee::Logically);
}

void EnumerationNamer::onDependencyChanged(int, Concept* _e)
{
	if (_e->isKind<TextLabel>())
	{
		if (!_e->asKind<TextLabel>()->isNamed())
			setUnnamed();
		else
			changed(Dependee::Logically);
	}
	else
		updateStem();
}

void EnumerationNamer::setUnnamed()
{
	if (isNamed())
		child(Identity)->killAndDelete();
	changed();
}

void EnumerationNamer::setNamed()
{
	if (!isNamed())
		middle(Identity).place(new TextLabel);
}

bool EnumerationNamer::isNamed() const
{
	return child(Identity);
}

}
