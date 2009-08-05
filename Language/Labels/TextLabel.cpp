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

#include <msString.h>
using MarttaSupport::String;
using MarttaSupport::Char;

#include "Labelled.h"
#include "EditDelegate.h"
#include "WebStylistRegistrar.h"
#include "TextLabel.h"

namespace Martta
{

MARTTA_OBJECT_CPP(TextLabel);

void TextLabel::initialiseClass()
{
	WebStylistRegistrar::get()->registerCss(staticKind.auxilliary(),
		".TextLabel-named { color: #000 }"
		".TextLabel-unnamed { color: #aaa }"
	);
}

void TextLabel::finaliseClass()
{
	WebStylistRegistrar::get()->unregisterCss(staticKind.auxilliary());
}

String TextLabel::code() const
{
	if (m_text.isEmpty())
		if (isValid())
			return String("_ANON%1").arg((int)this);
		else
			return String::null;
	else
		return tryParent<Labelled>()->labelCode(m_text);
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
	else
		return tryParent<Labelled>()->labelName(m_text);
}

String TextLabel::defineHtml() const
{
	if (name().isEmpty())
		return L"<span id=\"this\"><span class=\"minor\">[ANONYMOUS]</span></span>";
	return String("<span id=\"this\"><span class=\"TextLabel-%1\">").arg(isNamed() ? L"named" : L"unnamed") + tryParent<Labelled>()->labelHtml(name()) + L"</span></span>";
}

String TextLabel::defineLayout(ViewKeys const& _k) const
{
	if (name().isEmpty())
		return L"^;yminor;'[ANONYMOUS]'";

	return "^;" + tryParent<Labelled>()->labelLayout(String(isNamed() ? "c#000" : "c#aaa") + ";'" + name() + "'", _k);
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
		else if (_e->text().length() == 1 && (_e->text()[0].isAlphaNumeric() || _e->text()[0] == L'_' || _e->text()[0] == L' '))
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
	virtual String defineLayout(ViewKeys const& _k) const
	{
		// The tryParent()-> should be safe since Labelled checks for a null this-pointer.
		return "^;" + (m_text.isEmpty() ? String("yminor;'ANONYMOUS'") : subject()->tryParent<Labelled>()->labelLayout("'" + m_text + "'", _k));
	}
	virtual String defineHtml() const
	{
		// The tryParent()-> should be safe since Labelled checks for a null this-pointer.
		if (m_text.isEmpty())
			return L"<span class=\"minor\">[ANONYMOUS]</span>";
		return subject()->tryParent<Labelled>()->labelHtml(m_text);
//		return String(L"<span class=\"TextLabel-%1\">").arg(isNamed() ? L"named" : L"unnamed") + subject()->tryParent<Labelled>()->labelHtml(m_text)) + L"</span>";
	}
	String m_text;
};

bool TextLabel::keyPressed(KeyEvent const* _e)
{
	if (_e->isFocused() && _e->text().length() == 1 && _e->text()[0].isLower())
	{
		setEditing(_e->codeScene());
		if (isEditing(_e->codeScene()))
		{
			static_cast<Delegate*>(editDelegate(_e->codeScene()))->setText("");
			_e->reinterpretLater();
		}
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
