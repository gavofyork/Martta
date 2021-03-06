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
#include "WebViewable.h"
#include "WebInformer.h"
#include "Concept.h"

#ifndef M_API_Declaration
#define M_API_Declaration M_OUTAPI
#endif

namespace Martta
{

class M_CLASS ValueDefiner;

/**
 * Class for anything individually referencable in the language.
 * Currently this includes only functions, variables, types and enumeration values.
 */
class M_CLASS M_API_Declaration Declaration: public_super Concept, public_interface Identifiable, public_interface WebViewable, public_interface WebInformer
{
	MARTTA_PLACEHOLDER(Concept)
	MARTTA_ALSO_INHERITS(Identifiable, 0)
	MARTTA_ALSO_INHERITS(WebViewable, 1)
	MARTTA_ALSO_INHERITS(WebInformer, 2)

public:
	virtual ~Declaration();

	virtual String						superChildInformationHtml(Concept const* _e) const;

	/// From Identifiable (default implementations).
	virtual String						key() const;
	virtual Identifiable*				lookupChild(String const& _key) const;
	void								restorePtrs() const;

	virtual int							registerAnonymous(Identifiable const* _e, Identifiable const** _registrar) const;
	virtual void						registerAnonymous(Identifiable const* _e, int _k);

	List<Declaration*>					utilisedSiblings() const;
	virtual List<Declaration*>			utilised() const;

	List<ValueDefiner*>					valuesKnown() const;
	virtual List<ValueDefiner*>			valuesAdded() const { return List<ValueDefiner*>(); }

	virtual String						interfaceCode() const { return String::null; }
	virtual String						implementationCode() const { return String::null; }

	virtual void						apresLoad() { Concept::apresLoad(); Identifiable::apresLoad(); }

	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); Identifiable::properties(_p); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); Identifiable::setProperties(_p); }

private:
	mutable List<Identifiable const*>	m_anonyma;
};

}
