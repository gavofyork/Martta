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

class SimpleMethod;
class Simple;
class SimpleOperator;

class StringType: public TypeEntity
{
	MARTTA_OBJECT(TypeEntity)
	
	friend class SimpleMethod;
	friend class SimpleOperator;

public:
	static void							initialiseClass();
	static void							finaliseClass();
	inline static bool					keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e) { return simpleInsertionPointKeyPressHandler<StringType>(_p, _e, "$"); }
	virtual bool						isWellDefined() const { return true; }
	
protected:
	virtual bool						hasDefaultConstructor() const { return true; }
	virtual Types						assignableTypes() const;
	virtual QList<ValueDefiner*>		applicableMembers(Entity* _s = 0, bool _isConst = false) const;
	
	virtual QString						code(QString const& _middle) const;
	virtual QString						defineLayout(ViewKeys&) const;
	virtual bool						defineSimilarityFrom(TypeEntity const* _f, Castability _c) const;
	virtual bool						defineSimilarityTo(TypeEntity const* _t, Castability _c) const;
	virtual QString						idColour() const { return "#fb7"; }

	virtual TypeEntity*					newClone() const { return new StringType; }
	
	static QList<SimpleMethod*>			s_members;	
	static QList<SimpleOperator*>		s_nonMembers;	
};

}
