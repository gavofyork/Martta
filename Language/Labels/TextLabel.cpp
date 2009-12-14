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
using namespace MarttaSupport;

#include "Labelled.h"
#include "EditDelegate.h"
#include "TextLabel.h"

namespace Martta
{

MARTTA_PROPER_CPP(TextLabel);
MARTTA_REGISTER_CSS(TextLabel,
	".TextLabel-named { color: #000 }"
	".TextLabel-unnamed { color: #aaa }"
);

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

int TextLabel::familyDependencies() const
{
	return parentIs<Labelled>() ? DependsOnParent : DependsOnNothing;
}

String TextLabel::namey() const
{
	if (m_text.isEmpty() && isValid())
		return String("ANON%1").arg((int)this);//return "foo";	// TODO: make it proper.
	else if (m_text.isEmpty())
		return String::null;
	else
		return m_text;
}

void TextLabel::onDependencyChanged(int _a, Concept* _e)
{
	SafePointer<Concept> sp = this;
	if (_a == Logically)
		changed(Visually);
	if (!sp)
		Assert(false, "TextLabel has been deleted on a notification of its change.");
	else
		Super::onDependencyChanged(_a, _e);
}

String TextLabel::defineHtml() const
{
	if (namey().isEmpty())
		return L"<^><span class=\"minor\">[ANONYMOUS]</span>";
	return L"<^>" + String(L"<span class=\"TextLabel-%1\">").arg(isNamed() ? L"named" : L"unnamed") + tryParent<Labelled>()->labelHtml(namey()) + L"</span>";
}

String TextLabel::defineEditHtml(CodeScene* _cs) const
{
	if (EditDelegateFace* d = _cs->editDelegate(this))
	{
		// The tryParent()-> should be safe since Labelled checks for a null this-pointer.
		if (d->real().isEmpty())
			return L"<span class=\"minor\">[ANONYMOUS]</span>";
		return L"<span class=\"TextLabel-named\">" + tryParent<Labelled>()->labelHtml(d->real()) + L"</span>";
	}
	return String::null;
}

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
	virtual String real() const
	{
		return m_text;
	}

private:
	String m_text;
};

bool TextLabel::keyPressed(KeyEvent const* _e)
{
	if (_e->isFocused() && _e->text().length() == 1 && _e->text()[0].isLower())
	{
		_e->codeScene()->setEditing(this);
		if (_e->codeScene()->isEditing(this))
		{
			static_cast<Delegate*>(_e->codeScene()->editDelegate(this))->setText("");
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
