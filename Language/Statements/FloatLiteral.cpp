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
	if (_p.exists() && _p->isKind<IntegerLiteral>() && _e->text() == ".")
	{
		FloatLiteral* l = new FloatLiteral;
		l->setValue(_p->asKind<IntegerLiteral>()->value());
		_p->replace(l);
		l->setEditing(_e->codeScene());
	}
	else if (_p.exists() && _p->isKind<FloatLiteral>() && _e->text().length() == 1 && _e->text()[0].isNumber())
	{
		_p->asKind<FloatLiteral>()->setValue(_e->text().toInt());
		_p->setEditing(_e->codeScene());
	}
	else
		return false;
	return true;
}

String FloatLiteral::defineLayout(ViewKeys const&) const
{
	return String(m_precision == SinglePrecision ? "^;'%1f'" : m_precision == DoublePrecision ? "^;'%1'" : "^;'%1ld'").arg(m_value);
}

EditDelegateFace* FloatLiteral::newDelegate(CodeScene* _s)
{
	class Delegate: public EditDelegate<FloatLiteral>
	{
	public:
		Delegate(FloatLiteral* _e, CodeScene* _s): EditDelegate<FloatLiteral>(_e, _s), m_entry(String("%1").arg(_e->m_value))
		{
			if (!m_entry.contains("."))
				m_entry += ".";
		}
		virtual bool keyPressed(KeyEvent const* _e)
		{
			if (_e->text() == L"\b" && m_entry.size() > 1)
				m_entry = m_entry.left(m_entry.size() - 1);
			else if ((_e->text().length() == 1 && _e->text()[0].isNumber()) || (_e->text() == "." && !m_entry.contains(".")))
				m_entry += _e->text();
			else if (_e->text() == "f")
				subject()->setPrecision(SinglePrecision);
			else if (_e->text() == "d")
				subject()->setPrecision(DoublePrecision);
			else if (_e->text() == "l")
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
		String defineLayout(ViewKeys const&) const
		{
			return (String("^;ynormal;'%1';ycode;") + (subject()->m_precision == SinglePrecision ? "'f'" : subject()->m_precision == DoublePrecision ? "" : "'ld'")).arg(m_entry);
		}

		String m_entry;
	};
	return new Delegate(this, _s);
}

}
