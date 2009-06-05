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

#include "DeclarationEntity.h"
#include "ModelPtrFace.h"

namespace Martta
{

template<class T>
class ModelPtr: public ModelPtrFace
{
public:
	inline ModelPtr(T* _t = 0): ModelPtrFace(_t) {}
	inline ModelPtr(QString const& _k, RootEntity* _r): ModelPtrFace(_k, _r) {}
	inline ModelPtr(ModelPtr const& _c): ModelPtrFace(_c) {}

	inline ModelPtr& operator=(ModelPtr const& _c) { ModelPtrFace::operator=(_c); return *this; }
	inline ModelPtr& operator=(T* _t) { set(_t); return *this; }

	inline operator T*() const
	{
		Referencable* r = const_cast<ModelPtr*>(this)->get();
		M_ASSERT(!const_cast<ModelPtr*>(this)->get() || r->isKind<T>());
		return r ? r->asKind<T>() : 0;
	}
	inline T& operator*() const { return *this->operator T*(); }
	inline T* operator->() const { return this->operator T*(); }
};

}
