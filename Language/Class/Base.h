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
#include "WebViewable.h"
#include "Concept.h"

#ifndef M_API_Class
#define M_API_Class M_OUTAPI
#endif

namespace Martta
{

class M_CLASS Class;

class M_CLASS M_API_Class Base: public_super Concept, public_interface WebViewable
{
	MARTTA_PROPER(Concept)
	MARTTA_ALSO_INHERITS(WebViewable, 0)

public:
	MARTTA_NAMED(Accessibility)
	MARTTA_NAMED(Superclass)

	Access								access() const;
	String								code() const;
	Class*								classType() const;

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual String						defineHtml() const;
	virtual Kinds						allowedKinds(int _i) const;
	virtual void						onDependencyChanged(int, Concept* _e);
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual int							minRequired(int _i) const { return _i == Accessibility || _i == Superclass ? 1 : Super::minRequired(_i); }
	virtual bool						isChildInValidState(int _i) const;
	virtual bool						keyPressed(KeyEvent const* _e);
};

}
