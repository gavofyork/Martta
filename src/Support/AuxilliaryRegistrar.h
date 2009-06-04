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

#include "Kind.h"
#include "AuxilliaryFace.h"

namespace Martta
{

class AuxilliaryRegistrar
{
public:
	inline static AuxilliaryRegistrar*	get() { if (!s_this) s_this = new AuxilliaryRegistrar; return s_this; }
	
	inline AuxilliaryFace const*		auxilliary(QString const& _kindName) { return m_auxilliaries[_kindName]; }
	void								registerAuxilliary(AuxilliaryFace const* _a);
	template<class T> inline Kinds		immediateDeriveds() const { return immediateDeriveds(T::staticAuxilliary()); } 
	inline Kinds						immediateDeriveds(Kind _k) const { return m_derivedMap.values(_k.auxilliary()); } 
	
	void								recurseAux(AuxilliaryFace const* _face, QString const& _indent) const;
	
	void								initialiseAll() const;
	
private:
	static AuxilliaryRegistrar*			s_this;
	QHash<QString, AuxilliaryFace const*> m_auxilliaries;
	QList<AuxilliaryFace const*>		m_interfaces;
	QMultiMap<AuxilliaryFace const*, AuxilliaryFace const*> m_derivedMap;
	QMultiMap<AuxilliaryFace const*, AuxilliaryFace const*> m_altDerivedMap;
};

}

