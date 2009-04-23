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

#include "CommonGraphics.h"
#include "Variable.h"
#include "NamespaceEntity.h"
#include "Class.h"
#include "Callable.h"
#include "EditDelegate.h"
#include "DecorationContext.h"
#include "TextLabel.h"

namespace Martta
{

MARTTA_OBJECT_CPP(TextLabel);	

QString TextLabel::code() const
{
	QString prefix = "";
	if (context()->hasAncestor<NamespaceEntity>())
	{
		if (contextIs<Variable>())
		{
			if (context()->contextIs<Class>())
				prefix = "m_";
			else if (context()->contextIs<Callable>())
				prefix = "a_";
			else
				prefix = "l_";
		}
		else
			prefix = "";
	}
	else if (context()->isKind<NamespaceEntity>() && name().size() && name()[0].isNumber())
		prefix = "_";
	else
		prefix = "";
	return prefix + name();
}
	
void TextLabel::importDom(QDomElement const& _element)
{
	Super::importDom(_element);
	m_text = _element.toElement().attribute("text");
}

void TextLabel::exportDom(QDomElement& _element) const
{
	Super::exportDom(_element);
	_element.setAttribute("text", m_text);
}

QString TextLabel::name() const
{
	if (contextIs<Class>() || contextIs<NamespaceEntity>())
		return m_text.left(1).toUpper() + camelCase(m_text.mid(1));
	else
		return camelCase(m_text);
}

QString TextLabel::defineLayout(ViewKeys&) const
{
	QString key = "";
	if (context()->hasAncestor<NamespaceEntity>())
	{
		if (contextIs<Variable>() && context()->contextIs<Class>())
			key = "(;M4;[[[;fs-2;fb;c#777;e#fff;'M';]]];)";
		else if (contextIs<Variable>() && context()->contextIs<Callable>())
			key = "(;M4;[[[;fs-2;fb;c#777;e#fff;'_';]]];)";
	}
	return "^;" + (text().isEmpty() ? QString("ygrayed;'<unnamed>'") : (key + ";'" + name() + "'"));
}

void TextLabel::decorate(DecorationContext const& _c) const
{
	bool dec= false;
	if (context()->hasAncestor<NamespaceEntity>())
	{
		if (contextIs<Variable>() && context()->contextIs<Class>())
			dec = true;
		else if (contextIs<Variable>() && context()->contextIs<Callable>())
			dec = true;
	}
	if (dec)
	{
		QRectF r = alignedForUnitPen(_c(1));
		r.setWidth(qMin(_c(0).width(), r.height() * 2));

		QRgb c = qRgb(0, 0, 0);
	
		QRadialGradient go(_c(1).center(), r.height() * 2);
		go.setColorAt(0.f, qRgba(c, 32));
		go.setColorAt(1.f, qRgba(c, 0));
		_c->setPen(Qt::NoPen);
		_c->setBrush(go);
		_c->drawRoundRect(r, 50, 100);
	}
}

class Delegate: public EditDelegate<TextLabel>
{
public:
	Delegate(TextLabel* _e, CodeScene* _s): EditDelegate<TextLabel>(_e, _s), m_text(subject()->name()) {}
	void setText(QString const& _t) { m_text = _t; }
	virtual bool keyPressed(EntityKeyEvent const* _e)
	{
		if (_e->key() == Qt::Key_Backspace)
			m_text = m_text.left(m_text.size() - 1);
		else if (QRegExp("[A-Za-z0-9_ ]").exactMatch(_e->text()))
			m_text += _e->text();
		else
			return EditDelegate<TextLabel>::keyPressed(_e);
		return true;
	}
	virtual void commit()
	{
		// translate to all-lower, space separated.
		QString actual = "";
		bool wordBreak = true;
		for (int i = 0; i < m_text.size(); i++)
		{
			QChar c = m_text[i];
			if (c.isLetter())
			{
				if (c.isUpper() && i && m_text[i-1].isLower())
				{
					actual += " ";
					wordBreak = true;
				}
				else
					wordBreak = false;
				actual += c.toLower();
			}
			else if (c.isNumber())
			{
				actual += c;
				wordBreak = false;
			}
			else if ((c.isSpace() || c == '_') && !wordBreak)
			{
				actual += " ";
				wordBreak = true;
			}
		}
		subject()->setText(actual.simplified().trimmed());
	}
	virtual bool isValid() const
	{
		return !m_text.isEmpty();
	}
	virtual QString defineLayout(ViewKeys&) const
	{
		return "^;'" + m_text + "'";
	}
	QString m_text;
};

bool TextLabel::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->isFocused() && QRegExp("[a-z]").exactMatch(_e->text()))
	{
		setEditing(_e->codeScene());
		static_cast<Delegate*>(editDelegate(_e->codeScene()))->setText("");
		_e->reinterpretLater();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

EditDelegateFace* TextLabel::newDelegate(CodeScene* _s)
{
	return new Delegate(this, _s);
}

}
