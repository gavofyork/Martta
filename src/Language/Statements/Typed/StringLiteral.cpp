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

#include "StringLiteral.h"
#include "EditDelegate.h"

namespace Martta
{

MARTTA_OBJECT_CPP(StringLiteral);

bool StringLiteral::keyPressedOnPosition(Position const& _p, EntityKeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text() == "\"")
	{
		Entity* l = _p.place(new StringLiteral);
		l->setEditing(_e->codeScene());
	}
	else
		return false;
	return true;
}

void StringLiteral::importDom(QDomElement const& _element)
{
	Literal::importDom(_element);
	m_value = _element.attribute("value");
}

void StringLiteral::exportDom(QDomElement& _element) const
{
	Literal::exportDom(_element);
	_element.setAttribute("value", m_value);
}

String StringLiteral::defineLayout(ViewKeys const&) const
{
	return String("^;ycode;'\"';ynormal;'%1';ygrayed;'\"'").arg(m_value);
}

bool StringLiteral::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == "\"" && isEditing(_e->codeScene()))
	{
		clearEditing();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

EditDelegateFace* StringLiteral::newDelegate(CodeScene* _s)
{
	class Delegate: public EditDelegate<StringLiteral>
	{
	public:
		Delegate(StringLiteral* _e, CodeScene* _s): EditDelegate<StringLiteral>(_e, _s) {}
		virtual bool keyPressed(EntityKeyEvent const* _e)
		{
			if (_e->key() == Qt::Key_Backspace)
				subject()->m_value.chop(1);
			else if (_e->text()[0].isGraph() && _e->text() != L"\"")
				subject()->m_value += _e->text();
			else
				return false;
			subject()->changed();
			return true;
		}
	};
	return new Delegate(this, _s);
}

}
