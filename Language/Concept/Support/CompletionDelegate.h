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

M_TEST_METHOD_EXISTANCE_2(committed)

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

// Exists: List<R> T::possibilities()
// Exists: void T::set(R)
// Exists: R T::get()
// Works: NameTrait<R>::name(R)
// Might exist: T::committed()

template<class T, class R>
class CompletionDelegate: public EditDelegate<T>
{
public:
	CompletionDelegate(T* _e, CodeScene* _s, bool _allowEmpty = true):
		EditDelegate<T>		(_e, _s, _e->isPlaceholder()),
		m_selection			(EditDelegate<T>::subject()->get()),
		m_name				(NameTrait<R>::name(m_selection)),
		m_cycled			(-1),
		m_allowEmpty		(_allowEmpty)
	{
		m_possibilities = EditDelegate<T>::subject()->possibilities();
		updateCompletion();
	}
	R selection() const { return m_selection; }
	virtual void leavingEditIntact()
	{
		mDebug() << "leaving edit intact with " << m_selection;
		committed<T, R, CodeScene*>(*EditDelegate<T>::subject(), m_selection, EditDelegate<T>::codeScene());
	}
	virtual void commit()
	{
		EditDelegate<T>::subject()->set(m_selection);
	}
	virtual bool isValid() const
	{
		return m_cycled > -1 || (m_name + m_completion).toUpper() == NameTrait<R>::name(m_selection).toUpper();
	}
	virtual String real() const
	{
		return m_selection ? NameTrait<R>::name(m_selection).left(m_name.length()) : m_name;
	}
	virtual String unreal() const
	{
		return (m_cycled >= 0 && m_cycled < m_potentials.size() ? NameTrait<R>::name(m_potentials[m_cycled]).mid(m_name.length()) : m_completion);
	}
	virtual void* current() const 
	{
		return reinterpret_cast<void*>(m_selection);
	}
	virtual String comment() const
	{
		if (m_potentials.count() == 0)
			return L"?!";
		else if (m_potentials.count() > 1)
			return String(L" (of %1)").arg(m_potentials.count());
		return String::null;
	}
	virtual bool keyPressed(KeyEvent const* _e)
	{
		bool resetCycled = true;
		if (_e->text() == BackspaceKey && _e->modifiers() == ControlModifier && m_name.size() > 1)
			m_name.chop(1);
		else if (_e->text() == BackspaceKey && _e->modifiers() != ControlModifier && nameStarts(m_possibilities, m_name.left(1)).size() != m_potentials.size())
		{
			int potentials = m_potentials.size();
			while (nameStarts(m_possibilities, m_name).size() == potentials && m_name.length())
				m_name.chop(1);
		}
		else if (_e->text() == BackspaceKey && m_allowEmpty && !m_name.isEmpty())
			m_name.clear();
		else if (_e->text() == BackspaceKey)
		{
			CodeScene* cs = EditDelegate<T>::codeScene();
			cs->rememberCurrent();
			EditDelegate<T>::subject()->deleteAndRefill();
			cs->restoreCurrent();
			return true;
		}
		else if (_e->text().length() == 1 && _e->text()[0].isAlphaNumeric())
			m_name += _e->text();
		else if (_e->text() == TabKey)
			if (m_completion.isEmpty())
//				if (m_potentials.size() && m_cycled >= m_potentials.size() - 1)
					m_cycled = (m_cycled + 1 + m_potentials.size() + 1 + (_e->modifiers() == ShiftModifier ? -1 : 1)) % (m_potentials.size() + 1) - 1, resetCycled = false;
//				else{}
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

private:
	R							m_selection;
	String						m_name;
	String						m_completion;
	int							m_cycled;
	List<R>						m_possibilities;
	List<R>						m_potentials;
	bool						m_allowEmpty;
};

}

