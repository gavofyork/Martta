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

#ifndef _MSC_VER
#define inline MS_EXPORT inline
#endif

namespace MarttaSupport
{

class Rgb;
class TextStream;
class String;
template<class> class List;
template<class, class, int> class Hash;
template<class, class, int> class MultiHash;
typedef List<String> StringList;

#if defined(DEBUG)

void assertFailed(int, char const*, char const*, char const*, char const*);
#if defined(__GNUC__)
#define ASSERT(X, R) if ((X)) {} else MarttaSupport::assertFailed(__LINE__, __FILE__, __PRETTY_FUNCTION__, #X, R)
#else
#define ASSERT(X, R) if ((X)) {} else MarttaSupport::assertFailed(__LINE__, __FILE__, __FUNCTION__, #X, R)
#endif
#define ASSERT_EQ(X, Y) //if ((X)) {} else assertFailedEq(__LINE__, __FUNCTION__, #X, X, #Y, Y)
#define ASSERT_NE(X, Y) //if ((X)) {} else assertFailedNe(__LINE__, __FUNCTION__, #X, X, #Y, Y)
#define STATIC_ASSERT(X)

#else

#define ASSERT(X, R) (void)0

#endif

#define ASSERT_NR(X) ASSERT(X, "");

template<typename T>
inline T max(T const& _a, T const& _b) { return (_a < _b) ? _b : _a; }

typedef unsigned char uchar;
typedef unsigned short ushort;
namespace t
{
typedef unsigned int uint;
}
typedef unsigned long ulong;

template<typename T>
inline T min(T const& _a, T const& _b) { return (_b < _a) ? _b : _a; }

template<typename T>
inline T sign(T const& _x) { return _x < 0 ? -1 : _x > 0 ? 1 : 0; }

//#define inline MS_EXPORT inline
template<typename T>
inline void swap(T& _a, T& _b) { T t(_a); _a = _b; _b = t; }

template<typename T>
inline T abs(T _a) { return _a < 0 ? -_a : _a; }

template<typename T>
inline bool isPOT(T _a) { return !(_a & (_a - 1)); }

template<typename T>
inline T nextHigher(T _k)
{
	_k--;
	for (t::uint i = 1; i < sizeof(T) * 8; i = i * 2)
		_k = _k | _k >> i;
	return _k + 1;
}

inline t::uint floorLog2(t::uint _a) 
{
	t::uint rett;
#ifdef _MSC_VER
	_asm
	{
			mov	eax, _a
			bsr	eax, eax
			mov	rett, eax
	}	
#else
	asm ("bsr %1, %0;" :"=r"(rett) :"r"(_a));
#endif
	return rett;
}

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


// Taken from Qt 4.4, Copyright Nokia. Used under licence (GPL).
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
/* make use of typeof-extension */
template <typename T>
class MForeachContainer {
public:
    inline MForeachContainer(const T& t) : c(t), brk(0), i(c.begin()), e(c.end()) { }
    const T c;
    int brk;
    typename T::const_iterator i, e;
};

#define M_FOREACH(variable, container)                                \
for (MarttaSupport::MForeachContainer<__typeof__(container)> _container_(container); \
     !_container_.brk && _container_.i != _container_.e;              \
     __extension__  ({ ++_container_.brk; ++_container_.i; }))                       \
    for (variable = *_container_.i;; __extension__ ({--_container_.brk; break;}))

#else

struct MForeachContainerBase {};

template <typename T>
class MForeachContainer : public MForeachContainerBase {
public:
    inline MForeachContainer(const T& t): c(t), brk(0), i(c.begin()), e(c.end()){};
    const T c;
    mutable int brk;
    mutable typename T::const_iterator i, e;
    inline bool condition() const { return (!brk++ && i != e); }
};

template <typename T> inline T *mForeachPointer(const T &) { return 0; }

template <typename T> inline MForeachContainer<T> mForeachContainerNew(const T& t)
{ return MForeachContainer<T>(t); }

template <typename T>
inline const MForeachContainer<T> *mForeachContainer(const MForeachContainerBase *base, const T *)
{ return static_cast<const MForeachContainer<T> *>(base); }

#if (defined(_MSC_VER) && _MSC_VER < 1300 && !defined(__INTEL_COMPILER)) || defined(__sgi)
/*
   Proper for-scoping in VC++6 and MIPSpro CC
*/
#  define M_FOREACH(variable,container)                                                             \
    if(0){}else                                                                                     \
    for (const MarttaSupport::MForeachContainerBase &_container_ = mForeachContainerNew(container);                \
         mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->condition();       \
         ++mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->i)               \
        for (variable = *mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->i; \
             mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->brk;           \
             --mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->brk)

#else
#  define M_FOREACH(variable, container) \
    for (const MarttaSupport::MForeachContainerBase &_container_ = mForeachContainerNew(container); \
         mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->condition();       \
         ++mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->i)               \
        for (variable = *mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->i; \
             mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->brk;           \
             --mForeachContainer(&_container_, true ? 0 : mForeachPointer(container))->brk)
#endif

#endif

#ifndef foreach
#define foreach M_FOREACH
#endif

}
