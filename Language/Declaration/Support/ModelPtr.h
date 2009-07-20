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

#include "Identifiable.h"
#include "ModelPtrFace.h"
#include "Entity.h"

namespace Martta
{

class Entity;

template<class T>
class ModelPtr: public ModelPtrFace
{
public:
	inline ModelPtr(T* _t = 0): ModelPtrFace(_t) {}
	inline ModelPtr(ModelPtr const& _c): ModelPtrFace(_c) {}

	inline ModelPtr& operator=(ModelPtr const& _c) { ModelPtrFace::operator=(_c); return *this; }
	inline ModelPtr& operator=(T* _t) { set(_t); return *this; }
	inline bool operator==(T const* _t) const { return this->operator T*() == _t; }
	inline bool operator!=(T const* _t) const { return !operator==(_t); }
	inline bool operator==(T* _t) const { return this->operator T*() == _t; }
	inline bool operator!=(T* _t) const { return !operator==(_t); }

	inline operator T*() const
	{
		Identifiable* r = const_cast<ModelPtr*>(this)->get();
		AssertNR(!r || r->isKind<T>());
		return r ? r->asKind<T>() : 0;
	}
	inline T& operator*() const { return *this->operator T*(); }
	inline T* operator->() const { return this->operator T*(); }
};

template<class T> inline bool operator==(T const* _t, ModelPtr<T> const& _f) { return _t == _f; }
template<class T> inline bool operator!=(T const* _t, ModelPtr<T> const& _f) { return _t != _f; }
template<class T> inline bool operator==(T* _t, ModelPtr<T> const& _f) { return _t == _f; }
template<class T> inline bool operator!=(T* _t, ModelPtr<T> const& _f) { return _t != _f; }

}
