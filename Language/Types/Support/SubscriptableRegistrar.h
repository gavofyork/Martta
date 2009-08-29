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

#include <msHash.h>
using namespace MarttaSupport;

#include "Kind.h"
#include "Type.h"

#ifndef M_API_Types
#define M_API_Types M_OUTAPI
#endif

namespace Martta
{

class SubscriptOperation;

class SubscriptableRegistrar
{
	friend class SubscriptOperation;
	
public:
	static SubscriptableRegistrar*			get() { return s_this ? s_this : (s_this = new SubscriptableRegistrar); }
	
	template<class T> void					registerKind() { m_acceptable.insert(T::staticKind, Type(T())); }
	template<class T> void					unregisterKind() { m_acceptable.remove(T::staticKind); }
	template<class T> void					registerType(Type const& _t) { m_acceptable.insert(T::staticKind, _t); }
	template<class T> void					unregisterType(Type const& _t) { m_acceptable.remove(T::staticKind, _t); }
	
private:
	MultiHash<Kind, Type>					m_acceptable;
	
	static SubscriptableRegistrar*			s_this;
};

}
