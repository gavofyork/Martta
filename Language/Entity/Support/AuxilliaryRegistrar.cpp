/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "AuxilliaryRegistrar.h"

namespace Martta
{

AuxilliaryRegistrar* AuxilliaryRegistrar::s_this = 0;

int registerName(int _n, char const* _name)
{
	AuxilliaryRegistrar::get()->registerName(_n, _name);
	return _n;
}

MarttaSupport::String const& AuxilliaryRegistrar::nameOfArbitrary(int _n) const
{
	AssertNR(m_nameMap.contains(_n));
	return const_cast<AuxilliaryRegistrar*>(this)->m_nameMap[_n];
}

int AuxilliaryRegistrar::arbitraryOfName(String const& _name) const
{
	AssertNR(m_invNameMap.contains(_name));
	return m_invNameMap[_name];
}

void AuxilliaryRegistrar::registerName(int _n, char const* _name)
{
	m_names << _n;
	m_nameMap.insert(_n, _name);
	m_invNameMap.insert(_name, _n);
}

void AuxilliaryRegistrar::jigCache()
{
	m_allInterfaces.clear();
	m_interfacesMap.clear();
	m_supersMap.clear();
	m_immediateDerivedsMap.clear();

	foreach (AuxilliaryFace const* i, m_auxilliaries.values())
	{
		if (i->isInterface())
			m_allInterfaces.append(i);
		else
			m_immediateDerivedsMap.insert(i->superAuxilliary(), i);
		foreach (AuxilliaryFace const* j, calculateInterfaces(i))
		{
			m_interfacesMap.insert(i, j);
			m_supersMap.insert(i, j);
			m_derivedsMap.insert(j, i);
		}
		for (AuxilliaryFace const* j = i->superAuxilliary(); j; j = j->superAuxilliary())
		{
			m_supersMap.insert(i, j);
			m_derivedsMap.insert(j, i);
		}
	}
//	recurseAux(m_immediateDerivedsMap.values(0)[0], "");
}

void AuxilliaryRegistrar::initialiseClasses()
{
	if (m_isInitialised)
		return;	// TODO: reinitialise?

	foreach (AuxilliaryFace const* i, m_auxilliaries.values())
		i->initialise();

	m_isInitialised = true;
}

void AuxilliaryRegistrar::finaliseClasses()
{
	if (!m_isInitialised)
		return;

	foreach (AuxilliaryFace const* i, m_auxilliaries.values())
		i->finalise();

	m_isInitialised = false;
}

List<AuxilliaryFace const*> AuxilliaryRegistrar::calculateInterfaces(AuxilliaryFace const* _a) const
{
	AssertNR(_a);
	List<AuxilliaryFace const*> ret;
	List<AuxilliaryFace const*> yet;
	for (AuxilliaryFace const* k = _a;; k = yet.takeLast())
	{
		for (int i = 0; i < k->interfaceAuxilliaryCount(); i++)
			if (!ret.contains(k->interfaceAuxilliary(i)))
			{
				ret << k->interfaceAuxilliary(i);
				yet << k->interfaceAuxilliary(i);
			}
		if (yet.isEmpty())
			break;
	}

	if (_a->superAuxilliary())
		foreach (AuxilliaryFace const* a, calculateInterfaces(_a->superAuxilliary()))
			if (!ret.contains(a))
				ret << a;
	return ret;
}

void AuxilliaryRegistrar::recurseAux(AuxilliaryFace const* _face, String const& _indent) const
{
	mInfo() << _indent << _face->name();
	foreach (AuxilliaryFace const* i, m_immediateDerivedsMap.values(_face))
		recurseAux(i, _indent + "|   ");
}

void AuxilliaryRegistrar::registerAuxilliary(AuxilliaryFace const* _a)
{
//	mInfo() << "Registering" << _a->name();
	m_auxilliaries[_a->name()] = _a;
}

}
