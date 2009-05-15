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
#include "AccessLabel.h"
#include "TextLabel.h"
#include "EnumValue.h"
#include "Enumeration.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Enumeration);	
	
bool Enumeration::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (simpleInsertionPointKeyPressHandler<Enumeration>(_p, _e, "E"))
	{	if (_p.exists() && _p->isKind<Enumeration>() && _p->entityIs<AccessLabel>(1) && _p->contextIs<Class>())
			_p->entityAs<AccessLabel>(1)->setAccess(Private);
		return true;
	}
	return false;
}

Types Enumeration::assignableTypes() const
{
	return Type(const_cast<Enumeration*>(this));
}

Kinds Enumeration::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<TextLabel>();
	else if (_i == 1)
		return Kind::of<AccessLabel>();
	else
		return Kind::of<EnumValue>();
}

Access Enumeration::access() const
{
	return entityIs<AccessLabel>(1) ? entityAs<AccessLabel>(1)->access() : NoAccess;
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
	QString oldStem = m_stem;
	m_stem = QString();
	foreach (EnumValue* i, entitiesOf<EnumValue>())
		if (m_stem.isEmpty())
			m_stem = i->codeName();
		else
			while (m_stem != i->codeName().left(m_stem.length()))
				m_stem.chop(1);
	if (oldStem != m_stem)
		changed();
}

bool Enumeration::keyPressed(EntityKeyEvent const* _e)
{
	M_ASSERT(isComplete());
	if (_e->key() == Qt::Key_Return)
	{
		InsertionPoint p = (_e->isFocused() || _e->focalIndex() == 0) ?
		(/*_e->inserting() || */_e->modifiers() & Qt::ShiftModifier) ?
		middle(1) :
		back() :
		middle(_e->focalIndex() + ((/*_e->inserting() || */_e->modifiers() & Qt::ShiftModifier) ? 0 : 1));
		EnumValue* s = new EnumValue;
		s->prepareChildren();
		p.place(s);
		s->entity(0)->setCurrent();
	}
	else if (entitiesOf<AccessLabel>().size() && entitiesOf<AccessLabel>()[0]->asKind<Label>()->keyPressed(_e))
		return true;
	else if (_e->key() == Qt::Key_Home && _e->focalIndex() > -1)
	{
		entity(_e->focalIndex())->setCurrent();
	}
	else if (_e->text() == "H")
	{
		setCurrent();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

QString Enumeration::defineLayout(ViewKeys& _viewKeys) const
{
	QString ret = (access() == NoAccess ? "" : "m24,0,0,0;");
	if (_viewKeys["expanded"].toBool())
	{
		ret += "^;ycode;'enum ';fb;cblack;s" + Type(const_cast<Enumeration*>(this))->idColour() + ";!0;s;ycode;n;'{'";
		foreach (EnumValue* f, entitiesOf<EnumValue>())
			ret += QString(";n;i;%1").arg(f->contextIndex());
		ret += ";n;'}'";
	}
	else
	{
		ret += "^;ycode;'enum ';fb;cblack;s" + Type(const_cast<Enumeration*>(this))->idColour() + ";!0;s;yminor;' (";
		int n = entitiesOf<EnumValue>().count();
		if (n > 1 || !entitiesOf<EnumValue>()[0]->codeName().isEmpty())
			ret += QString::number(n) + " entr" + (n > 1 ? "ies" : "y");
		if (ret.endsWith("("))
			ret += "empty";
		ret += ")'";
	}
	return ret;
}

void Enumeration::decorate(DecorationContext const& _p) const
{
	if (access() != NoAccess)
	{
		_p->setPen(Qt::NoPen);
		QColor c = AccessLabel::idColour(access());
		c.setAlpha(64);
		_p->setBrush(c);
		_p->drawRect(QRectF(16.f, 0.f, 4.f, _p.cap(0).height()));
	}
	Super::decorate(_p);
}

}
