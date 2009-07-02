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

class Const: public ModifyingType
{
	MARTTA_OBJECT(ModifyingType)

public:
	Const() {}
	
	inline static bool					keyPressedOnPosition(Position const& _p, EntityKeyEvent const* _e) { return simplePositionKeyPressHandler<Const>(_p, _e, "!", false); }
	
protected:
	virtual bool						hasDefaultConstructor() const { return original() && original()->hasDefaultConstructor(); }
	virtual QList<ValueDefiner*>		applicableMembers(Entity* _s = 0, bool = false) const { return original() ? original()->applicableMembers(_s, true) : Super::applicableMembers(_s); }
	virtual bool						isType(Kind _typeKind) { return Entity::isKind(_typeKind) || original()->isType(_typeKind); }
	virtual TypeEntity*					asType(Kind _typeKind) { if (Entity::isKind(_typeKind)) return this; M_ASSERT(original()->isType(_typeKind)); return original()->asType(_typeKind); }
	virtual QString						code(QString const& _middle) const;
	virtual QString						defineLayout(ViewKeys&) const;
	virtual TypeEntity*					newClone() const { return new Const; }
	virtual void						decorate(DecorationContext const& _c) const;
	virtual Kinds						deniedKinds(int _i) const;
	virtual bool						canStandAlone() const { return false; }
	virtual bool						defineSimilarityFrom(TypeEntity const* _f, Castability _c) const;
	virtual bool						defineSimilarityTo(TypeEntity const* _t, Castability _c) const;
};

}
