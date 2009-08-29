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

#include "IntegerLiteral.h"
#include "EditDelegate.h"
#include "FloatLiteral.h"

namespace Martta
{

MARTTA_OBJECT_CPP(FloatLiteral);

bool FloatLiteral::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isKind<IntegerLiteral>() && _e->text() == L".")
	{
		FloatLiteral* l = new FloatLiteral;
		l->setValue(_p->asKind<IntegerLiteral>()->value());
		_p->replace(l);
		_e->codeScene()->setEditing(l);
	}
	else if (_p.exists() && _p->isKind<FloatLiteral>() && _e->text().length() == 1 && _e->text()[0].isNumber())
	{
		_p->asKind<FloatLiteral>()->setValue(_e->text().toInt());
		_e->codeScene()->setEditing(_p.entity());
	}
	else
		return false;
	return true;
}

String FloatLiteral::defineHtml() const
{
	return String(L"<^span><span class=\"FloatLiteral Literal\">%2</span><span class=\"keyword\">%1</span></span>").arg(m_precision == SinglePrecision ? L"f" : m_precision == DoublePrecision ? L"" : L"ld").arg(m_value);
}

String FloatLiteral::defineEditHtml(CodeScene* _cs) const
{
	return String(L"<^span><span class=\"FloatLiteral Literal\">%2</span><span class=\"keyword\">%1</span></span>").arg(m_precision == SinglePrecision ? L"f" : m_precision == DoublePrecision ? L"" : L"ld").arg(_cs->editDelegate(this)->real());
}

EditDelegateFace* FloatLiteral::newDelegate(CodeScene* _s)
{
	class Delegate: public EditDelegate<FloatLiteral>
	{
	public:
		Delegate(FloatLiteral* _e, CodeScene* _s): EditDelegate<FloatLiteral>(_e, _s, false), m_entry(String::number(_e->m_value))
		{
			if (!m_entry.contains(L'.'))
				m_entry += L".";
		}
		virtual bool keyPressed(KeyEvent const* _e)
		{
			if (_e->text() == BackspaceKey && m_entry.size() > 1)
				m_entry = m_entry.left(m_entry.size() - 1);
			else if ((_e->text().length() == 1 && _e->text()[0].isNumber()) || (_e->text() == "." && !m_entry.contains(".")))
				m_entry += _e->text();
			else if (_e->text() == L"f")
				subject()->setPrecision(SinglePrecision);
			else if (_e->text() == L"d")
				subject()->setPrecision(DoublePrecision);
			else if (_e->text() == L"l")
				subject()->setPrecision(LongDoublePrecision);
			else
				return false;
			return true;
		}
		virtual void commit()
		{
			subject()->setValue(m_entry.toDouble());
		}
		virtual bool isValid() const
		{
			bool ret;
			m_entry.toDouble(&ret);
			return ret;
		}
		virtual String real() const
		{
			return m_entry;
		}

	private:
		String m_entry;
	};
	return new Delegate(this, _s);
}

}
