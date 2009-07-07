/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include <msDebug.h>

#include "TestHelper.h"

#include "Type.h"
#include "ExplicitType.h"
#include "Root.h"
#include "Class.h"
#include "Const.h"
#include "Reference.h"
#include "TypeEntity.h"
using namespace Martta;

int supportTests();
int coreTests();
int typeTests();
int changeTests();

char const* s_asserted;

#if defined(DEBUG)
namespace MarttaTest
{
void assertFailed(int _line, char const* _file, char const* _function, char const* _failed, char const* _reason)
{
	fprintf(stderr, "ASSERTION FAILED: line %d of %s in %s: %s\n", _line, _file, _function, _reason);
//	__asm{int 3};
	s_asserted = _failed;
}
}
#endif

int test()
{
	int failed = 0;

#if defined(DEBUG)	
	MarttaSupport::s_alternativeAssertionHandler = &MarttaTest::assertFailed;
	TEST("Asserting")
	{
		bool testHasActuallySucceeded = true;
		(void)(testHasActuallySucceeded);
		AssertNR(!testHasActuallySucceeded);
		FAILED_IF(!s_asserted);
		s_asserted = 0;
	}
#endif
	
	failed += supportTests();
	TEST("Auxilliaries Initialisation")
		AuxilliaryRegistrar::get()->jigCache();
	failed += coreTests();
	failed += typeTests();
	failed += changeTests();


	{
		Root r;
		Class* C = new Class;
		C->prepareChildren();
		r.back().place(C);
		Type t;
		t = Type(ExplicitType(C)).topWith(Const());
		Type u;
		u = t.toppedWith(Reference());
		String code;
		code = u->code();
		mDebug() << code;
	}
	
#if defined(DEBUG)
	TEST("Memory leaking")
	{
		mInfo() << "News/Deletes/Remaining/Types = " << s_news << "/" << s_deletes << "/" << (s_news - s_deletes) << "/" << TypeEntity::s_typeCount;
		FAILED_IF(s_news - s_deletes != 1);
		FAILED_IF(TypeEntity::s_typeCount != 1);
	}
#endif
	
	MarttaSupport::s_alternativeAssertionHandler = 0;
	mInfo() << "PASSED :-)";
	
	return failed;
}































