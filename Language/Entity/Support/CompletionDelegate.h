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

#pragma once

#include <msSupport.h>
#include <msStringList.h>
using namespace MarttaSupport;

#include "KeyEvent.h"
#include "CodeScene.h"
#include "Position.h"
#include "SpecialKeys.h"
#include "EditDelegate.h"

MS_TEST_METHOD_EXISTANCE_1(committed)

namespace Martta
{

template<class T>
class NameTrait
{
public:
	static String name(T _val) { return _val ? _val->name() : String(); }
};

template<class S> inline List<S> nameStarts(List<S> const& _l, String const& _s)
{
	List<S> ret;
	foreach (S i, _l)
		if (NameTrait<S>::name(i).toLower().startsWith(_s.toLower()))
			ret << i;
	return ret;
}

// Exists: List<R> T::possibilities()
// Exists: void T::set(R)
// Exists: R T::get()
// Works: NameTrait<R>::name(R)
// Exists: String T::defineEditHtml(R); should contain <?> for where the editing is to be.
// Might exist: T::committed()

template<class T, class R>
class CompletionDelegate: public EditDelegate<T>
{
public:
	CompletionDelegate(T* _e, CodeScene* _s): EditDelegate<T>(_e, _s), m_selection(EditDelegate<T>::subject()->get()), m_name(NameTrait<R>::name(m_selection))
	{
		m_immediateCommits = _e->isPlaceholder();
		m_possibilities = EditDelegate<T>::subject()->possibilities();
		updateCompletion();
	}
	virtual void leavingEditIntact()
	{
		mDebug() << "leaving edit intact with " << m_selection;
		committed<T, R>(*EditDelegate<T>::subject(), m_selection);
	}
	virtual void commit()
	{
		EditDelegate<T>::subject()->set(m_selection);
		EditDelegate<T>::codeScene()->repaint(EditDelegate<T>::subject());
	}
	virtual bool isValid() const
	{
		return m_cycled > -1 || (m_name + m_completion).toUpper() == NameTrait<R>::name(m_selection).toUpper();
	}
	virtual String defineHtml() const
	{
		String ret = EditDelegate<T>::subject() ? EditDelegate<T>::subject()->defineEditHtml(m_selection) : L"<?>";
		if (ret.contains(L"<?>"))
			return ret.replace(L"<?>", (m_selection ? NameTrait<R>::name(m_selection).left(m_name.length()) : m_name) + "<span class=\"unreal\">" + (m_cycled >= 0 && m_cycled < m_potentials.size() ? NameTrait<R>::name(m_potentials[m_cycled]).mid(m_name.length()) : m_completion) + "</span>");
		else
			return ret;
	}
	virtual bool keyPressed(KeyEvent const* _e)
	{
		bool resetCycled = true;
		if (_e->text() == L"\b" && _e->modifiers() == ControlModifier && m_name.size() > 1)
			m_name.chop(1);
		else if (_e->text() == L"\b" && _e->modifiers() != ControlModifier && nameStarts(m_possibilities, m_name.left(1)).size() != m_potentials.size())
		{
			int potentials = m_potentials.size();
			while (nameStarts(m_possibilities, m_name).size() == potentials && m_name.length())
				m_name.chop(1);
		}
		else if (_e->text() == L"\b")
		{
			CodeScene* cs = EditDelegate<T>::codeScene();
			Position p = EditDelegate<T>::subject()->over();
			EditDelegate<T>::subject()->deleteAndRefill();
			if (p.exists())
				cs->setCurrent(p.entity());
			return true;
		}
		else if (_e->text().length() == 1 && _e->text()[0].isLetter())
			m_name += _e->text();
		else if (_e->text() == L"\t")
			if (m_completion.isEmpty())
				if (m_potentials.size() && m_cycled < m_potentials.size() - 1)
					m_cycled++, resetCycled = false;
				else{}
			else
				m_name += m_completion;
		else
			return EditDelegate<T>::keyPressed(_e);
		updateCompletion();
		if (resetCycled)
			m_cycled = -1;

		m_selection = 0;
		if (m_cycled > -1)
			m_selection = m_potentials[m_cycled];
		else
			foreach (R t, m_possibilities)
				if (NameTrait<R>::name(t) == m_name + m_completion)
					m_selection = t;
		return true;
	}
	void updateCompletion()
	{
		m_completion = "";
		m_potentials.clear();
		m_potentials = nameStarts(m_possibilities, m_name);
		if (m_potentials.size() == 1)
		{
			m_completion = NameTrait<R>::name(m_potentials[0]).mid(m_name.size());
			m_name = NameTrait<R>::name(m_potentials[0]).left(m_name.size());
		}
		else if (m_potentials.size())
		{
			// This will reset any up/down selection should another character be typed; need to check that we have narrowed down the
			// selection before auto-reselecting.
			while (nameStarts(m_possibilities, m_name + m_completion).size() == m_potentials.size())
				m_completion += NameTrait<R>::name(m_potentials[0])[m_name.size() + m_completion.size()];
			m_completion.chop(1);
		}
	}
	R							m_selection;
	String						m_name;
	String						m_completion;
	int							m_cycled;
	List<R>						m_possibilities;
	List<R>						m_potentials;
	bool						m_immediateCommits;
};

}

