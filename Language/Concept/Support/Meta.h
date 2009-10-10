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

#include "Auxilliary.h"
#include "Kind.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

int M_API_Concept registerName(int _n, char const* _class, char const* _name);

class Concept;
class M_API_Concept Nothing { public: static AuxilliaryFace const* staticAuxilliary() { return 0; } void const* tryInterface(Kind) const { return 0; } };

namespace tryCastPrivate
{
template<class T, class F> struct isCompatible { static char testCompat(...); static int testCompat(T); static const bool value = sizeof(testCompat((F)0)) - 1; };
template<class T, class F, bool B> struct XL { static T doCast(F) { return 0; } };
template<class T, class F> struct XL<T, F, true> { static T doCast(F _f) { return static_cast<T>(_f); } };
}
template<class T, class F> T tryCast(F _f) { return tryCastPrivate::XL<T, F, tryCastPrivate::isCompatible<F, T>::value || tryCastPrivate::isCompatible<T, F>::value>::doCast(_f); }

}

#define MARTTA_BASIC \
public: \
	template<class T> friend struct Martta::GetCount; \
	template<class T> friend class Martta::Auxilliary; \
	template<class T> friend class Martta::InterfaceAuxilliary; \
	inline virtual Kind					kind() const { return this ? staticKind : Kind(Nothing::staticAuxilliary()); } \
	static Kind							staticKind; \
	static AuxilliaryFace const*		staticAuxilliary(); \
	void const*							tryInterface(Kind _k) const; \
private: \
	template<int i, int d = 0>			struct AltSuper { typedef Nothing TheType; }; \
	template<int m = -1, typename T = void>	struct AltSuperCount { static const int count = AltSuperCount<m + 1, typename AltSuper<m + 1>::TheType>::count; }; \
	template<int m>						struct AltSuperCount<m, Nothing> { static const int count = m; }; \
	template<int m, int d = 0>			struct ASHelper { static AuxilliaryFace const** altSupers() { static AuxilliaryFace const* r[m]; r[m - 1] = AltSuper<m - 1>::TheType::staticAuxilliary(); memcpy(r, ASHelper<m - 1>::altSupers(), (m - 1) * sizeof(AuxilliaryFace const*)); return r; } }; \
	template<int d>						struct ASHelper<0, d> { static AuxilliaryFace const** altSupers() { static AuxilliaryFace const* r[1]; return r; } }; \
	template<int m, typename T>			struct ASTHelper { typedef typename AltSuper<m - 1>::TheType H; static void const* altSupers(T const* _this, Kind _k) { /*mDebug() << "[" << T::staticKind.name() << "] Searching on " << m << "-1th entry: " << H::staticKind.name() << " for " << _k.name();*/ return H::staticKind.isKind(_k) ? tryCast<H const*>(_this)->tryInterface(_k) : ASTHelper<m - 1, T>::altSupers(_this, _k); } }; \
	template<typename T>				struct ASTHelper<0, T> { static void const* altSupers(T const*, Kind) { /*mDebug() << "[" << T::staticKind.name() << "] Unable to find."; */return 0; } }; \
	virtual void const*					toInterface(Kind _k) const { return tryInterface(_k); }

#define MARTTA_ALSO_INHERITS(S, i) \
private: \
	template<int d>						struct AltSuper<i + 1, d> { typedef S TheType; };

#define MARTTA_NOTION(S) \
public: \
	virtual void const*					tryKindBlind(Kind _k) const = 0; \
	template<class T> inline T*			asKind() { if (!this) return 0; T* ret = const_cast<T*>(reinterpret_cast<T const*>(tryKindBlind(Kind::of<T>()))); AssertNR(ret); return ret; } \
	template<class T> inline T const*	asKind() const { if (!this) return 0; T const* ret = reinterpret_cast<T const*>(tryKindBlind(Kind::of<T>())); AssertNR(ret); return ret; } \
	template<class T> inline T*			tryKind() { if (!this) return 0; return const_cast<T*>(reinterpret_cast<T const*>(tryKindBlind(Kind::of<T>()))); } \
	template<class T> inline T const*	tryKind() const { if (!this) return 0; return reinterpret_cast<T const*>(tryKindBlind(Kind::of<T>())); } \
	template<class T> inline bool		isKind() const { if (!this) return false; return tryKindBlind(Kind::of<T>()); } \
	MARTTA_BASIC \
	MARTTA_ALSO_INHERITS(S, -1)

#define MARTTA_COMMON(S) \
public: \
	typedef S Super; \
	MARTTA_BASIC \
	MARTTA_ALSO_INHERITS(S, -1)

#define MARTTA_INITIALISED_OBJECT(S) \
public: \
	Concept::asKind; \
	Concept::tryKind; \
	Concept::isKind; \
	virtual void const*					tryKindBlind(Kind _k) const { if (_k == staticKind) return reinterpret_cast<void const*>(this); return Super::tryKindBlind(_k); } \
	MARTTA_COMMON(S)

#define MARTTA_INITIALISED_PLACEHOLDER(S) \
public: \
	Concept::asKind; \
	Concept::tryKind; \
	Concept::isKind; \
	virtual void const*					tryKindBlind(Kind _k) const { if (_k == staticKind) return reinterpret_cast<void const*>(this); return Super::tryKindBlind(_k); } \
	MARTTA_COMMON(S)

#define MARTTA_PROPER(S) \
public: \
	static inline void					initialiseClass() {} \
	static inline void					finaliseClass() {} \
	MARTTA_INITIALISED_OBJECT(S)

#define MARTTA_PLACEHOLDER(S) \
public: \
	static inline void					initialiseClass() {} \
	static inline void					finaliseClass() {} \
	MARTTA_INITIALISED_PLACEHOLDER(S)

#define MARTTA_NAMED(X) static const int X;
#define MARTTA_NAMED_CPP(E, X) const int E::X = registerName(-((int)(((unsigned)&X) / sizeof(X))), #E "::" #X)
#define MARTTA_NAME_ALIAS(X) static const int& X;
#define MARTTA_NAME_ALIAS_CPP(E, X, Y) const int& E::X = E::Y

#define MARTTA_CPP_BASIC(E) \
	static AuxilliaryFace const* s_auxilliary_##E = 0;
#define MARTTA_CPP_BASIC_END(E) \
	Kind E::staticKind = Kind(E::staticAuxilliary());

#define MARTTA_OB_OR_PH_CPP(E, PH) \
	MARTTA_CPP_BASIC(E) \
	AuxilliaryFace const* E::staticAuxilliary() { if (!s_auxilliary_##E) s_auxilliary_##E = new Auxilliary<E>("Martta::" #E, PH); return s_auxilliary_##E; } \
	void const* E::tryInterface(Kind _k) const { AssertNR(this); if (_k == staticKind) { return (void const*)this; } if (void const* r = ASTHelper<GetCount<E>::Value, E>::altSupers(this, _k)) return r; return 0; } \
	MARTTA_CPP_BASIC_END(E)
//	void const* E::tryInterface(Kind _k) const { AssertNR(this); if (_k == staticKind) { return (void const*)this; } if (void const* r = ASTHelper<GetCount<E>::Value, E>::altSupers(this, _k)) return r; return Super::tryInterface(_k); }

#define MARTTA_PLACEHOLDER_CPP(E) MARTTA_OB_OR_PH_CPP(E, true)
#define MARTTA_PROPER_CPP(E) MARTTA_OB_OR_PH_CPP(E, false)

#define MARTTA_NOTION_CPP(E) \
	MARTTA_CPP_BASIC(E) \
	AuxilliaryFace const* E::staticAuxilliary() { if (!s_auxilliary_##E) s_auxilliary_##E = new InterfaceAuxilliary<E>("Martta::" #E); return s_auxilliary_##E; } \
	void const* E::tryInterface(Kind _k) const { /*mDebug() << "tryInterface: " << E::staticKind.name() << ", searching " << _k.name();*/ AssertNR(this); if (_k == staticKind) { /*mDebug() << "Matched: Want " << _k.name() << ", got " << E::staticKind.name() << "!"; */return (void const*)this; } /*mDebug() << "Trying ASTHelper " << GetCount<E>::Value << " interfaces";*/ if (void const* r = ASTHelper<GetCount<E>::Value, E>::altSupers(this, _k)) return r; /*mDebug() << "Failed";*/ return 0; } \
	MARTTA_CPP_BASIC_END(E)

#define public_interface virtual public
#define public_super virtual public
#define public_super_interface virtual public
