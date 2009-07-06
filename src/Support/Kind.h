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
#include <msList.h>
using namespace MarttaSupport;
//using MarttaSupport::List;

#include "AuxilliaryRegistrar.h"
#include "AuxilliaryFace.h"

namespace Martta
{

class Entity;
class Kind;

class Kinds: public List<Kind>
{
public:
	Kinds() {}
	Kinds(Kind const& _k) { append(_k); }
	Kinds(List<Kind> const& _k): List<Kind>(_k) {}
	inline Kinds(List<AuxilliaryFace const*> const& _k);
	
	inline Kinds operator,(Kind const& _c) const { return Kinds(*this) << _c; }
	
	inline Kinds& operator<<(Kind const& _k) { append(_k); return *this; }
	inline Kinds& operator<<(Kinds const& _k) { this->List<Kind>::operator<<(_k); return *this; }
	
	inline bool containsBaseOf(Kind const& _derived) const;
	inline bool containsKindOf(Kind const& _base) const;
	inline Kind commonBase() const;
};

class Kind
{
	friend class Entity;
	
public:
	Kind(): m_mo(0) {}
	Kind(AuxilliaryFace const* _mo): m_mo(_mo) {}
	template<class T> inline static Kind of() { return Kind(T::staticAuxilliary()); }

	inline Kind super() const { return m_mo ? Kind(m_mo->superAuxilliary()) : Kind(); }
	Kinds deriveds() const;
	Kinds immediateDeriveds() const { return m_mo ? AuxilliaryRegistrar::get()->immediateDeriveds(m_mo) : Kinds(); }
	Kinds interfaces() const { return m_mo ? AuxilliaryRegistrar::get()->interfaces(m_mo) : Kinds(); }
	Kinds immediateInterfaces() const { return m_mo ? AuxilliaryRegistrar::get()->immediateInterfaces(m_mo) : Kinds(); }
	Kinds supers() const { return m_mo ? AuxilliaryRegistrar::get()->supers(m_mo) : Kinds(); }
	inline String name() const { return m_mo ? m_mo->name() : 0; }
	inline AuxilliaryFace const* auxilliary() const { return m_mo; }
	Entity* spawnPrepared() const;

	inline bool isKind(Kind _base) const { return _base.m_mo == m_mo || AuxilliaryRegistrar::get()->supers().contains(m_mo, _base.m_mo); }
	inline bool isKind(Kinds const& _bases) const;
	template<class T>
	inline bool isKind() const { return isKind(T::staticKind); }

	inline bool isBaseOf(Kind _derived) const { return _derived.isKind(*this); }
	inline bool isBaseOf(Kinds const& _deriveds) const { return _deriveds.containsKindOf(*this); }
	template<class T>
	inline bool isBaseOf() const { return isBaseOf(T::staticKind); }

	inline bool strictlyDerivedFrom(Kind _base) const { return super().isKind(_base); }
	inline bool strictBaseOf(Kind _derived) const { return _derived.strictlyDerivedFrom(*this); }

	inline bool operator==(Kind _c) const { return m_mo == _c.m_mo; }
	inline bool operator!=(Kind _c) const { return !operator==(_c); }
	inline bool operator<(Kind _cmp) const { return m_mo < _cmp.m_mo; }
	inline Kinds operator,(Kind _c) const { return List<Kind>() << *this << _c; }

private:
	AuxilliaryFace const* m_mo;
};

}

inline uint qHash(Martta::Kind const& _k)
{
	return qHash(_k.auxilliary());
}

inline bool Martta::Kind::isKind(Kinds const& _bases) const
{
	return _bases.containsBaseOf(*this);
}

inline Martta::Kinds::Kinds(List<AuxilliaryFace const*> const& _k)
{
	foreach (AuxilliaryFace const* i, _k)
		append(Kind(i));
}

inline bool Martta::Kinds::containsBaseOf(Kind const& _derived) const
{
	foreach (Kind k, *this)
	if (_derived.isKind(k))
		return true;
	return false;
}

inline bool Martta::Kinds::containsKindOf(Kind const& _base) const
{
	foreach (Kind k, *this)
		if (k.isKind(_base))
			return true;
	return false;
}

inline Martta::Kind Martta::Kinds::commonBase() const
{
	AssertNR(size());
	Kind best = at(0);
	for (int i = 1; i < size(); ++i)
		if (best.isKind(at(i)))
			best = at(i);
		else
			while (!at(i).isKind(best))
				best = best.super();
	return best;
}
