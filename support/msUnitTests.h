/* ***** BEGIN LICENSE BLOCK *****
 * Version: MarttaSupport License version 1.0
 *
 * The contents of this file are subject to the MarttaSupport License
 * version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You should have received a copy of the
 * MarttaSupport License "COPYING.MarttaSupport" along with Martta; if not
 * you may obtain a copy of the License at http://quidprocode.co.uk/Martta/
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

#include "msList.h"

namespace MarttaSupport
{

class UnitTest
{
public:
	virtual bool doTest() = 0;
	virtual char const* name() = 0;

protected:
	UnitTest();
};

class UnitTests
{
	friend class UnitTest;

public:
	static bool go();

private:
	static UnitTests* get();

	List<UnitTest*> m_tests;
	static UnitTests* s_this;
};

#define MS_TEST(Name) \
class UnitTest_ ## Name: public UnitTest \
{ \
	virtual char const* name() { return #Name; } \
	virtual bool doTest(); \
}; \
UnitTest* s_testInstance ## Name = new UnitTest_ ## Name; \
bool UnitTest_ ## Name::doTest()

}
