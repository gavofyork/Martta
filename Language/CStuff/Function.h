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

#include "LambdaNamer.h"
#include "Compound.h"
#include "TopLevel.h"

namespace Martta
{

class Function: public TopLevel, public_interface LambdaNamer
{
	MARTTA_OBJECT(TopLevel)
	MARTTA_INHERITS(LambdaNamer, 0)

public:
	Function(): m_qualifiers(NoQualifiers), m_ellipsis(false) {}

	virtual bool						ellipsis() const { return m_ellipsis; }
	void								setEllipsis(bool _e) { m_ellipsis = _e; changed(); }

protected:
	virtual int							minRequired(int _i) const { return _i == Returned || _i == Body ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;

	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); }
	virtual void						onDependencyRemoved(Entity*, int) { changed(); }

	virtual Entity*						isExpander() const { return body()->child(0); }
	virtual String						defineLayout(ViewKeys const& _v) const { return "^;" + LambdaNamer::defineLayout(_v); }
	virtual String						interfaceCode() const { return LambdaNamer::interfaceCode(); }
	virtual String						implementationCode() const { return LambdaNamer::implementationCode(); }
	virtual bool						keyPressed(KeyEvent const* _e) { return LambdaNamer::keyPressed(_e) ? true : Super::keyPressed(_e); }

	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"ellipsis"] = String::number(m_ellipsis); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_ellipsis = _p[L"ellipsis"].toBool(); }
private:
	Qualifiers							m_qualifiers;
	bool								m_ellipsis;
};

}
