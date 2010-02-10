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

#include "Type.h"
#include "PhysicalType.h"

#ifndef M_API_Types
#define M_API_Types M_OUTAPI
#endif

namespace Martta
{

// Arguments children.
// child(Returned) is return type.
// child(n) is argument n
// For ... functions, m_ellipsis is set.
class M_CLASS M_API_Types FunctionType: public_super PhysicalType
{


#define S PhysicalType
public:
	static inline void					initialiseClass() {}
	static inline void					finaliseClass() {}
public:
	typedef S Super;
public:
	template<class T> inline T*			asKind() { if (!this) return 0; return Concept::asKind<T>(); }
	template<class T> inline T const*	asKind() const { if (!this) return 0; return Concept::asKind<T>(); }
	template<class T> inline T*			tryKind() { if (!this) return 0; return Concept::tryKind<T>(); }
	template<class T> inline T const*	tryKind() const { if (!this) return 0; return Concept::tryKind<T>(); }
	template<class T> inline bool		isKind() const { if (!this) return 0; return Concept::isKind<T>(); }
public:
	template<class T> friend struct Martta::GetCount;
	template<class T> friend class Martta::Auxilliary;
	template<class T> friend class Martta::NotionAuxilliary;
	inline virtual Kind					kind() const { return this ? staticKind : Kind(Nothing::staticAuxilliary()); }
	static Kind							staticKind;
	static AuxilliaryFace const*		staticAuxilliary();
	::Martta::RawConstMemory			tryInterface(Kind _k) const;
private:
	template<int i, int d = 0>			struct AltSuper { typedef Nothing TheType; };
	template<int m = -1, typename T = void>	struct AltSuperCount { static const int count = AltSuperCount<m + 1, typename AltSuper<m + 1>::TheType>::count; };
	template<int m>						struct AltSuperCount<m, Nothing> { static const int count = m; };
	template<int m, int d = 0>			struct ASHelper { static AuxilliaryFace const** altSupers() { static AuxilliaryFace const* r[m]; r[m - 1] = AltSuper<m - 1>::TheType::staticAuxilliary(); memcpy(r, ASHelper<m - 1>::altSupers(), (m - 1) * sizeof(AuxilliaryFace const*)); return r; } };
	template<int d>						struct ASHelper<0, d> { static AuxilliaryFace const** altSupers() { static AuxilliaryFace const* r[1]; return r; } };
	template<int m, typename T>			struct ASTHelper { typedef typename AltSuper<m - 1>::TheType H; static ::Martta::RawConstMemory altSupers(T const* _this, Kind _k) { /*mDebug() << "[" << T::staticKind.name() << "] Searching on " << m << "-1th entry: " << H::staticKind.name() << " for " << _k.name();*/ return H::staticKind.isKind(_k) ? tryCast<H const*>(_this)->tryInterface(_k) : ASTHelper<m - 1, T>::altSupers(_this, _k); } };
	template<typename T>				struct ASTHelper<0, T> { static ::Martta::RawConstMemory altSupers(T const*, Kind) { /*mDebug() << "[" << T::staticKind.name() << "] Unable to find."; */return 0; } };
	virtual ::Martta::RawConstMemory	toInterface(Kind _k) const { return tryInterface(_k); }
private:
	template<int d>						struct AltSuper<0, d> { typedef S TheType; };
protected:
	virtual ::Martta::RawConstMemory					tryKindBlind(Kind _k) const { if (_k == staticKind) return reinterpret_cast< ::Martta::RawConstMemory>(this); return Super::tryKindBlind(_k); }
#undef S
//	MARTTA_PROPER(PhysicalType)

public:
	MARTTA_NAMED(Returned)

	FunctionType(bool _ellipsis = false, bool _wild = false): m_ellipsis(_ellipsis), m_wild(_wild) {}

	bool								ellipsis() const { return m_ellipsis; }
	virtual bool						isUltimatelyNull() const { return m_wild; }
	Type								returnType() const { return m_wild ? Type() : *childAs<TypeConcept>(Returned); }
	Type								argumentType(int _i) const { return !m_wild && childIs<TypeConcept>(_i) ? *childAs<TypeConcept>(_i) : Type(); }
	int									minimumArgCount() const { return m_wild ? 0 : cardinalChildCount(); }
	bool								hasArgumentAt(int _i) const { return m_wild || m_ellipsis || _i < cardinalChildCount(); }
	virtual bool						isWellDefined() const { for (int i = 0; i < minimumArgCount(); ++i) if (!argumentType(i)->isWellDefined()) return false; return returnType()->isWellDefined(); }

	void								setEllipsis(bool _on = true) { m_ellipsis = _on; }

	virtual Rgb							idColour() const { return 0xbb77ff; }

private:
	virtual Types						assignableTypes() const;
	virtual int							minRequired(int _i) const { return _i == Returned ? m_wild ? 0 : 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int) const { return m_wild ? Kinds() : Kinds(Kind::of<TypeConcept>()); }
	virtual String						code(String const& _middle) const;
	virtual TypeConcept*					newClone() const { return new FunctionType(m_ellipsis, m_wild); }
	virtual bool						canStandAlone() const { return false; }
	virtual bool						defineSimilarityTo(TypeConcept const* _t, Castability _c) const;
	virtual List<Declaration*>			utilisedX() const;
	virtual void						setProperties(Hash<String, String> const& _ps) { Super::setProperties(_ps); m_ellipsis = _ps[L"ellipsis"].toBool(); m_wild = _ps[L"wild"].toBool(); }

	bool m_ellipsis;
	bool m_wild;
};

}
