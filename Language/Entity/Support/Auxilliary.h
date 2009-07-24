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

namespace Martta
{

template<class T> struct GetCount
{
	typedef typename T::template AltSuperCount<> X;
	enum { Value = X::count };
};

template<class T>
class Auxilliary: public AuxilliaryFace
{
public:
	Auxilliary(char const* _name, bool _isPlaceholder): m_name(_name), m_isPlaceholder(_isPlaceholder) { AuxilliaryRegistrar::get()->registerAuxilliary(this); }
	virtual bool						isInterface() const { return false; }
	virtual bool						isPlaceholder() const { return m_isPlaceholder; }
	virtual char const*					name() const { return m_name; }
	virtual bool						dispatchKeyPress(Position const& _p, KeyEvent const* _e) const;
	virtual void						initialise() const { T::initialiseClass(); }
	virtual void						finalise() const { T::finaliseClass(); }
	virtual AuxilliaryFace const*		superAuxilliary() const { return T::Super::staticAuxilliary(); }
	virtual AuxilliaryFace const*		interfaceAuxilliary(int _i) const { return T::template ASHelper<GetCount<T>::Value>::altSupers()[_i]; }
	virtual int							interfaceAuxilliaryCount() const { return GetCount<T>::Value; }
	virtual Entity*						create() const { return new T; }
	virtual void const*					offset(Entity const* _e) const { return reinterpret_cast<void const*>(static_cast<T const*>(_e)); }

private:
	char const*							m_name;
	bool								m_isPlaceholder;
};

template<class T>
class InterfaceAuxilliary: public AuxilliaryFace
{
public:
	InterfaceAuxilliary(char const* _name): m_name(_name) { AuxilliaryRegistrar::get()->registerAuxilliary(this); }
	virtual bool						isInterface() const { return true; }
	virtual bool						isPlaceholder() const { return false; }	// Undefined.
	virtual char const*					name() const { return m_name; }
	virtual bool						dispatchKeyPress(Position const&, KeyEvent const*) const { return false; }
	virtual void						initialise() const {}
	virtual void						finalise() const {}
	virtual AuxilliaryFace const*		superAuxilliary() const { return 0; }
	virtual AuxilliaryFace const*		interfaceAuxilliary(int _i) const { return T::template ASHelper<GetCount<T>::Value>::altSupers()[_i]; }
	virtual int							interfaceAuxilliaryCount() const { return GetCount<T>::Value; }
	virtual Entity*						create() const { return 0; }
	virtual void const*					offset(Entity const*) const { return 0; }
private:
	char const*							m_name;
};

}

template<class T>
bool Martta::Auxilliary<T>::dispatchKeyPress(Position const& _p, KeyEvent const* _e) const
{
	if (_p.allowedToBeKind<T>() && T::keyPressedOnPosition(_p, _e))
		return true;
	foreach (Kind k, AuxilliaryRegistrar::get()->immediateDeriveds<T>())
		if (AuxilliaryRegistrar::get()->auxilliary(k.name())->dispatchKeyPress(_p, _e))
			return true;
	return false;
}
