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

#include "ConceptSupport.h"
#include "Auxilliary.h"
#include "Kind.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

typedef char const* RawConstMemory;

int M_API_Concept registerName(int _n, char const* _class, char const* _name);

class M_CLASS M_API_Concept Nothing { public: static AuxilliaryFace const* staticAuxilliary() { return 0; } ::Martta::RawConstMemory tryInterface(Kind) const { return 0; } };

namespace tryCastPrivate
{
template<class T, class F> struct isCompatible { static char testCompat(...); static int testCompat(T); static const bool value = sizeof(testCompat((F)0)) - 1; };
template<class T, class F, bool B> struct XL { static T doCast(F) { return 0; } };
template<class T, class F> struct XL<T, F, true> { static T doCast(F _f) { return static_cast<T>(_f); } };
}
template<class T, class F> T tryCast(F _f) { return tryCastPrivate::XL<T, F, tryCastPrivate::isCompatible<F, T>::value || tryCastPrivate::isCompatible<T, F>::value>::doCast(_f); }

}

#define MARTTA_ALSO_INHERITS(S, i) \
private: \
	template<int d>						struct AltSuper<i + 1, d> { typedef S TheType; };

#define MARTTA_COMMON(S) \
public: \
	template<class T> friend struct Martta::GetCount; \
	template<class T> friend class Martta::Auxilliary; \
	template<class T> friend class Martta::NotionAuxilliary; \
	inline virtual Kind					kind() const { return this ? staticKind : Kind(Nothing::staticAuxilliary()); } \
	static Kind							staticKind; \
	static AuxilliaryFace const*		staticAuxilliary(); \
	::Martta::RawConstMemory			tryInterface(Kind _k) const; \
private: \
	template<int i, int d = 0>			struct AltSuper { typedef Nothing TheType; }; \
	template<int m = -1, typename T = void>	struct AltSuperCount { static const int count = AltSuperCount<m + 1, typename AltSuper<m + 1>::TheType>::count; }; \
	template<int m>						struct AltSuperCount<m, Nothing> { static const int count = m; }; \
	template<int m, int d = 0>			struct ASHelper { static AuxilliaryFace const** altSupers() { static AuxilliaryFace const* r[m]; r[m - 1] = AltSuper<m - 1>::TheType::staticAuxilliary(); memcpy(r, ASHelper<m - 1>::altSupers(), (m - 1) * sizeof(AuxilliaryFace const*)); return r; } }; \
	template<int d>						struct ASHelper<0, d> { static AuxilliaryFace const** altSupers() { static AuxilliaryFace const* r[1]; return r; } }; \
	template<int m, typename T>			struct ASTHelper { typedef typename AltSuper<m - 1>::TheType H; static ::Martta::RawConstMemory altSupers(T const* _this, Kind _k) { /*mDebug() << "[" << T::staticKind.name() << "] Searching on " << m << "-1th entry: " << H::staticKind.name() << " for " << _k.name();*/ return H::staticKind.isKind(_k) ? tryCast<H const*>(_this)->tryInterface(_k) : ASTHelper<m - 1, T>::altSupers(_this, _k); } }; \
	template<typename T>				struct ASTHelper<0, T> { static ::Martta::RawConstMemory altSupers(T const*, Kind) { /*mDebug() << "[" << T::staticKind.name() << "] Unable to find."; */return 0; } }; \
	virtual ::Martta::RawConstMemory	toInterface(Kind _k) const { return tryInterface(_k); } \
	MARTTA_ALSO_INHERITS(S, -1)

#define MARTTA_CONCEPT(S) \
public: \
	template<class T> inline T*			asKind() { if (!this) return 0; return Concept::asKind<T>(); } \
	template<class T> inline T const*	asKind() const { if (!this) return 0; return Concept::asKind<T>(); } \
	template<class T> inline T*			tryKind() { if (!this) return 0; return Concept::tryKind<T>(); } \
	template<class T> inline T const*	tryKind() const { if (!this) return 0; return Concept::tryKind<T>(); } \
	template<class T> inline bool		isKind() const { if (!this) return 0; return Concept::isKind<T>(); } \
	MARTTA_COMMON(S)

#define MARTTA_NOTION(S) \
	MARTTA_CONCEPT(S) \
protected: \
	virtual ::Martta::RawConstMemory					tryKindBlind(Kind _k) const { if (_k == staticKind) return reinterpret_cast< ::Martta::RawConstMemory>(this); return 0; }

#define MARTTA_INITIALISED(S) \
public: \
	typedef S Super; \
	MARTTA_CONCEPT(S) \
protected: \
	virtual ::Martta::RawConstMemory					tryKindBlind(Kind _k) const { if (_k == staticKind) return reinterpret_cast< ::Martta::RawConstMemory>(this); return Super::tryKindBlind(_k); } \

#define MARTTA_INITIALISED_PROPER(S) MARTTA_INITIALISED(S)
#define MARTTA_INITIALISED_PLACEHOLDER(S) MARTTA_INITIALISED(S)

#define MARTTA_PROPER(S) \
public: \
	static inline void					initialiseClass() {} \
	static inline void					finaliseClass() {} \
	MARTTA_INITIALISED_PROPER(S)

#define MARTTA_PLACEHOLDER(S) \
public: \
	static inline void					initialiseClass() {} \
	static inline void					finaliseClass() {} \
	MARTTA_INITIALISED_PLACEHOLDER(S)

#define MARTTA_NAMED(X) static const int X;
#define MARTTA_NAMED_CPP(E, X) const int E::X = registerName(-((int) ((((unsigned long)&X) / sizeof(int)) % ((1l << (sizeof(int) * 8 - 1)) - 1)) ), #E "::" #X)
#define MARTTA_NAME_ALIAS(X) static const int& X;
#define MARTTA_NAME_ALIAS_CPP(E, X, Y) const int& E::X = E::Y

#define MARTTA_OBJECT_CPP(E, AUX) \
	static AuxilliaryFace const* s_auxilliary_##E = 0; \
	AuxilliaryFace const* E::staticAuxilliary() { if (!s_auxilliary_##E) s_auxilliary_##E = new AUX; return s_auxilliary_##E; } \
	::Martta::RawConstMemory E::tryInterface(Kind _k) const { AssertNR(this); if (_k == staticKind) { return reinterpret_cast< ::Martta::RawConstMemory>(this); } if (::Martta::RawConstMemory r = ASTHelper<GetCount<E>::Value, E>::altSupers(this, _k)) return r; return 0; } \
	Kind E::staticKind = Kind(E::staticAuxilliary());

#define MARTTA_NOTION_CPP(E) MARTTA_OBJECT_CPP(E, NotionAuxilliary<E>("Martta::" #E))
#define MARTTA_PLACEHOLDER_CPP(E) MARTTA_OBJECT_CPP(E, Auxilliary<E>("Martta::" #E, true))
#define MARTTA_PROPER_CPP(E) MARTTA_OBJECT_CPP(E, Auxilliary<E>("Martta::" #E, false))

#define public_interface virtual public
#define public_super virtual public
#define public_super_interface virtual public
