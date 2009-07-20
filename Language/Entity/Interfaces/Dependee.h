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

#include "Dier.h"
#include "ChangeMan.h"

namespace Martta
{

class Dependee: virtual public Dier
{
	MARTTA_INTERFACE

public:
	enum { Logically = 0x0001, Visually = 0x0002, UserAspect = Visually << 1, AllAspects = 0xffff };

protected:
	/// To be called when something about the object has changed. Notifies dependents.
	/// If _aspect & Visually then it calls a relayoutLater().
	bool								changed(int _aspect = Dependee::AllAspects) { return ChangeMan::get()->changed(this, _aspect); }

	virtual void						oneFootInTheGrave(Dependee* _replacement = 0) { ChangeMan::get()->oneFootInTheGrave(this, _replacement); }

	virtual ~Dependee() { ChangeMan::get()->dead(this); }
};

}
