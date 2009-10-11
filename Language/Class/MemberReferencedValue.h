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

#include "ReferencedValue.h"

#ifndef M_API_Class
#define M_API_Class M_OUTAPI
#endif

namespace Martta
{

class M_API_Class MemberReferencedValue: public_super ReferencedValue
{
	MARTTA_PROPER(ReferencedValue)

public:
	MemberReferencedValue(ValueDefiner* _subject = 0);

	virtual List<ValueDefiner*>			possibilities() const { return possibilities(over(), true, true); }
	static List<ValueDefiner*>			possibilities(Position const& _p, bool _methods = true, bool _nonMethods = true);

protected:
	virtual Type						apparentType() const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual String						defineEditHtml(CodeScene* _cs) const;

	virtual Kinds						ancestralDependencies() const;
	virtual void						onDependencyChanged(int _a, Concept* _d) { if (_d->isKind<MemberLambda>() || _d->isKind<Class>()) changed(Logically); Super::onDependencyChanged(_a, _d); }
};

class M_API_Class FloatingMemberReferencedValue: public_super MemberReferencedValue
{
	MARTTA_PROPER(MemberReferencedValue)

public:
	FloatingMemberReferencedValue(ValueDefiner* _subject = 0): ReferencedValue(_subject), MemberReferencedValue(_subject) {}

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual String						code() const;
};

}
