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

#include <cmath>

#include "IntegerLiteral.h"
#include "EditDelegate.h"

namespace Martta
{

MARTTA_OBJECT_CPP(IntegerLiteral);

bool IntegerLiteral::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _e->text().length() == 2 && _e->text()[0] == L'-' && _e->text()[1].isNumber() && _p->isPlaceholder())
	{
		IntegerLiteral* l = new IntegerLiteral(_e->text().toInt());
		_e->noteStrobeCreation(l, _p.entity());
		_p.place(l);
		_e->codeScene()->setEditing(l);
	}
	else if (_p.exists() && _p->isPlaceholder() && _e->text().length() == 1 && _e->text()[0].isNumber())
	{
		IntegerLiteral* l = new IntegerLiteral(_e->text().toInt());
		_p.place(l);
		_e->codeScene()->setEditing(l);
	}
	else if (_p.exists() && _p->isKind<IntegerLiteral>() && _e->text().length() == 1 && _e->text()[0].isNumber())
	{
		_p->asKind<IntegerLiteral>()->setValue(_e->text().toDouble());
		_e->codeScene()->setEditing(_p.entity());
	}
	else
		return false;
	return true;
}

String IntegerLiteral::defineHtml() const
{
	double value;
	modf(m_value, &value);
	String ret = (value > 0 || value < 0) ? L"" : L",0";
	for (int v = (int)(value < 0.0 ? -value : value); v >= 1; v /= 1000)
		ret = (L",%1" + ret).arg((uint)fmod((double)v, 1000.0), v >= 1000 ? 3 : 0, 10, '0');
	ret = ret.mid(1);
	if (m_signed)
		ret = ((m_value < 0.0) ? "-" : "") + ret;
	return String(L"<^><span class=\"IntegerLiteral Literal\">%1</span><span class=\"keyword\">%2%3</span>").arg(ret).arg(m_signed ? L"" : L"u").arg(m_range == ShortRange ? L"s" : m_range == LongRange ? L"l" : m_range == LonglongRange ? L"ll" : L"");
}

String IntegerLiteral::defineEditHtml(CodeScene* _cs) const
{
	if (EditDelegateFace* d = _cs->editDelegate(this))
		return String(L"<span class=\"Literal IntegerLiteral\">%1<span class=\"unreal\">%4</span></span><span class=\"keyword\">%2%3</span>").arg(d->real()).arg(m_signed ? "" : "u").arg((m_range == ShortRange ? L"s" : m_range == LongRange ? L"l" : m_range == LonglongRange ? L"ll" : L"")).arg(d->unreal());
	return String::null;
}

EditDelegateFace* IntegerLiteral::newDelegate(CodeScene* _s)
{
	class Delegate: public EditDelegate<IntegerLiteral>
	{
	public:
		Delegate(IntegerLiteral* _e, CodeScene* _s): EditDelegate<IntegerLiteral>(_e, _s), m_entry(String::number(_e->m_value))
		{
			if (!subject()->m_signed)
				m_entry.remove("-");
		}
		virtual bool keyPressed(KeyEvent const* _e)
		{
			if (_e->text() == BackspaceKey && m_entry.size())
				m_entry = m_entry.left(m_entry.size() - 1);
			else if (_e->text().length() == 1 && _e->text()[0].isNumber())
				m_entry += _e->text();
			else if (_e->text() == L"s" && subject()->range() == ShortRange)
				subject()->setRange(NaturalRange);
			else if (_e->text() == L"s")
				subject()->setRange(ShortRange);
			else if (_e->text() == L"l" && subject()->range() == LongRange)
				subject()->setRange(LonglongRange);
			else if (_e->text() == L"l" && subject()->range() == LonglongRange)
				subject()->setRange(NaturalRange);
			else if (_e->text() == L"l")
				subject()->setRange(LongRange);
			else if (_e->text() == L"u")
			{
				subject()->setSignedness(!subject()->signedness());
				if (!subject()->m_signed)
					m_entry.remove(L"-");
			}
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
			m_entry.toInt(&ret);
			return ret;
		}
		virtual String real() const
		{
			return m_entry;
		}
		virtual String unreal() const
		{
			return m_entry.isEmpty() ? L"0" : String::null;
		}

	private:
		String m_entry;
	};
	return new Delegate(this, _s);
}

}
