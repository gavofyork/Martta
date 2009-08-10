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

namespace Martta
{

class Reference: public ModifyingType
{
	MARTTA_OBJECT(ModifyingType)

public:
	inline static bool					keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<Reference>(_p, _e, "&", false); }

private:
	virtual List<ValueDefiner*>			applicableMembers(Entity const* _s = 0, bool _isConst = false) const { return original() ? original()->applicableMembers(_s, _isConst) : Super::applicableMembers(_s); }
	virtual String						code(String const& _middle) const { return original()->code("&" + _middle); }
	virtual bool						isType(Kind _typeKind) { return Entity::isKind(_typeKind) || original()->isType(_typeKind); }
	virtual TypeEntity*					asType(Kind _typeKind) { if (Entity::isKind(_typeKind)) return this; AssertNR(original()->isType(_typeKind)); return original()->asType(_typeKind); }
	virtual Rgb							idColour() const { return original() ? original()->idColour() : TypeEntity::null->idColour(); }
	virtual TypeEntity* 				newClone() const { return new Reference; }
	virtual bool						defineSimilarityFrom(TypeEntity const* _f, Castability _c) const;
	virtual bool						defineSimilarityTo(TypeEntity const* _t, Castability _c) const;
	virtual String						defineLayout(ViewKeys const&) const;
	virtual String						defineHtml() const;
	virtual List<Declaration*>			utilised() const { return List<Declaration*>(); }
};

}