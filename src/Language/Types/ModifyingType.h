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

#include "TypeEntity.h"

namespace Martta
{

// Note the 'child' (i.e. the type that is modified in some way) is always at position 0. This
// means that in derived classes which need other children (e.g. Array, Memberify) they have
// to ensure that the 'child' is in place before any secondary entities are added.
	
class ModifyingType: public TypeEntity
{
	MARTTA_PLACEHOLDER(TypeEntity)

public:
	inline TypeEntity*					child() const { return entityIs<TypeEntity>(0) ? entityAs<TypeEntity>(0) : TypeEntity::null; }

	/// Deletes this object and puts its child in its place.
	void								unknit();
	virtual bool						isUltimatelyNull() const { return !child() || child()->isUltimatelyNull(); }
	virtual bool						isWellDefined() const { return child() && child()->isWellDefined(); }
	
protected:
	virtual int							minimumRequired() const { return 1; }
	virtual Kinds						allowedKinds(int _index) const;
	virtual QString						idColour() const { return child() ? child()->idColour() : TypeEntity::null->idColour(); }
	virtual QString						defineLayout(ViewKeys&) const { return "0;^;" + modifierLayout(); }
	virtual QString						modifierLayout() const { return QString(); }
	virtual TypeEntity*					newClone() const { return new ModifyingType; }
	virtual TypeEntity*					bottom() { return entityIs<TypeEntity>(0) ? entityAs<TypeEntity>(0) : this; }
	virtual bool						canStandAlone() const { return child() ? child()->canStandAlone() : false; }
	virtual QList<DeclarationEntity*>	utilisedX() const { return child() ? child()->utilised() : Super::utilised(); }
};

}
