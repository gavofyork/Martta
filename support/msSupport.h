/**
 * @filename msSupport.h
 * @author Gavin Wood
 * @date 03/06/2008
 * @copyright quid pro code, Ltd.
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MarttaSupport License version 1.0
 *
 * The contents of this file are subject to the MarttaSupport License
 * version 1.0 (the "License"); you may not use this file except in 
 * compliance with the License. You should have received a copy of the 
 * MarttaSupport License "COPYING.MarttaSupport" along with Martta; if not
 * you may obtain a copy of the License at http://quidprocode.co.uk/Martta/
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

#define MS_EXPORT __attribute__((visibility("default")))

#define privateinline inline
#define inline MS_EXPORT inline

namespace MarttaSupport
{
	
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

template<typename T>
inline T nextHigher(T _k)
{
	_k--;
	for (uint i = 1; i < sizeof(T) * 8; i = i * 2)
		_k = _k | _k >> i;
	return _k + 1;
}

template<typename T>
inline T max(T _a, T _b) { return (_a < _b) ? _b : _a; }

template<typename T>
inline T min(T _a, T _b) { return (_a > _b) ? _b : _a; }

template<typename T>
inline void swap(T& _a, T& _b) { T t(_a); _a = _b; _b = t; }

template<typename T>
inline T abs(T _a) { return _a < 0 ? -_a : _a; }

template<typename T>
inline bool isPOT(T _a) { return !(_a & (_a - 1)); }

inline uint floorLog2(uint _a) { uint ret; asm ("bsr %1, %0;" :"=r"(ret) :"r"(_a)); return ret; }

#define ASSERT(X)

struct MethodExistsTrue { enum { c_val = 1 }; };
struct MethodExistsFalse { enum { c_val = 0 }; };
template<class T, void (T::*)()> struct MethodExistsStruct {};
template<class T> MethodExistsFalse& IfMethodExists(...) { return *(MethodExistsFalse*)0; }

// Must be used in global namespace
#define MS_TEST_METHOD_EXISTANCE(M) \
namespace MarttaSupport \
{ \
template<class T> MethodExistsTrue& IfMethodExists(MethodExistsStruct<T, &T::M>*) { return *(MethodExistsTrue*)0; } \
template<class T> inline void M(T& _a, MethodExistsTrue&) { _a.M(); } \
template<class T> inline void M(T&, MethodExistsFalse&){} \
template<class T> inline void M(T& _a) { M(_a, IfMethodExists<T>(0)); } \
}


}
