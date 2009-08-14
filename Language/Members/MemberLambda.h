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
#include "MemberValue.h"

namespace Martta
{

// Just for non-static callables

class MemberLambda: public MemberValue, public_interface LambdaNamer
{
	MARTTA_PLACEHOLDER(MemberValue)
	MARTTA_INHERITS(LambdaNamer, 0)

public:
	enum { Constness = FirstNamed, EndOfNamed };

	virtual bool						isConst() const;
	Type								thisType() const;

protected:
	virtual String						definePreHtml() const;
	virtual String						definePostHtml() const;

	/// Override if you want a completely different layout (rather than the LambdaNamer template).
	/// Overriding this will still do the const decoration etc. If you don't want that, override Member::memberDefineHtml or defineHtml.
	virtual String						memberLambdaDefineHtml() const { return LambdaNamer::defineHtml(); }

	virtual String						memberImplementationCode() const { return LambdaNamer::implementationCode(); }
	virtual String						memberInterfaceCode() const { return LambdaNamer::interfaceCode(); }

	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); }
	virtual void						onDependencyRemoved(Entity*, int) { changed(); }
	virtual List<Declaration*>			utilised() const;
	virtual int							minRequired(int _i) const { return _i == Constness || _i == Body || _i == Returned ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;

	virtual String						basicCode(FunctionCodeScope _ref) const;

	virtual bool						isInValidState() const { return type().isWellDefined(); }
	virtual Entity*						isExpander() const;
	virtual String						memberDefineHtml() const;
	virtual bool						keyPressed(KeyEvent const* _e);
//	virtual void						memberDecorate(DecorationContext const& _p) const;
	virtual Type						type() const { return MemberValue::memberifiedType(LambdaNamer::type()); }
};

}
