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

#include "ValueDefiner.h"
#include "WebViewable.h"
#include "Declaration.h"

#ifndef M_API_Namers
#define M_API_Namers M_OUTAPI
#endif

namespace Martta
{

class M_API_Namers EnumValue: public_super Declaration, public_interface ValueDefiner, public_interface WebViewable
{
	MARTTA_PROPER(Declaration)
	MARTTA_ALSO_INHERITS(ValueDefiner, 0)
	MARTTA_ALSO_INHERITS(WebViewable, 1)

	friend class Concept;
	friend class EnumValueResolver;

public:
	MARTTA_NAMED(Definition)

	// Accessor methods.
	// From ValueDefiner from TypeNamer.
	virtual Type						type() const;
	virtual String						code() const;

protected:
	virtual Identifiable*				addressableContext() const { return parentAs<Identifiable>()->addressableContext(); }
	virtual String						defineHtml() const;
	virtual int							minRequired(int _i) const { return _i == Identity ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int) const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual bool						isChildInValidState(int _i) const;
	virtual bool						isSuperfluous() const;
//	virtual bool						usurpsChild(Concept const* _e) const { return _e == child(Identity); }
	virtual void						apresLoad() { Declaration::apresLoad(); ValueDefiner::apresLoad(); WebViewable::apresLoad(); }

	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(int, Concept* _e) { if (_e == child(Identity)) { /*checkForCullingLater(); */changed(Logically); } }
};

}
