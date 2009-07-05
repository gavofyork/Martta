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

#include "Invocation.h"
#include "ModelPtr.h"
#include "Constructor.h"

namespace Martta
{

class Construction: public Invocation
{
	MARTTA_OBJECT(Invocation)

public:
	Construction(Constructor* _c = 0) : m_subject(0) { setDependency(m_subject, _c); }

	virtual int							minRequired(int _i) const;
	virtual Kinds						allowedKinds(int _index) const;
	virtual Types						allowedTypes(int _index) const;

	virtual Type						type() const;
	virtual String						code() const;

	virtual void						apresLoad() { addDependency(m_subject); Super::apresLoad(); }
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"subject"] = m_subject.key(); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_subject.restoreFrom(_p[L"subject"]); }

protected:
	ModelPtr<Constructor>				m_subject;
};

}
