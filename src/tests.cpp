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

#define MS_TEST_EXISTANCE_1(bar)\
enum bar##MethodExistsTrue {};\
enum bar##MethodExistsFalse {};\
template<class T, class A, void (T::*)(A)> struct bar##MethodExistsStruct1 {};\
template<class T, class A> bar##MethodExistsFalse& bar##IfMethodExists1(...) { return *(bar##MethodExistsFalse*)0; }\
template<class T, class A> bar##MethodExistsTrue& bar##IfMethodExists1(bar##MethodExistsStruct1<T, A, &T::bar>*) { return *(bar##MethodExistsTrue*)0; }\
template<class T, class A> inline void __TEST_##bar(T& _a, A _arg, bar##MethodExistsTrue&) { _a.bar(_arg); }\
template<class T, class A> inline void __TEST_##bar(T&, A, bar##MethodExistsFalse&){}\
template<class T, class A> inline void bar(T& _a, A _arg) { __TEST_##bar(_a, _arg, bar##IfMethodExists1<T, A>(0)); }

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

MS_TEST_EXISTANCE_1(resize)

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
	
	TEST("Method existance")
	{
		String s = L"Hello world!";
		resize(s, 5u);
		FAILED_IF(s != L"Hello");
		int i;
		resize(i, 5);
	}
	
	failed += supportTests();
	TEST("Auxilliaries Initialisation")
		AuxilliaryRegistrar::get()->jigCache();
		
	TEST("Entity evaluator")
	{
		Entity* e = Entity::evaluate(L"Root");
		UNLESS(e->kind().name() == L"Martta::Root")("Couldn't create root entity through evaluation");
		TypeEntity* te = Entity::evaluate(L"BuiltinType[id=0]")->asKind<TypeEntity>();
		UNLESS(te->code() == L"void")("Couldn't create void type entity through evaluation");
		delete te;
		te = Entity::evaluate(L"Const{BuiltinType[id=16]}")->asKind<TypeEntity>();
		UNLESS(te->code() == L"int const")("Couldn't create int const type entity through evaluation");
		delete te;
		e->back().place(Entity::evaluate(L"Namespace{TextLabel[text=project]}{Class{TextLabel[text=program]}{Method{TextLabel[text=main]}{BuiltinType[id=0]}}}"));
		e->debugTree();
		e->killAndDelete();
	}

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































