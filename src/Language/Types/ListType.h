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

class SimpleOperator;
class SimpleMethod;

class ListType: public ModifyingType
{
	MARTTA_OBJECT(ModifyingType)
	
	friend class SimpleMethod;
	friend class SimpleOperator;

public:
	enum { Length = FirstNamed, EndOfNamed };

	static void							initialiseClass();
	static void							finaliseClass();
	inline static bool					keyPressedOnPosition(Position const& _p, EntityKeyEvent const* _e) { return simplePositionKeyPressHandler<ListType>(_p, _e, "[["); }
	
protected:
	// Assumes we are (eventually) a non-const ref.
	virtual Types						assignableTypes() const;
	virtual bool						hasDefaultConstructor() const { return true; }
	virtual QList<ValueDefiner*>		applicableMembers(Entity* _s = 0, bool _isConst = false) const;
	
	virtual QString						code(QString const& _middle) const;
	virtual int							minRequired(int _i) const { return _i == Original ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual Types						allowedTypes(int _i) const;
	virtual TypeEntity*					newClone() const { return new ListType; }
	virtual QString						defineLayout(ViewKeys&) const;
	virtual bool						defineSimilarityFrom(TypeEntity const* _f, Castability _c) const;
	virtual QString						idColour() const { return "#7fb"; }
	
	static QList<SimpleMethod*>			s_members;
	static QList<SimpleOperator*>		s_nonMembers;
};

}
