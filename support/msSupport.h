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

#if defined(_MSC_VER)
#define M_EXPORT
#define M_NOEXPORT
#define M_INAPI __declspec( dllexport )
#define M_OUTAPI __declspec( dllimport )
#else
#define M_EXPORT __attribute__((visibility("default")))
#define M_NOEXPORT __attribute__((visibility("hidden")))
#define M_INAPI
#define M_OUTAPI
#endif

#define m_privateinline M_NOEXPORT inline
#define m_inline M_EXPORT inline

// Fairly horrible, but seems like Qt will clash with us otherwise.
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

namespace MarttaSupport
{

typedef void (*AssertionHandler)(int, char const*, char const*, char const*, char const*);
extern AssertionHandler s_alternativeAssertionHandler;

class Rgb;
class TextStream;
class String;
template<class> class List;
template<class, class, int> class Hash;
template<class, class, int> class MultiHash;
class StringList;

#if defined(DEBUG)

void assertFailed(int, char const*, char const*, char const*, char const*);
#if defined(__GNUC__)
#define Assert(X, R) if ((X)) {} else MarttaSupport::assertFailed(__LINE__, __FILE__, __PRETTY_FUNCTION__, #X, R)
#else
#define Assert(X, R) if ((X)) {} else MarttaSupport::assertFailed(__LINE__, __FILE__, __FUNCTION__, #X, R)
#endif
#if defined(__GNUC__)
#define AssertEq(X, Y, R) if ((X) == (Y)) {} else MarttaSupport::assertFailed(__LINE__, __FILE__, __PRETTY_FUNCTION__, String(#X " != " #Y " (%1 != %2)").arg(X).arg(Y).toCString(), R)
#else
#define AssertEq(X, Y, R) if ((X) == (Y)) {} else MarttaSupport::assertFailed(__LINE__, __FILE__, __FUNCTION__, String(#X " != " #Y " (%1 != %2)").arg(X).arg(Y).toCString(), R)
#endif
#define StaticAssert(X)

#else

#define Assert(X, R) (void)0
#define AssertEq(X, Y, R) (void)0
#define StaticAssert(X) (void)0

#endif

#define AssertNR(X) Assert(X, "");
#define AssertEqNR(X, Y) AssertEq(X, Y, "");

#if _MSC_VER
template<class T> m_inline T round(T const& x) { return ((x)>=0?(long)((x)+0.5):(long)((x)-0.5)); }
#endif

template<typename T>
m_inline T max(T const& _a, T const& _b) { return (_a < _b) ? _b : _a; }

template<typename T>
m_inline T min(T const& _a, T const& _b) { return (_b < _a) ? _b : _a; }

template<typename T>
m_inline T sign(T const& _x) { return _x < 0 ? -1 : _x > 0 ? 1 : 0; }

//#define m_inline M_EXPORT m_inline
template<typename T>
m_inline void swap(T& _a, T& _b) { T t(_a); _a = _b; _b = t; }

template<typename T>
m_inline T abs(T _a) { return _a < 0 ? -_a : _a; }

template<typename T>
m_inline bool isPOT(T _a) { return !(_a & (_a - 1)); }

template<typename T>
m_inline T nextHigher(T _k)
{
	_k--;
	for (uint i = 1; i < sizeof(T) * 8; i = i * 2)
		_k = _k | _k >> i;
	return _k + 1;
}

m_inline uint floorLog2(uint _a)
{
	uint rett;
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

template<typename T> class isSimple { public: enum { value = false }; typedef T* StorageType; };
template<typename T> class isSimple<T*> { public: enum { value = true }; typedef T* StorageType; };
#define M_DECLARE_SIMPLE_TYPE(T) template<> class isSimple<T> { public: enum { value = true }; typedef T StorageType; };
template<> class isSimple<bool> { public: enum { value = true }; typedef bool StorageType; };
template<> class isSimple<wchar_t> { public: enum { value = true }; typedef wchar_t StorageType; };
template<> class isSimple<signed char> { public: enum { value = true }; typedef signed char StorageType; };
template<> class isSimple<unsigned char> { public: enum { value = true }; typedef unsigned char StorageType; };
template<> class isSimple<signed short> { public: enum { value = true }; typedef signed short StorageType; };
template<> class isSimple<unsigned short> { public: enum { value = true }; typedef unsigned short StorageType; };
template<> class isSimple<signed int> { public: enum { value = true }; typedef signed int StorageType; };
template<> class isSimple<unsigned int> { public: enum { value = true }; typedef unsigned int StorageType; };
template<> class isSimple<signed long> { public: enum { value = true }; typedef signed long StorageType; };
template<> class isSimple<unsigned long> { public: enum { value = true }; typedef unsigned long StorageType; };
template<> class isSimple<signed long long> { public: enum { value = true }; typedef signed long long StorageType; };
template<> class isSimple<unsigned long long> { public: enum { value = true }; typedef unsigned long long StorageType; };
template<> class isSimple<float> { public: enum { value = true }; typedef float StorageType; };
template<> class isSimple<double> { public: enum { value = true }; typedef double StorageType; };
template<> class isSimple<long double> { public: enum { value = true }; typedef long double StorageType; };

// star; use instead of * operator on a item of T.
template<class T> m_inline T& refFor(typename isSimple<T>::StorageType& _d) { if (isSimple<T>::value) return *(T*)&_d; else return **(T**)&_d; }
template<class T> m_inline T const& refFor(typename isSimple<T>::StorageType const& _d) { if (isSimple<T>::value) return *(T const*)&_d; else return **(T const**)&_d; }
template<class T> m_inline bool refIsGood(typename isSimple<T>::StorageType const& _d) { return isSimple<T>::value || *(T**)&_d; }
template<class T> m_inline void initRef(typename isSimple<T>::StorageType& _d) { if (!isSimple<T>::value) *(T**)&_d = 0; }
template<class T> m_inline void finRef(typename isSimple<T>::StorageType& _d) { if (!isSimple<T>::value) delete &refFor<T>(_d); }
template<class T> m_inline void setRef(typename isSimple<T>::StorageType& _d, T const& _src)
{
	if (refIsGood<T>(_d))
		refFor<T>(_d) = _src;
	else
		*(T**)&_d = new T(_src);
}

#define M_TEST_METHOD_EXISTANCE(bar)\
enum bar##MethodExistsTrue {};\
enum bar##MethodExistsFalse {};\
template<class T, void (T::*)()> struct bar##MethodExistsStruct {};\
template<class T> bar##MethodExistsFalse& bar##IfMethodExists(...) { return *(bar##MethodExistsFalse*)0; }\
template<class T> bar##MethodExistsTrue& bar##IfMethodExists(bar##MethodExistsStruct<T, &T::bar>*) { return *(bar##MethodExistsTrue*)0; }\
template<class T> m_inline void __TEST_##bar(T& _a, bar##MethodExistsTrue&) { _a.bar(); }\
template<class T> m_inline void __TEST_##bar(T&, bar##MethodExistsFalse&){}\
template<class T> m_inline void bar(T& _a) { __TEST_##bar(_a, bar##IfMethodExists<T>(0)); }

#define M_TEST_METHOD_EXISTANCE_1(bar)\
enum bar##MethodExistsTrue1 {};\
enum bar##MethodExistsFalse1 {};\
template<class T, class A, void (T::*)(A)> struct bar##MethodExistsStruct1 {};\
template<class T, class A> bar##MethodExistsFalse1& bar##IfMethodExists1(...) { return *(bar##MethodExistsFalse1*)0; }\
template<class T, class A> bar##MethodExistsTrue1& bar##IfMethodExists1(bar##MethodExistsStruct1<T, A, &T::bar>*) { return *(bar##MethodExistsTrue1*)0; }\
template<class T, class A> m_inline void __TEST_1##bar(T& _a, A _arg, bar##MethodExistsTrue1&) { _a.bar(_arg); }\
template<class T, class A> m_inline void __TEST_1##bar(T&, A, bar##MethodExistsFalse1&){}\
template<class T, class A> m_inline void bar(T& _a, A _arg) { __TEST_1##bar(_a, _arg, bar##IfMethodExists1<T, A>(0)); }

// Taken from Qt 4.4, Copyright Nokia. Used under licence (GPL).
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
/* make use of typeof-extension */
template <typename T>
class MForeachContainer {
public:
	m_inline MForeachContainer(const T& t) : c(t), brk(0), i(c.begin()), e(c.end()) { }
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
	m_inline MForeachContainer(const T& t): c(t), brk(0), i(c.begin()), e(c.end()){};
	const T c;
	mutable int brk;
	mutable typename T::const_iterator i, e;
	m_inline bool condition() const { return (!brk++ && i != e); }
};

template <typename T> m_inline T *mForeachPointer(const T &) { return 0; }

template <typename T> m_inline MForeachContainer<T> mForeachContainerNew(const T& t)
{ return MForeachContainer<T>(t); }

template <typename T>
m_inline const MForeachContainer<T> *mForeachContainer(const MForeachContainerBase *base, const T *)
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
