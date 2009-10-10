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

#include "WebInformer.h"
#include "Type.h"

#ifndef M_API_TypeConcept
#define M_API_TypeConcept M_OUTAPI
#endif

namespace Martta
{

class M_API_TypeConcept TypeNamer: public_super_interface WebInformer
{
	MARTTA_NOTION(WebInformer)

public:
	virtual ~TypeNamer() {}

	// Accessor methods.
	/// The effective type of this entity. This takes into account the parent of the entity,
	/// and thus implicit casting may occur.
	virtual Type						apparentType() const { return type(); }
	/// The actual type it thinks it is. This is (typically) independent of the parent and
	/// the type has not been implicitly casted according to the "hole" it is trying to fit.
	virtual Type						type() const { return Type(); }

	Types								ourAllowedTypes() const;
	Types								ourDeniedTypes() const;

	virtual String						informationHtml() const;
};

template<class T> List<T*> filterTypeds(Type const& _t, List<T*> const& _l);
template<class T> List<T*> filterTypedsInv(Type const& _t, List<T*> const& _l);

}

template<class T>
List<T*> Martta::filterTypeds(Type const& _t, List<T*> const& _l)
{
	List<T*> ret;

	foreach (T* i, _l)
	{
//		mDebug() << "fT:" << i->type()->code() << "(" << _t->code() << ")";
		if (i->type().isSimilarTo(_t, TypeConcept::Physical) && i->type().isSimilarTo(_t, TypeConcept::Convertible))
		{
//			mDebug() << "OK";
			ret << i;
		}
	}
	return ret;
}

template<class T>
List<T*> Martta::filterTypedsInv(Type const& _t, List<T*> const& _l)
{
	List<T*> ret;

	foreach (T* i, _l)
		if (!(i->type().isSimilarTo(_t, TypeConcept::Physical) && i->type().isSimilarTo(_t, TypeConcept::Convertible)))
			ret << i;
	return ret;
}
