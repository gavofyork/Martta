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

#include "InsertionPoint.h"
#include "AuxilliaryFace.h"
#include "AuxilliaryRegistrar.h"

namespace Martta
{

template<class T>
class Auxilliary: public AuxilliaryFace
{
public:
	Auxilliary(char const* _name): m_name(_name) { AuxilliaryRegistrar::get()->registerAuxilliary(this); }
	virtual char const*					name() const { return m_name; }
	virtual AuxilliaryFace const*		superAuxilliary() const { return T::Super::staticAuxilliary(); }
	virtual Entity*						create() const { return new T; }
	virtual bool						dispatchKeyPress(InsertionPoint const& _p, EntityKeyEvent const* _e) const;
	virtual void						initialise() const { T::initialiseClass(); }
	virtual void						finalise() const { T::finaliseClass(); }
	
private:
	char const*							m_name;
};

}

template<class T>
bool Martta::Auxilliary<T>::dispatchKeyPress(InsertionPoint const& _p, EntityKeyEvent const* _e) const
{
	if (_p.allowedToBeKind<T>() && T::keyPressedOnInsertionPoint(_p, _e)) return true;
	foreach (Kind k, AuxilliaryRegistrar::get()->immediateDeriveds<T>())
		if (AuxilliaryRegistrar::get()->auxilliary(k.name())->dispatchKeyPress(_p, _e))
			return true;
	return false;
}
