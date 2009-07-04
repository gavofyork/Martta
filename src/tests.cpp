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

#include <QDebug>

#include "TestHelper.h"

#include "Type.h"
#include "ExplicitType.h"
#include "Root.h"
#include "Class.h"
#include "Const.h"
#include "Reference.h"


#include "TypeEntity.h"
namespace Martta
{
extern int g_typeCount;
}
using namespace Martta;

int supportTests();
int coreTests();
int typeTests();
int changeTests();

int test()
{
	s_testing = true;
	int failed = 0;
	TEST("Asserting")
	{
		bool testHasActuallySucceeded = true;
		M_ASSERT(!testHasActuallySucceeded);
		FAILED_IF(!s_asserted);
		s_asserted = 0;
	}
	
	failed += supportTests();
	TEST("Auxilliaries Initialisation")
		AuxilliaryRegistrar::get()->jigCache();
/*	failed += coreTests();
	failed += typeTests();
	failed += changeTests();
*/

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
	
	TEST("Memory leaking")
	{
		mInformation() << "News/Deletes/Remaining/Types = " << s_news << "/" << s_deletes << "/" << (s_news - s_deletes) << "/" << g_typeCount;
		FAILED_IF(s_news - s_deletes != 1);
		FAILED_IF(g_typeCount != 1);
	}
	
	s_testing = false;
	mInformation() << "PASSED :-)";
	
//	exit(failed);
	return failed;
}































