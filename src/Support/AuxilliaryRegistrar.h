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

#include <QString>

#include "AuxilliaryFace.h"

namespace Martta
{

int registerName(int _n, char const* _name);

class AuxilliaryRegistrar
{
public:
	AuxilliaryRegistrar(): m_isInitialised(false) {}

	inline static AuxilliaryRegistrar*	get() { if (!s_this) s_this = new AuxilliaryRegistrar; return s_this; }
	void								registerName(int _n, char const* _name);
	inline QList<int> const&			names() const { return m_names; }
	QString const&						nameOfArbitrary(int _n) const;
	int									arbitraryOfName(QString const& _name) const;
	
	inline AuxilliaryFace const*		auxilliary(QString const& _kindName) { return m_auxilliaries[_kindName]; }
	void								registerAuxilliary(AuxilliaryFace const* _a);
	inline QList<AuxilliaryFace const*> const&	allInterfaces() const { return m_allInterfaces; }
	
	template<class T>
	inline QList<AuxilliaryFace const*> immediateInterfaces() const { return immediateInterfaces(T::staticAuxilliary()); } 
	inline QList<AuxilliaryFace const*> immediateInterfaces(AuxilliaryFace const* _a) const { QList<AuxilliaryFace const*> ret; for (int i = 0; i < _a->interfaceAuxilliaryCount(); i++) ret << _a->interfaceAuxilliary(i); return ret; }
	template<class T>
	inline QList<AuxilliaryFace const*> immediateDeriveds() const { return immediateDeriveds(T::staticAuxilliary()); } 
	inline QList<AuxilliaryFace const*> immediateDeriveds(AuxilliaryFace const* _a) const { return m_immediateDerivedsMap.values(_a); } 
	template<class T>
	inline QList<AuxilliaryFace const*> interfaces() const { return interfaces(T::staticAuxilliary()); } 
	inline QList<AuxilliaryFace const*> interfaces(AuxilliaryFace const* _a) const { return m_interfacesMap.values(_a); }
	template<class T>
	inline QList<AuxilliaryFace const*> supers() const { return supers(T::staticAuxilliary()); } 
	inline QList<AuxilliaryFace const*> supers(AuxilliaryFace const* _a) const { return m_supersMap.values(_a); }
	
	inline QMultiMap<AuxilliaryFace const*, AuxilliaryFace const*> const& immediateDeriveds() const { return m_immediateDerivedsMap; }
	inline QMultiMap<AuxilliaryFace const*, AuxilliaryFace const*> const& interfaces() const { return m_interfacesMap; }
	inline QMultiMap<AuxilliaryFace const*, AuxilliaryFace const*> const& supers() const { return m_supersMap; }
	
	void								recurseAux(AuxilliaryFace const* _face, QString const& _indent) const;
	void								initialiseClasses();
	void								finaliseClasses();
	void								jigCache();
	
private:
	QList<AuxilliaryFace const*>		calculateInterfaces(AuxilliaryFace const* _a) const;

	static AuxilliaryRegistrar*			s_this;										///< Us.
	bool								m_isInitialised;							///< We're initialised.
	QList<int>							m_names;									///< The entity names.
	QHash<int, QString>					m_nameMap;
	QHash<QString, int>					m_invNameMap;
	QHash<QString, AuxilliaryFace const*> m_auxilliaries;							///< All our auxilliaries.
	QList<AuxilliaryFace const*>		m_allInterfaces;							///< All our interfaces.
	QMultiMap<AuxilliaryFace const*, AuxilliaryFace const*> m_immediateDerivedsMap;	///< Immediately derived classes (only Entity-derived classes here).
	QMultiMap<AuxilliaryFace const*, AuxilliaryFace const*> m_interfacesMap;		///< All interfaces fulfilled (no Entity-derived classes in the values here).
	QMultiMap<AuxilliaryFace const*, AuxilliaryFace const*> m_supersMap;			///< All supers (way back to Entity together with interfaces).
};

}

