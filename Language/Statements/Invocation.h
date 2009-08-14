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

#include "Evaluation.h"

namespace Martta
{

class Invocation: public Evaluation
{
	MARTTA_OBJECT(Evaluation)

public:
	enum { Callee = FirstNamed, EndOfNamed };

	String								callList() const { return callList(castEntities<Typed>(cardinalChildren())); }
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual int							minRequired(int _i) const;
	virtual Kinds						allowedKinds(int _index) const;
	virtual Types						allowedTypes(int _index) const;
	virtual Type						type() const;
	virtual String						code() const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual String						defineHtml() const;
	String								callList(List<Typed*> _parameters) const;
	virtual int							familyDependencies() const { return Super::familyDependencies() | DependsOnChildren; }
	virtual void						onDependencyChanged(Entity* _e);
};

}
