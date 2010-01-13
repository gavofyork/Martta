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

#include "WebStylist.h"
#include "Comment.h"

namespace Martta
{

MARTTA_PROPER_CPP(Comment);
MARTTA_REGISTER_CSS(Comment, ".Comment { background-color: #ffc; color: #553; font-weight: normal; }");

bool Comment::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _p.allowedToBeKind<Comment>() && _e->text() == L"//")
	{
		Concept* l = _p.place(new Comment);
		_e->codeScene()->setEditing(l);
	}
	else
		return false;
	return true;
}

String Comment::defineHtml() const
{
	if (m_value.contains(L'\n'))
		return mark() + tagOf(L"Comment", tagOf(L"minor symbol", L"/* ") + htmlEscape(m_value).replace(L"\n", L"<br/>") + tagOf(L"minor symbol", L"*/"), "div");
	else
		return mark() + tagOf(L"Comment", tagOf(L"minor symbol", L"// ") + htmlEscape(m_value), "div");
}

bool Comment::keyPressed(KeyEvent const* _e)
{
	return Super::keyPressed(_e);
}

EditDelegateFace* Comment::newDelegate(CodeScene* _s)
{
	class Delegate: public EditDelegate<Comment>
	{
	public:
		Delegate(Comment* _e, CodeScene* _s): EditDelegate<Comment>(_e, _s) {}
		virtual bool keyPressed(KeyEvent const* _e)
		{
			if (_e->text() == BackspaceKey && subject()->m_value.length())
				subject()->m_value.chop(1);
			else if (_e->text()[0].isGraph() || _e->text()[0] == L'\n' || _e->text()[0] == L' ')
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
