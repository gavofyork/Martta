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

#include "CQualifiers.h"
#include "VariableNamer.h"
#include "TopLevel.h"

#ifndef M_API_CStuff
#define M_API_CStuff M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_CStuff Variable: public_super TopLevel, public_interface VariableNamer
{
	MARTTA_PROPER(TopLevel)
	MARTTA_ALSO_INHERITS(VariableNamer, 0)

public:
	Qualifiers							qualifiers() const { return m_qualifiers; }

protected:
	virtual String						defineHtml() const { return L"<^>" + defineVariableHtml() + tagOf(L"minor symbol", L";"); }
	virtual List<Declaration*>			utilised() const { return actualType()->utilised(); }

	virtual bool						keyPressed(KeyEvent const* _e) { return VariableNamer::keyPressed(_e) ? true : Super::keyPressed(_e); }
	virtual int							minRequired(int _i) const { return _i == OurType ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(int _a, Concept* _e) { VariableNamer::onDependencyChanged(_a, _e); Super::onDependencyChanged(_a, _e); }
	virtual String						interfaceCode() const { return Martta::code(m_qualifiers & VariableMask) + VariableNamer::interfaceCode(); }
	virtual String						implementationCode() const { return VariableNamer::implementationCode(); }
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"qualifiers"] = String::number((uint)m_qualifiers); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_qualifiers = Qualifiers(_p[L"qualifiers"].toUint()); }
	virtual bool						isInValidState() const { return Super::isInValidState() && VariableNamer::isInValidState(); }
	virtual void						apresLoad() { TopLevel::apresLoad(); VariableNamer::apresLoad(); }

private:
	Qualifiers							m_qualifiers;
};

}
