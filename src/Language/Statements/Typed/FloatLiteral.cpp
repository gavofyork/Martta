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

#include "IntegerLiteral.h"
#include "EditDelegate.h"
#include "FloatLiteral.h"

namespace Martta
{

MARTTA_OBJECT_CPP(FloatLiteral);

bool FloatLiteral::keyPressedOnPosition(Position const& _p, EntityKeyEvent const* _e)
{
	if (_p.exists() && _p->isKind<IntegerLiteral>() && _e->text() == ".")
	{
		FloatLiteral* l = new FloatLiteral;
		l->setValue(_p->asKind<IntegerLiteral>()->value());
		_p->replace(l);
		l->setEditing(_e->codeScene());
	}
	else if (_p.exists() && _p->isKind<FloatLiteral>() && QRegExp("[0-9]").exactMatch(_e->text()))
	{
		_p->asKind<FloatLiteral>()->setValue(_e->text().toInt());
		_p->setEditing(_e->codeScene());
	}
	else
		return false;
	return true;
}

QString FloatLiteral::defineLayout(ViewKeys&) const
{
	return QString(m_precision == SinglePrecision ? "^;'%1f'" : m_precision == DoublePrecision ? "^;'%1'" : "^;'%1ld'").arg(m_value);
}

void FloatLiteral::exportDom(QDomElement& _element) const
{
	Literal::exportDom(_element);
	_element.setAttribute("value", m_value);
	_element.setAttribute("precision", (int)m_precision);
}

void FloatLiteral::importDom(QDomElement const& _element)
{
	Literal::importDom(_element);
	m_value = _element.attribute("value").toDouble();
	m_precision = (Precision)_element.attribute("precision").toInt();
}

EditDelegateFace* FloatLiteral::newDelegate(CodeScene* _s)
{
	class Delegate: public EditDelegate<FloatLiteral>
	{
	public:
		Delegate(FloatLiteral* _e, CodeScene* _s): EditDelegate<FloatLiteral>(_e, _s), m_entry(QString("%1").arg(_e->m_value))
		{
			if (!m_entry.contains("."))
				m_entry += ".";
		}
		virtual bool keyPressed(EntityKeyEvent const* _e)
		{
			if (_e->key() == Qt::Key_Backspace && m_entry.size() > 1)
				m_entry = m_entry.left(m_entry.size() - 1);
			else if (QRegExp("[0-9]").exactMatch(_e->text()) || _e->text() == "." && !m_entry.contains("."))
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
		QString defineLayout(ViewKeys&) const
		{
			return (QString("^;ynormal;'%1';ycode;") + (subject()->m_precision == SinglePrecision ? "'f'" : subject()->m_precision == DoublePrecision ? "" : "'ld'")).arg(m_entry);
		}
		
		QString m_entry;
	};
	return new Delegate(this, _s);
}

}
