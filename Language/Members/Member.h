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

#include "AccessLabel.h"
#include "Declaration.h"

namespace Martta
{

class TypeDefintion;

/**
 * Class for any entity that can be found inside a class (i.e. with an access attribute).
 */
class Member: public Declaration
{
	MARTTA_PLACEHOLDER(Declaration)

public:
	enum { Accessibility = FirstNamed, EndOfNamed };

	virtual Access						access() const;
	TypeDefinition*						typeDefinition() const;

	virtual String						interfaceCode() const;
	virtual String						implementationCode() const { return memberImplementationCode(); }

protected:
	// New virtuals.
	virtual String						memberInterfaceCode() const { return String::null; }
	virtual String						memberImplementationCode() const { return String::null; }
	virtual String						memberDefineHtml() const { return String::null; }

	// Old virtuals.
	virtual int							minRequired(int _i) const { return _i == Accessibility || _i == Identity ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
//	virtual void						decorate(DecorationContext const& _p) const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); }
	virtual String						defineHtml() const;
};

}
