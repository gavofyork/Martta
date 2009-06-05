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

namespace Martta
{

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
	static AuxilliaryFace const*		staticAuxilliary(); \
	static Kind							staticKind; \
	inline virtual Kind					kind() const { return staticKind; } \
	template<int i, int d = 0>			struct AltSuper { typedef Nothing TheType; }; \
	template<int m = -1, typename T = void>	struct AltSuperCount { static const int count = AltSuperCount<m + 1, typename AltSuper<m + 1>::TheType>::count; }; \
	template<int m>						struct AltSuperCount<m, Nothing> { static const int count = m; }; \
	template<int m, int d = 0>			struct ASHelper { static AuxilliaryFace const** altSupers() { static AuxilliaryFace const* r[m]; r[m - 1] = AltSuper<m - 1>::TheType::staticAuxilliary(); memcpy(r, ASHelper<m - 1>::altSupers(), (m - 1) * sizeof(AuxilliaryFace const*)); return r; } }; \
	template<int d>						struct ASHelper<0, d> { static AuxilliaryFace const** altSupers() { static AuxilliaryFace const* r[0]; return r; } }; \
	template<int m, typename T>			struct ASTHelper { typedef typename AltSuper<m - 1>::TheType H; static void const* altSupers(T const* _this, Kind _k) { /*qDebug() << "[" << T::staticKind.name() << "] Searching on " << m << "-1th entry: " << H::staticKind.name() << " for " << _k.name();*/ return H::staticKind.isKind(_k) ? tryCast<H const*>(_this)->tryInterface(_k) : ASTHelper<m - 1, T>::altSupers(_this, _k); } }; \
	template<typename T>				struct ASTHelper<0, T> { static void const* altSupers(T const*, Kind) { /*qDebug() << "[" << T::staticKind.name() << "] Unable to find."; */return 0; } }; \
	virtual void const*					toInterface(Kind _k) const { return tryInterface(_k); } \
	void const*							tryInterface(Kind _k) const;

#define MARTTA_INHERITS(S, i) \
	template<int d>						struct AltSuper<i, d> { typedef S TheType; };

#define MARTTA_INTERFACE \
	public: \
	static const bool IsInterface = true; \
	static const bool IsObject = false; \
	static const bool IsPlaceholder = false; \
	virtual Entity const* self() const = 0; \
	virtual Entity* self() = 0; \
	template<class T> inline T*			asKind() { return this && self() ? self()->asKind<T>() : 0; } \
	template<class T> inline T const*	asKind() const { return this && self() ? self()->asKind<T>() : 0; } \
	template<class T> inline bool		isKind() const { return this && self() ? self()->isKind<T>() : false; } \
	MARTTA_BASIC

#define MARTTA_COMMON(S) \
	public: \
	virtual Entity const*				self() const { return this; } \
	virtual Entity*						self() { return this; } \
	template<class T> inline T*			asKind() { return Entity::asKind<T>(); } \
	template<class T> inline T const*	asKind() const { return Entity::asKind<T>(); } \
	template<class T> inline bool		isKind() const { return Entity::isKind<T>(); } \
	typedef S Super; \
	MARTTA_BASIC

#define MARTTA_OBJECT(S) \
	MARTTA_COMMON(S) \
	public: \
	static const bool IsInterface = false; \
	static const bool IsObject = true; \
	static const bool IsPlaceholder = false; \
	inline virtual bool					isPlaceholder() const { return false; }

#define MARTTA_PLACEHOLDER(S) \
	MARTTA_COMMON(S) \
	public: \
	static const bool IsInterface = false; \
	static const bool IsObject = false; \
	static const bool IsPlaceholder = true; \
	inline virtual bool					isPlaceholder() const { return true; }

#define MARTTA_CPP_BASIC(E) \
	static AuxilliaryFace const* s_auxilliary_##E = 0; \
	Kind E::staticKind = Kind(E::staticAuxilliary());

#define MARTTA_OBJECT_CPP(E) \
	MARTTA_CPP_BASIC(E) \
	AuxilliaryFace const* E::staticAuxilliary() { if (!s_auxilliary_##E) s_auxilliary_##E = new Auxilliary<E>("Martta::" #E); return s_auxilliary_##E; } \
	void const* E::tryInterface(Kind _k) const { /*qDebug() << "tryInterface(Object): " << E::staticKind.name() << ", searching " << _k.name();*/ M_ASSERT(this); if (_k == staticKind) { /*qDebug() << "Shouldn't happen - object matched but wanted interface: Want " << _k.name() << ", got " << E::staticKind.name() << "!";*/ return (void const*)this; } /*qDebug() << "Trying ASTHelper " << GetCount<E>::Value << " interfaces";*/ if (void const* r = ASTHelper<GetCount<E>::Value, E>::altSupers(this, _k)) return r; /*qDebug() << "Moving to Super (" << Super::staticKind.name() << ")...";*/ return Super::tryInterface(_k); }

#define MARTTA_INTERFACE_CPP(E) \
	MARTTA_CPP_BASIC(E) \
	AuxilliaryFace const* E::staticAuxilliary() { if (!s_auxilliary_##E) s_auxilliary_##E = new InterfaceAuxilliary<E>("Martta::" #E); return s_auxilliary_##E; } \
	void const* E::tryInterface(Kind _k) const { /*qDebug() << "tryInterface: " << E::staticKind.name() << ", searching " << _k.name();*/ M_ASSERT(this); if (_k == staticKind) { /*qDebug() << "Matched: Want " << _k.name() << ", got " << E::staticKind.name() << "!"; */return (void const*)this; } /*qDebug() << "Trying ASTHelper " << GetCount<E>::Value << " interfaces";*/ if (void const* r = ASTHelper<GetCount<E>::Value, E>::altSupers(this, _k)) return r; /*qDebug() << "Failed";*/ return 0; }

#define public_interface virtual public