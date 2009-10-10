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

#include "Position.h"
#include "AuxilliaryFace.h"
#include "AuxilliaryRegistrar.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

template<class T> struct GetCount
{
	typedef typename T::template AltSuperCount<> X;
	enum { Value = X::count };
};

template<class T>
class NotionAuxilliary: public AuxilliaryFace
{
public:
	inline NotionAuxilliary(char const* _name): m_name(_name) { AuxilliaryRegistrar::get()->registerAuxilliary(this); }
	virtual inline bool						isInterface() const { return true; }
	virtual inline bool						isPlaceholder() const { return false; }	// Undefined.
	virtual inline char const*				name() const { return m_name; }
	virtual inline bool						dispatchKeyPress(Position const&, KeyEvent const*) const { return false; }
	virtual inline void						initialise() const {}
	virtual inline void						finalise() const {}
	virtual inline AuxilliaryFace const*	superAuxilliary() const { return 0; }
	virtual inline AuxilliaryFace const*	interfaceAuxilliary(int _i) const { return T::template ASHelper<GetCount<T>::Value>::altSupers()[_i]; }
	virtual inline int						interfaceAuxilliaryCount() const { return GetCount<T>::Value; }
	virtual inline Concept*					create() const { return 0; }

private:
	char const*								m_name;
};

template<class T>
class Auxilliary: public NotionAuxilliary<T>
{
public:
	inline Auxilliary(char const* _name, bool _isPlaceholder): NotionAuxilliary<T>(_name), m_isPlaceholder(_isPlaceholder) {}
	virtual inline bool						isInterface() const { return false; }
	virtual inline bool						isPlaceholder() const { return m_isPlaceholder; }
	virtual inline bool						dispatchKeyPress(Position const& _p, KeyEvent const* _e) const;
	virtual inline void						initialise() const { T::initialiseClass(); }
	virtual inline void						finalise() const { T::finaliseClass(); }
	virtual inline AuxilliaryFace const*	superAuxilliary() const { return T::Super::staticAuxilliary(); }
	virtual inline Concept*					create() const { return new T; }

private:
	bool									m_isPlaceholder;
};

}

template<class T>
inline bool Martta::Auxilliary<T>::dispatchKeyPress(Position const& _p, KeyEvent const* _e) const
{
	if (_p.allowedToBeKind<T>() && T::keyPressedOnPosition(_p, _e))
		return true;
	foreach (Kind k, AuxilliaryRegistrar::get()->immediateDeriveds<T>())
		if (AuxilliaryRegistrar::get()->auxilliary(k.name())->dispatchKeyPress(_p, _e))
			return true;
	return false;
}
