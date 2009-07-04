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

#include "msSupport.h"
#include "EditDelegate.h"

MS_TEST_METHOD_EXISTANCE(committed)

namespace Martta
{

// Exists: List<R> T::possibilities()
// Exists: void T::set(R)
// Exists: R T::get()
// Works: NameTrait<R>::name(R)
// Exists: QString T::defineEditLayout(ViewKeys const&, R); should contain %1 for where the editing is to be.
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
		::MarttaSupport::committed<T>(*EditDelegate<T>::subject());
	}
	virtual void commit()
	{
		EditDelegate<T>::subject()->set(m_selection);
		EditDelegate<T>::codeScene()->update();
	}
	virtual bool isValid() const
	{
		return (m_name + m_completion).toUpper() == NameTrait<R>::name(m_selection).toUpper();
	}
	virtual String defineLayout(ViewKeys const& _v) const
	{
		String ret = EditDelegate<T>::subject()->defineEditLayout(_v, m_selection);
		if (ret.contains("%1"))
			return ret.arg("'" + m_name + "';s;ygrayed;'" + m_completion + "'");
		else
			return ret;
	}
	virtual bool keyPressed(EntityKeyEvent const* _e)
	{
		if (_e->text() == L"\b" && m_name.size() > 1)
			m_name.chop(1);
		else if (_e->text().length() == 1 && _e->text()[0].isLetter())
			m_name += _e->text();
		else if (_e->text() == L"\t")
			m_name += m_completion;
		else
			return EditDelegate<T>::keyPressed(_e);
		updateCompletion();

		m_selection = 0;
		foreach (R t, m_possibilities)
			if (NameTrait<R>::name(t) == m_name + m_completion)
				m_selection = t;
		return true;
	}
	void updateCompletion()
	{
		m_completion = "";
		List<R> potentials = nameStarts(m_possibilities, m_name);
		if (potentials.size() == 1)
		{
			m_completion = NameTrait<R>::name(potentials[0]).mid(m_name.size());
			m_name = NameTrait<R>::name(potentials[0]).left(m_name.size());
		}
		else if (potentials.size())
		{
			// This will reset any up/down selection should another character be typed; need to check that we have narrowed down the
			// selection before auto-reselecting.
			while (nameStarts(m_possibilities, m_name + m_completion).size() == potentials.size())
				m_completion += NameTrait<R>::name(potentials[0])[m_name.size() + m_completion.size()];
			m_completion.chop(1);
		}
	}
	R							m_selection;
	String						m_name;
	String						m_completion;
	List<R>					m_possibilities;
	bool						m_immediateCommits;
};

}

