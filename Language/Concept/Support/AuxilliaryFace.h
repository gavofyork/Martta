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

#include <msSupport.h>
using namespace MarttaSupport;

#include "ConceptSupport.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

class KeyEvent;
class Position;

class AuxilliaryFace
{
public:
	virtual ~AuxilliaryFace() {}
	virtual AuxilliaryFace const*		interfaceAuxilliary(int _i) const = 0;
	virtual int							interfaceAuxilliaryCount() const = 0;
	virtual AuxilliaryFace const*		superAuxilliary() const = 0;
	virtual bool						isInterface() const = 0;
	virtual bool						isPlaceholder() const = 0;
	virtual char const*					name() const = 0;
//	virtual RawConstMemory				offset(Concept const*) const = 0;

	// They will return 0 is called on an Interface.
	virtual Concept*						create() const = 0;
	virtual void						initialise() const = 0;
	virtual void						finalise() const = 0;
	virtual bool						dispatchKeyPress(Position const& _p, KeyEvent const* _e) const = 0;
};

}
