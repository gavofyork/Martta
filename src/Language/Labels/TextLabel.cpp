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

#include <msString.h>
using MarttaSupport::String;
using MarttaSupport::Char;

#include "CommonGraphics.h"
#include "Variable.h"
#include "Argument.h"
#include "Namespace.h"
#include "TypeDefinition.h"
#include "MemberVariable.h"
#include "LambdaNamer.h"
#include "EditDelegate.h"
#include "DecorationContext.h"
#include "TextLabel.h"

namespace Martta
{

MARTTA_OBJECT_CPP(TextLabel);	

String TextLabel::code() const
{
	if (m_text.isEmpty())
		if (isValid())
			return String("_ANON%1").arg((int)this);
		else
			return String::null;
	else
	{
		String prefix = "";
		if (parent()->hasAncestor<Namespace>())
		{
			if (parentIs<Argument>())
				prefix = "a_";
			else if (parentIs<MemberVariable>())
				prefix = "m_";
			else if (parentIs<VariableNamer>())
				prefix = "l_";
			else
				prefix = "";
		}
		else if (parent()->isKind<Namespace>() && name().size() && name()[0].isNumber())
			prefix = "_";
		else
			prefix = "";
		return prefix + name();
	}
}
	
void TextLabel::importDom(QDomElement const& _element)
{
	Super::importDom(_element);
	m_text = _element.toElement().attribute("text");
}

void TextLabel::exportDom(QDomElement& _element) const
{
	Super::exportDom(_element);
	_element.setAttribute("text", m_text.isEmpty() ? code() : m_text);
}

void TextLabel::apresLoad()
{
	if (m_text.startsWith("_ANON"))
		m_text = String::null;
}

String TextLabel::name() const
{
	if (m_text.isEmpty() && isValid())
		return String("_ANON%1").arg((int)this);//return "foo";	// TODO: make it proper.
	else if (m_text.isEmpty())
		return String::null;
	else if (parentIs<TypeDefinition>() || parentIs<Namespace>())
		return m_text.left(1).toUpper() + camelCase(m_text.mid(1));
	else
		return camelCase(m_text);
}

String TextLabel::defineLayout(ViewKeys const&) const
{
	String key = "";
	if (parent()->hasAncestor<Namespace>())
	{
		if (parentIs<Argument>())
			key = "(;M4;[[[;fs-2;fb;c#777;e#fff;'_';]]];);";
		else if (parentIs<MemberVariable>())
			key = "(;M4;[[[;fs-2;fb;c#777;e#fff;'M';]]];);";
	}
	return "^;" + key + (name().isEmpty() ? "yminor;'[ANONYMOUS]'" : (String(isNamed() ? "c#000" : "c#aaa") + ";'" + name() + "'"));
}
/*
void TextLabel::decorate(DecorationContext const& _c) const
{
	bool dec= false;
	if (parent()->hasAncestor<Namespace>())
	{
		if (parentIs<Variable>() && parent()->parentIs<Member>())
			dec = true;
		else if (parentIs<Argument>())
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
}*/

class Delegate: public EditDelegate<TextLabel>
{
public:
	Delegate(TextLabel* _e, CodeScene* _s): EditDelegate<TextLabel>(_e, _s), m_text(subject()->isNamed() ? subject()->text() : String::null) {}
	void setText(String const& _t) { m_text = _t; }
	virtual bool keyPressed(KeyEvent const* _e)
	{
		if (_e->text() == L"\b")
			m_text = m_text.left(m_text.size() - 1);
		else if (_e->text().length() == 1 && (_e->text()[0].isAlphaNumeric() || _e->text()[0] == L'_'))
			m_text += _e->text();
		else
			return EditDelegate<TextLabel>::keyPressed(_e);
		return true;
	}
	virtual void commit()
	{
		// translate to all-lower, space separated.
		String actual = "";
		bool wordBreak = true;
		for (int i = 0; i < m_text.size(); i++)
		{
			MarttaSupport::Char c = m_text[i];
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
		return true;//!m_text.isEmpty();
	}
	virtual String defineLayout(ViewKeys const&) const
	{
		String key = "";
		if (subject()->parent()->hasAncestor<Namespace>())
		{
			if (subject()->parentIs<Argument>())
				key = "(;M4;[[[;fs-2;fb;c#777;e#fff;'_';]]];)";
			else if (subject()->parentIs<MemberVariable>())
				key = "(;M4;[[[;fs-2;fb;c#777;e#fff;'M';]]];)";
		}
		return "^;" + (m_text.isEmpty() ? String("yminor;'ANONYMOUS'") : (key + ";'" + m_text + "'"));
	}
	String m_text;
};

bool TextLabel::keyPressed(KeyEvent const* _e)
{
	if (_e->isFocused() && _e->text().length() == 1 && _e->text()[0].isLower())
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
