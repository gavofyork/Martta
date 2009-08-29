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
#include <msString.h>
#include <msHash.h>
#include <msList.h>
using namespace MarttaSupport;

#ifndef M_API_Entity
#define M_API_Entity M_OUTAPI
#endif

namespace Martta
{

class Entity;
class EditDelegateFace;
class CodeScene;
class KeyEvent;
class Dier;
class Position;
template<class T, bool F> class SafePointer;

typedef Hash<String, String> ViewKeys;

template<class T, class F> inline List<T*> castEntities(List<F*> _f)
{
	List<T*> ret;
	foreach (F* i, _f)
		ret << i->template asKind<T>();
	return ret;
}

template<class T, class F> inline List<T*> filterEntities(List<F*> _f)
{
	List<T*> ret;
	foreach (F* i, _f)
		if (T* t = i->template tryKind<T>())
			ret << t;
	return ret;
}

template<class T, class F> inline List<F*> filterEntitiesInv(List<F*> _f)
{
	List<F*> ret;
	foreach (F* i, _f)
		if (!i->template isKind<T>())
			ret << i;
	return ret;
}

}
