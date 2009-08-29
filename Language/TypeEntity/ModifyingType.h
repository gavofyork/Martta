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

#include "WebViewable.h"
#include "TypeEntity.h"

#ifndef M_API_TypeEntity
#define M_API_TypeEntity M_OUTAPI
#endif

namespace Martta
{

// Note the 'child' (i.e. the type that is modified in some way) is always at position 0. This
// means that in derived classes which need other children (e.g. Array, Memberify) they have
// to ensure that the 'child' is in place before any secondary entities are added.

class M_API_TypeEntity ModifyingType: public TypeEntity, public_interface WebViewable
{
	MARTTA_PLACEHOLDER(TypeEntity)
	MARTTA_INHERITS(WebViewable, 0)

public:
	enum { Original = Default };

	inline TypeEntity*					original() const { return childIs<TypeEntity>(Original) ? childAs<TypeEntity>(Original) : TypeEntity::null; }

	/// Deletes this object and puts its child in its place.
	void								unknit();
	virtual bool						isUltimatelyNull() const { return isPlaceholder() || !original() || original()->isUltimatelyNull(); }
	virtual bool						isWellDefined() const { return !isPlaceholder() && original() && original()->isWellDefined(); }

protected:
	virtual int							minRequired(int _i) const { return _i == Original ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _index) const;
	virtual Rgb							idColour() const { return original() ? original()->idColour() : TypeEntity::null->idColour(); }
	virtual String						defineHtml() const { return toHtml(child(Original)) + L"<^>" + modifierHtml(); }
	virtual String						modifierHtml() const { return String::null; }
	virtual TypeEntity*					newClone() const { return new ModifyingType; }
	virtual TypeEntity*					bottom() { return childIs<TypeEntity>(Original) ? childAs<TypeEntity>(Original) : this; }
	virtual List<Declaration*>			utilisedX() const { return original() ? original()->utilised() : Super::utilised(); }
};

}
