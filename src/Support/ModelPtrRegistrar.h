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

#include <QHash>

#include "Declaration.h"

namespace Martta
{

class ModelPtrFace;
class Identifiable;

class ModelPtrRegistrar
{
public:
	ModelPtrRegistrar() { s_this = this; }

	static ModelPtrRegistrar*			get() { return s_this ? s_this : new ModelPtrRegistrar; }

	void								toBeRestored(ModelPtrFace* _p);
	void								restorePtrs(Declaration const* _root) const;
	
	void								registerDeclaration(Declaration* _e) { M_ASSERT(!m_registered.contains(_e->key())); m_registered[_e->key()] = _e; }
	void								unregisterDeclaration(Declaration* _e) { M_ASSERT(m_registered.values().contains(_e)); m_registered.remove(m_registered.key(_e)); }
	Declaration*						findDeclaration(QString const& _key) const { if (m_registered.contains(_key)) return m_registered[_key]; return 0; }
	
private:
	QHash<QString, Declaration*>		m_registered;
	mutable QList<ModelPtrFace*>		m_modelPtrs;

	static ModelPtrRegistrar*			s_this;
};

}
