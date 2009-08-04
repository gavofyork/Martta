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
#include <msList.h>
using namespace MarttaSupport;

namespace Martta
{

class SafePointerTarget;
template<class T> class SafePointer;

class SafePointerFace
{
	friend class SafePointerTarget;
	template<class T> friend class SafePointer;

protected:
	inline SafePointerFace(SafePointerTarget* _e = 0): m_pointer(0) { set(_e); }
	inline ~SafePointerFace() { set(0); }
	inline void set(SafePointerTarget* _e);

private:
	SafePointerTarget*		m_pointer;
};

template<class T>
class SafePointer: public SafePointerFace
{
	friend class SafePointerTarget;

public:
	inline SafePointer(T* _e = 0): SafePointerFace(_e) {}
	inline SafePointer(SafePointer<T> const& _s): SafePointerFace(_s.m_pointer) {}
	inline SafePointer& operator=(SafePointer<T> const& _s) { set(_s.m_pointer); return *this; }
	inline SafePointer& operator=(T* _e) { set(_e); return *this; }

	inline operator T*() const { return get(); }
	inline T* operator->() const { return get(); }
	inline T& operator*() const { return *get(); }

private:
	inline T* get() const { return static_cast<T*>(m_pointer); }
};

class SafePointerTarget
{
	friend class SafePointerFace;

protected:
	void rewirePointer(SafePointerTarget* _sub)
	{
		if (_sub == this || !_sub)
			return;

		M_FOREACH (SafePointerFace* i, m_safePointers)
		{
			i->m_pointer = _sub;
			_sub->m_safePointers << i;
		}
		m_safePointers.clear();
	}

	inline ~SafePointerTarget()
	{
		foreach (SafePointerFace* i, m_safePointers)
			i->m_pointer = 0;
	}
	List<SafePointerFace*>					m_safePointers;
};

void SafePointerFace::set(SafePointerTarget* _e)
{
	if (m_pointer == _e)
		return;
	if (m_pointer)
		m_pointer->m_safePointers.removeAll(this);
	m_pointer = _e;
	if (m_pointer)
		m_pointer->m_safePointers.append(this);
}

}
