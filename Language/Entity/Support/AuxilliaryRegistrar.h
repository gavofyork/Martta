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

#include "AuxilliaryFace.h"

#include <msString.h>
#include <msList.h>
#include <msHash.h>
using MarttaSupport::List;
using MarttaSupport::Hash;
using MarttaSupport::MultiHash;
using MarttaSupport::String;
using MarttaSupport::StringList;

#ifndef M_API_Entity
#define M_API_Entity M_OUTAPI
#endif

namespace Martta
{

int registerName(int _n, char const* _name);

class AuxilliaryRegistrar
{
public:
	AuxilliaryRegistrar(): m_isInitialised(false) {}

	inline static AuxilliaryRegistrar*	get() { if (!s_this) s_this = new AuxilliaryRegistrar; return s_this; }
	void								registerName(int _n, char const* _name);
	inline List<int> const&			names() const { return m_names; }
	MarttaSupport::String const&						nameOfArbitrary(int _n) const;
	int									arbitraryOfName(String const& _name) const;

	inline AuxilliaryFace const*		auxilliary(String const& _kindName) { return m_auxilliaries.value(_kindName); }
	void								registerAuxilliary(AuxilliaryFace const* _a);
	inline List<AuxilliaryFace const*> const&	allInterfaces() const { return m_allInterfaces; }

	template<class T>
	inline List<AuxilliaryFace const*> immediateInterfaces() const { return immediateInterfaces(T::staticAuxilliary()); }
	inline List<AuxilliaryFace const*> immediateInterfaces(AuxilliaryFace const* _a) const { List<AuxilliaryFace const*> ret; for (int i = 0; i < _a->interfaceAuxilliaryCount(); i++) ret << _a->interfaceAuxilliary(i); return ret; }
	template<class T>
	inline List<AuxilliaryFace const*> immediateDeriveds() const { return immediateDeriveds(T::staticAuxilliary()); }
	inline List<AuxilliaryFace const*> immediateDeriveds(AuxilliaryFace const* _a) const { return m_immediateDerivedsMap.values(_a); }
	template<class T>
	inline List<AuxilliaryFace const*> deriveds() const { return deriveds(T::staticAuxilliary()); }
	inline List<AuxilliaryFace const*> deriveds(AuxilliaryFace const* _a) const { return m_derivedsMap.values(_a); }
	template<class T>
	inline List<AuxilliaryFace const*> interfaces() const { return interfaces(T::staticAuxilliary()); }
	inline List<AuxilliaryFace const*> interfaces(AuxilliaryFace const* _a) const { return m_interfacesMap.values(_a); }
	template<class T>
	inline List<AuxilliaryFace const*> supers() const { return supers(T::staticAuxilliary()); }
	inline List<AuxilliaryFace const*> supers(AuxilliaryFace const* _a) const { return m_supersMap.values(_a); }

	inline MultiHash<AuxilliaryFace const*, AuxilliaryFace const*> const& immediateDeriveds() const { return m_immediateDerivedsMap; }
	inline MultiHash<AuxilliaryFace const*, AuxilliaryFace const*> const& interfaces() const { return m_interfacesMap; }
	inline MultiHash<AuxilliaryFace const*, AuxilliaryFace const*> const& supers() const { return m_supersMap; }

	void								recurseAux(AuxilliaryFace const* _face, String const& _indent) const;
	template<class C> void				initialiseClasses(C* _f)
	{
		if (m_isInitialised)
			return;
		int s = m_auxilliaries.size();
		int ii = 0;
		foreach (AuxilliaryFace const* i, m_auxilliaries.values())
		{
			i->initialise();
			if (_f)
				(*_f)(ii++, s);
		}
		m_isInitialised = true;
	}
	void								operator()(int, int) {}
//	void								initialiseClasses() { initialiseClasses(this); }
	void								finaliseClasses();
	void								jigCache();

private:
	List<AuxilliaryFace const*>			calculateInterfaces(AuxilliaryFace const* _a) const;

	static AuxilliaryRegistrar*			s_this;										///< Us.
	bool								m_isInitialised;							///< We're initialised.
	List<int>							m_names;									///< The entity names.
	Hash<int, String>					m_nameMap;
	Hash<String, int>					m_invNameMap;
	Hash<String, AuxilliaryFace const*> m_auxilliaries;								///< All our auxilliaries.
	List<AuxilliaryFace const*>			m_allInterfaces;							///< All our interfaces.
	MultiHash<AuxilliaryFace const*, AuxilliaryFace const*> m_immediateDerivedsMap;	///< Immediately derived classes (only Entity-derived classes here).
	MultiHash<AuxilliaryFace const*, AuxilliaryFace const*> m_interfacesMap;		///< All interfaces fulfilled (no Entity-derived classes in the values here).
	MultiHash<AuxilliaryFace const*, AuxilliaryFace const*> m_supersMap;			///< All supers (way back to Entity together with interfaces).
	MultiHash<AuxilliaryFace const*, AuxilliaryFace const*> m_derivedsMap;			///< All deriveds.
};

}

