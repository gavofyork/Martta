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

#include "Identifiable.h"
#include "Entity.h"

namespace Martta
{

class ValueDefiner;

/**
 * Class for anything individually referencable in the language.
 * Currently this includes only functions, variables, types and enumeration values.
 */
class Declaration: public Entity, public_interface Identifiable
{
	MARTTA_PLACEHOLDER(Entity)
	MARTTA_INHERITS(Identifiable, 0)

public:
	virtual ~Declaration();

	/// From Identifiable (default implementations).
	virtual String						key() const;
	virtual Identifiable*				lookupChild(String const& _key) const;
	void								archivePtrs(bool _clear = false) const;
	void								restorePtrs() const;
	
	virtual int							registerAnonymous(Identifiable const* _e, Identifiable const** _registrar) const { if (_registrar) *_registrar = this; if (m_anonyma.contains(_e)) return m_anonyma.indexOf(_e); m_anonyma << _e; return m_anonyma.size() - 1; }
	virtual void						registerAnonymous(Identifiable const* _e, int _k) { while (m_anonyma.size() <= _k) m_anonyma << 0; m_anonyma[_k] = _e; }

	List<Declaration*>					utilisedSiblings() const;
	virtual List<Declaration*>			utilised() const;

	List<ValueDefiner*>					valuesKnown() const;
	virtual List<ValueDefiner*>			valuesAdded() const { return List<ValueDefiner*>(); }
	
	virtual String						interfaceCode() const { return String(); }
	virtual String						implementationCode() const { return String(); }

	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); Identifiable::properties(_p); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); Identifiable::setProperties(_p); }

private:
	mutable List<Identifiable const*>	m_anonyma;
};

}
