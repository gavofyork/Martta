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

#include "Referenced.h"

namespace Martta
{

class MemberReferenced: public Referenced
{
	MARTTA_OBJECT(Referenced)

public:
	MemberReferenced(ValueDefiner* _subject = 0): Referenced(_subject) {}

	virtual List<ValueDefiner*>			possibilities() const { return possibilities(over()); }
	static List<ValueDefiner*>			possibilities(Position const& _p);

protected:
	virtual Type						apparentType() const;
	virtual bool						keyPressed(KeyEvent const* _e);

	virtual Kinds						ancestralDependencies() const;
	virtual bool						isInValidState() const;
};

class FloatingMemberReferenced: public MemberReferenced
{
	MARTTA_OBJECT(MemberReferenced)

public:
	FloatingMemberReferenced(ValueDefiner* _subject = 0): MemberReferenced(_subject) {}

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual String						code() const;
};

}
