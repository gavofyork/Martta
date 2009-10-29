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

#include <msHash.h>
#include <msList.h>
#include <msString.h>
using namespace MarttaSupport;

#include "Identifiable.h"

#ifndef M_API_Declaration
#define M_API_Declaration M_OUTAPI
#endif

namespace Martta
{

class ModelPtrFace;
class Identifiable;

class M_API_Declaration ModelPtrRegistrar
{
public:
	ModelPtrRegistrar() { s_this = this; }

	static ModelPtrRegistrar*			get() { return s_this ? s_this : new ModelPtrRegistrar; }

	void								toBeRestored(ModelPtrFace* _p);
	void								restorePtrs(Identifiable const* _root);
	void								restorePtrsOf(Identifiable const* _id);

	void								registerTemp(Identifiable const* _e, String const& _key) { AssertNR(!m_registered.contains(_key)); m_tempRegistered[_key] = const_cast<Identifiable*>(_e); }
	void								registerDeclaration(Identifiable* _e) { AssertNR(!m_registered.contains(_e->key())); m_registered[_e->key()] = _e; }
	void								unregisterDeclaration(Identifiable* _e) { AssertNR(m_registered.values().contains(_e)); m_registered.remove(m_registered.key(_e)); }

	Identifiable*						find(String const& _key) const { if (m_registered.contains(_key)) return m_registered[_key]; if (m_tempRegistered.contains(_key)) return m_tempRegistered[_key]; return 0; }

private:
	Hash<String, Identifiable*>			m_registered;
	Hash<String, Identifiable*>			m_tempRegistered;
	mutable List<ModelPtrFace*>			m_modelPtrs;

	static ModelPtrRegistrar*			s_this;
};

}
