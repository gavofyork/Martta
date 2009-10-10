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

#include "ModifyingType.h"

#ifndef M_API_SimpleTypes
#define M_API_SimpleTypes M_OUTAPI
#endif

namespace Martta
{

class M_API_SimpleTypes Const: public_super ModifyingType
{
	MARTTA_PROPER(ModifyingType)

public:
	Const() {}

	inline static bool					keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<Const>(_p, _e, "!", false); }

protected:
	virtual bool						hasDefaultConstructor() const { return original() && original()->hasDefaultConstructor(); }
	virtual List<ValueDefiner*>			applicableMembers(Concept const* _s = 0, bool = false) const { return original() ? original()->applicableMembers(_s, true) : Super::applicableMembers(_s); }
	virtual bool						isType(Kind _typeKind) { return Concept::isKind(_typeKind) || original()->isType(_typeKind); }
	virtual TypeConcept*					asType(Kind _typeKind) { if (Concept::isKind(_typeKind)) return this; AssertNR(original()->isType(_typeKind)); return original()->asType(_typeKind); }
	virtual String						code(String const& _middle) const;
	virtual String						defineHtml() const;
	virtual TypeConcept*					newClone() const { return new Const; }
	virtual Kinds						deniedKinds(int _i) const;
//	virtual bool						canStandAlone() const { return false; }
	virtual bool						canStandAlone() const { return original() ? original()->canStandAlone() : false; }
	virtual bool						defineSimilarityFrom(TypeConcept const* _f, Castability _c) const;
	virtual bool						defineSimilarityTo(TypeConcept const* _t, Castability _c) const;
	virtual bool						keyPressed(KeyEvent const* _e);
};

}
