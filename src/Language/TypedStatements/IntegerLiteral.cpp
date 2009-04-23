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

#include <QtXml>

#include "IntegerLiteral.h"
#include "EditDelegate.h"

namespace Martta
{

MARTTA_OBJECT_CPP(IntegerLiteral);

bool IntegerLiteral::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (_p.exists() && QRegExp("-[0-9]").exactMatch(_e->text()) && _p->isPlaceholder())
	{
		IntegerLiteral* l = new IntegerLiteral;
		_p.place(l);
		l->setValue(_e->text().toDouble());
		l->setEditing(_e->codeScene());
	}
	else if (_p.exists() && _p->isPlaceholder() && QRegExp("[0-9]").exactMatch(_e->text()))
	{
		IntegerLiteral* l = new IntegerLiteral;
		_p.place(l);
		l->setValue(_e->text().toDouble());
		l->setEditing(_e->codeScene());
	}
	else if (_p.exists() && _p->isKind<IntegerLiteral>() && QRegExp("[0-9]").exactMatch(_e->text()))
	{
		_p->asKind<IntegerLiteral>()->setValue(_e->text().toDouble());
		_p->setEditing(_e->codeScene());
	}
	else
		return false;
	return true;
}

QString IntegerLiteral::defineLayout(ViewKeys&) const
{
	double value;
	modf(m_value, &value);
	QString ret = (value > 0 || value < 0) ? "" : ",0";
	for (int v = value < 0 ? -value : value; v >= 1; v /= 1000)
		ret = (",%1" + ret).arg((int)fmod(v, 1000), v >= 1000 ? 3 : 0, 10, QChar('0'));
	ret = ret.mid(1);
	if (m_signed)
		ret = (m_value < 0 ? "-" : "") + ret;
	return QString("^;ynormal;'%1';ycode;'%2';'%3'").arg(ret).arg(m_signed ? "" : "u").arg(m_range == ShortRange ? "s" : m_range == LongRange ? "l" : m_range == LonglongRange ? "ll" : "");
}

void IntegerLiteral::exportDom(QDomElement& _element) const
{
	Literal::exportDom(_element);
	_element.setAttribute("value", m_value);
	_element.setAttribute("range", (int)m_range);
}

void IntegerLiteral::importDom(QDomElement const& _element)
{
	Literal::importDom(_element);
	m_value = _element.attribute("value").toDouble();
	m_range = (Range)_element.attribute("range").toInt();
}

EditDelegateFace* IntegerLiteral::newDelegate(CodeScene* _s)
{
	class Delegate: public EditDelegate<IntegerLiteral>
	{
	public:
		Delegate(IntegerLiteral* _e, CodeScene* _s): EditDelegate<IntegerLiteral>(_e, _s), m_entry (QString::number(_e->m_value))
		{
			if (!subject()->m_signed)
				m_entry.remove("-");
		}
		virtual bool keyPressed(EntityKeyEvent const* _e)
		{
			if (_e->key() == Qt::Key_Backspace && m_entry.size() > 1)
				m_entry = m_entry.left(m_entry.size() - 1);
			else if (QRegExp("[0-9]").exactMatch(_e->text()))
				m_entry += _e->text();
			else if (_e->text() == "s" && subject()->range() == ShortRange)
				subject()->setRange(NaturalRange);
			else if (_e->text() == "s")
				subject()->setRange(ShortRange);
			else if (_e->text() == "l" && subject()->range() == LongRange)
				subject()->setRange(LonglongRange);
			else if (_e->text() == "l" && subject()->range() == LonglongRange)
				subject()->setRange(NaturalRange);
			else if (_e->text() == "l")
				subject()->setRange(LongRange);
			else if (_e->text() == "u")
			{
				subject()->setSignedness(!subject()->signedness());
				if (!subject()->m_signed)
					m_entry.remove("-");
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
		virtual QString defineLayout(ViewKeys&) const
		{
			QString ret;
			return ret + QString("^;ynormal;'%1';ycode;'%2';'%3'").arg(m_entry).arg(subject()->m_signed ? "" : "u").arg((subject()->m_range == ShortRange ? "s" : subject()->m_range == LongRange ? "l" : subject()->m_range == LonglongRange ? "ll" : ""));
		}
		QString m_entry;
	};
	return new Delegate(this, _s);
}

}
