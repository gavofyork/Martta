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

#include <errno.h>

#include <QDomDocument>
#include <QDomElement>

#include "TestHelper.h"
#include "Meta.h"

#include "Type.h"
#include "TypeEntity.h"
#include "Const.h"
#include "Base.h"
#include "Reference.h"
#include "Array.h"
#include "Memberify.h"
#include "BuiltinType.h"
#include "ExplicitType.h"
#include "FunctionType.h"
#include "ModifyingType.h"
#include "TextLabel.h"
#include "AccessLabel.h"
#include "ListType.h"
#include "StringType.h"
#include "Root.h"
#include "Pointer.h"
#include "Enumeration.h"
#include "EnumValue.h"
#include "Argument.h"

#include "Method.h"
#include "Variable.h"
#include "Referenced.h"
#include "Namespace.h"
#include "Compound.h"
using namespace Martta;

class SPT: public SafePointerTarget { public: SafePointerTarget::rewirePointer; };

using namespace Martta;
class TagA
{
	MARTTA_INTERFACE
public:
	void killAndDelete() { delete this; }
	virtual char tagAVirtual() { return 'A'; }
	virtual ~TagA() {}
};
MARTTA_INTERFACE_CPP(TagA);


class Another
{
public:
	virtual void fooBar() {}
	virtual ~Another() {}
};

class TagB
{
	MARTTA_INTERFACE
public:
	void killAndDelete() { delete this; }
	virtual char tagBVirtual() { return 'B'; }
	virtual ~TagB() {}
};
MARTTA_INTERFACE_CPP(TagB);

class TagC
{
	MARTTA_INTERFACE
public:
	void killAndDelete() { delete this; }
	virtual char tagCVirtual() { return 'C'; }
	virtual ~TagC() {}
};
MARTTA_INTERFACE_CPP(TagC);

class TagD: public Another, public TagB
{
	MARTTA_INTERFACE
	MARTTA_INHERITS(TagB, 0)
public:
	void killAndDelete() { delete this; }
	virtual char tagDVirtual() { return 'D'; }
	virtual ~TagD() {}
};
MARTTA_INTERFACE_CPP(TagD);

class TagE
{
	MARTTA_INTERFACE
public:
	void killAndDelete() { delete this; }
	virtual char tagEVirtual() { return 'E'; }
	virtual ~TagE() {}
};
MARTTA_INTERFACE_CPP(TagE);

class TestEntity: public Entity, public TagA, public TagE
{
	MARTTA_OBJECT(Entity)
	MARTTA_INHERITS(TagA, 0)
	MARTTA_INHERITS(TagE, 1)
public:
	virtual void oneVirtual() {};
	void killAndDelete() { Entity::killAndDelete(); }
};
MARTTA_OBJECT_CPP(TestEntity);

class TestEntityB: public TestEntity, public TagC, public TagD
{
	MARTTA_OBJECT(TestEntity)
	MARTTA_INHERITS(TagC, 0)
	MARTTA_INHERITS(TagD, 1)
public:
	virtual void anotherVirtual() {};
	void killAndDelete() { Entity::killAndDelete(); }
};
MARTTA_OBJECT_CPP(TestEntityB);

class TestNegatives: public Entity
{
	MARTTA_OBJECT(Entity)
public:
	MARTTA_NAMED(NamedChildB)
	
	enum { NamedChildA = FirstNamed, EndOfNamed };
};
MARTTA_OBJECT_CPP(TestNegatives);
MARTTA_NAMED_CPP(TestNegatives, NamedChildB);

class TestNegativesB: public TestNegatives
{
	MARTTA_OBJECT(TestNegatives)
	
public:
	enum { NamedChildC = FirstNamed, EndOfNamed };
	int minRequired(int _i) const { return _i != NamedChildA ? _i != NamedChildB ? _i != NamedChildC ? Super::minRequired(_i) : 2 : 1 : 0; }
	Kinds allowedKinds(int) const { return Kind::of<TestNegatives>(); }
};
MARTTA_OBJECT_CPP(TestNegativesB);

int coreTests()
{
	int failed = 0;
	
#define DERIVES_MAIN(BASE, DERIVED) \
	FAILED_IF(Kind::of<BASE>().isKind<DERIVED>()); \
	FAILED_IF(!Kind::of<DERIVED>().isKind<BASE>()); \
	FAILED_IF(Kind::of<BASE>().isKind(DERIVED::staticKind)); \
	FAILED_IF(!Kind::of<DERIVED>().isKind(BASE::staticKind)); \
	FAILED_IF(Kind::of<BASE>().isKind(Kind::of<DERIVED>())); \
	FAILED_IF(!Kind::of<DERIVED>().isKind(Kind::of<BASE>()));
#define DERIVES_ONE(BASE, DERIVED) { \
	DERIVED& d = *new DERIVED; \
	FAILED_IF(!d.Entity::isKind(BASE::staticKind)); \
	FAILED_IF(!d.isKind<BASE>()); \
	FAILED_IF(!d.Entity::isKind(Kind::of<BASE>())); \
	d.DERIVED::killAndDelete(); \
	}
#define DERIVES_OTHER(BASE, DERIVED) { \
	BASE& b = *new BASE; \
	DERIVED& d = *new DERIVED; \
	FAILED_IF(b.isKind<DERIVED>()); \
	FAILED_IF(b.Entity::isKind(d.staticKind)); \
	FAILED_IF(b.Entity::isKind(Kind::of<DERIVED>())); \
	d.DERIVED::killAndDelete(); \
	b.BASE::killAndDelete(); \
	}
#define DERIVES_TT(BASE, DERIVED) DERIVES_MAIN(BASE, DERIVED)
#define DERIVES_TO(BASE, DERIVED) DERIVES_MAIN(BASE, DERIVED) DERIVES_ONE(BASE, DERIVED)
#define DERIVES_OO(BASE, DERIVED) DERIVES_MAIN(BASE, DERIVED) DERIVES_ONE(BASE, DERIVED) DERIVES_OTHER(BASE, DERIVED)
#define UNRELATED(BASE, DERIVED) \
	FAILED_IF(Kind::of<BASE>().isKind<DERIVED>()); \
	FAILED_IF(Kind::of<DERIVED>().isKind<BASE>()); \
	FAILED_IF(Kind::of<BASE>().isKind(DERIVED::staticKind)); \
	FAILED_IF(Kind::of<DERIVED>().isKind(BASE::staticKind)); \
	FAILED_IF(Kind::of<BASE>().isKind(Kind::of<DERIVED>())); \
	FAILED_IF(Kind::of<DERIVED>().isKind(Kind::of<BASE>())); \

	TEST("Entity derivation")
	{
		DERIVES_OO(Entity, TestEntity);
		DERIVES_OO(Entity, TestEntityB);
		UNRELATED(Entity, TagA);
		UNRELATED(Entity, TagB);
		UNRELATED(Entity, TagC);
		UNRELATED(Entity, TagD);
	}
	TEST("Tag derivation")
	{
		DERIVES_TO(TagA, TestEntity);
		DERIVES_TO(TagA, TestEntityB);
		UNRELATED(TagA, TagB);
		UNRELATED(TagA, TagC);
		UNRELATED(TagA, TagD);
	}
	TEST("TestEntity derivation")
	{
		DERIVES_OO(TestEntity, TestEntityB);
		UNRELATED(TestEntity, TagB);
		UNRELATED(TestEntity, TagC);
		UNRELATED(TestEntity, TagD);
	}
	TEST("TagB derivation")
	{
		DERIVES_TO(TagB, TestEntityB);
		UNRELATED(TagB, TagC);
		DERIVES_TT(TagB, TagD);
	}
	TEST("TagC derivation")
	{
		DERIVES_TO(TagC, TestEntityB);
		UNRELATED(TagC, TagD);
	}
	TEST("TagD derivation")
	{
		DERIVES_TO(TagD, TestEntityB);
	}
	TEST("Tag with asKind")
	{
		Entity* t = new TestEntity;
		Entity* b = new TestEntityB;
		char* v = (char*)0x1000;
/*		mDebug() << (void*)static_cast<Entity*>((TestEntity*)v);
		mDebug() << (void*)static_cast<TagA*>((TestEntity*)v);
		mDebug() << (void*)static_cast<TagE*>((TestEntity*)v);
		mDebug() << t << t->tryKind<TagA>() << " " << t->tryKind<TagB>() << " " << t->tryKind<TagC>() << " " << t->tryKind<TagD>() << " " << t->tryKind<TagE>();
		mDebug() << (void*)static_cast<Entity*>((TestEntityB*)v);
		mDebug() << (void*)static_cast<TagA*>((TestEntityB*)v);
		mDebug() << (void*)static_cast<TagB*>((TestEntityB*)v);
		mDebug() << (void*)static_cast<TagC*>((TestEntityB*)v);
		mDebug() << (void*)static_cast<TagD*>((TestEntityB*)v);
		mDebug() << (void*)static_cast<TagE*>((TestEntityB*)v);
		mDebug() << b << b->tryKind<TagA>() << " " << b->tryKind<TagB>() << " " << b->tryKind<TagC>() << " " << b->tryKind<TagD>() << " " << b->tryKind<TagE>();
		mDebug() << b->asKind<TagA>()->tagAVirtual();
		mDebug() << b->asKind<TagB>()->tagBVirtual();
		mDebug() << b->asKind<TagC>()->tagCVirtual();
		mDebug() << b->asKind<TagD>()->tagDVirtual();
		mDebug() << b->asKind<TagE>()->tagEVirtual();
*/		FAILED_IF(((char*)static_cast<Entity*>((TestEntity*)v) - v) != ((char*)(t->asKind<Entity>()) - (char*)t));
		FAILED_IF((char*)static_cast<TagA*>((TestEntity*)v) - v != (char*)(t->asKind<TagA>()) - (char*)t);
		FAILED_IF((char*)static_cast<TagE*>((TestEntity*)v) - v != (char*)(t->asKind<TagE>()) - (char*)t);
		FAILED_IF((char*)static_cast<Entity*>((TestEntityB*)v) - v != (char*)(b->asKind<Entity>()) - (char*)b);
		FAILED_IF((char*)static_cast<TagA*>((TestEntityB*)v) - v != (char*)(b->asKind<TagA>()) - (char*)b);
		FAILED_IF((char*)static_cast<TagB*>((TestEntityB*)v) - v != (char*)(b->asKind<TagB>()) - (char*)b);
		FAILED_IF((char*)static_cast<TagC*>((TestEntityB*)v) - v != (char*)(b->asKind<TagC>()) - (char*)b);
		FAILED_IF((char*)static_cast<TagD*>((TestEntityB*)v) - v != (char*)(b->asKind<TagD>()) - (char*)b);
		FAILED_IF((char*)static_cast<TagE*>((TestEntityB*)v) - v != (char*)(b->asKind<TagE>()) - (char*)b);
		FAILED_IF(t->asKind<TagA>()->tagAVirtual() != 'A');
		FAILED_IF(t->asKind<TagE>()->tagEVirtual() != 'E');
		FAILED_IF(b->asKind<TagA>()->tagAVirtual() != 'A');
		FAILED_IF(b->asKind<TagB>()->tagBVirtual() != 'B');
		FAILED_IF(b->asKind<TagC>()->tagCVirtual() != 'C');
		FAILED_IF(b->asKind<TagD>()->tagDVirtual() != 'D');
		FAILED_IF(b->asKind<TagE>()->tagEVirtual() != 'E');
		b->killAndDelete();
		t->killAndDelete();
	}
	TEST("Safe pointer assignment")
	{
		SafePointer<SPT> p;
		{
			SPT a;
			p = &a;
		}
		FAILED_IF(p);
	}
	TEST("Safe pointer copying")
	{
		SafePointer<SPT> q;
		{
			SafePointer<SPT> p;
			SPT a;
			p = &a;
			q = p;
		}
		FAILED_IF(q);
	}
	TEST("Safe pointer construction")
	{
		SafePointer<SPT> q;
		{
			SPT a;
			SafePointer<SPT> p(&a);
			q = p;
		}
		FAILED_IF(q);
	}
	TEST("Safe pointer rewiring")
	{
		SPT a;
		SafePointer<SPT> p = &a;
		{
			SPT b;
			a.rewirePointer(&b);
		}
		FAILED_IF(p);
	}
	/*TEST("Pointer archival/restoration")
	{
		Root* r = new Root;
		Namespace* n = new Namespace;
		n->prepareChildren();
		r->back().place(n);
		n->childAs<TextLabel>(Identifiable::Identity)->setText("n");

		Class* X = new Class;
		X->prepareChildren();
		n->back().place(X);
		X->childAs<TextLabel>(Identifiable::Identity)->setText("X");

		Class* Y = new Class;
		Y->prepareChildren();
		n->back().place(Y);
		Y->childAs<TextLabel>(Identifiable::Identity)->setText("Y");

		Method* M = new Method;
		M->prepareChildren();
		X->back().place(M);
		M->childAs<TextLabel>(Identifiable::Identity)->setText("foo");
		M->child(LambdaNamer::Returned)->replace(new BuiltinType(Void));
		
		Argument* v = new Argument;
		v->middle(Identifiable::Identity).place(new TextLabel("a"));
		v->middle(VariableNamer::OurType).place(new ExplicitType(Y));
		M->back().place(v);
		
		Referenced* f = new Referenced(v); 
		f->prepareChildren();
		M->childOf<Compound>()->back().place(f);
		
		r->restorePtrs();
		QDomDocument doc;
		QDomElement prj = doc.createElement("project");
		doc.appendChild(prj);
		QDomElement ele = doc.createElement("entity");
		ele.setAttribute("kind", n->kind().name().toCString());
		prj.appendChild(ele);
		
		r->archivePtrs();
		n->exportDom(ele);
		n->killAndDelete();
		r->importDom(doc.documentElement());
		r->restorePtrs();
	
		FAILED_IF(!r->child(0)->childOf<Class>()->childOf<Method>()->childOf<Compound>()->childIs<Referenced>(1));
		mDebug() << &*r->child(0)->childOf<Class>()->childOf<Method>()->childOf<Compound>()->childAs<Referenced>(1)->subject();
		FAILED_IF(!r->child(0)->childOf<Class>()->childOf<Method>()->childOf<Compound>()->childAs<Referenced>(1)->subject());
		
		r->killAndDelete();
	}*/
	TEST("Pre-test")
	{
		Root* r = new Root;
		SafePointer<TestNegatives> a = new TestNegatives;
		SafePointer<TestNegatives> b = new TestNegatives;
		a->silentMove(r->back());
		b->silentMove(a->middle(0));
		r->killAndDelete();
		FAILED_IF(a);
		FAILED_IF(b);
	}
#define CLEAR_TEST(pos) \
	{ \
		Root* r = new Root; \
		SafePointer<TestNegatives> a = new TestNegatives; \
		SafePointer<TestNegatives> b = new TestNegatives; \
		a->silentMove(r->back()); \
		b->silentMove(pos ? a->middle(pos) : a->back()); \
		TEST("Clearing entities (children: " #pos ")") FAILED_IF(!a->children().contains(b)); \
		TEST("Clearing entities (cardinalChildren(): " #pos ")") FAILED_IF(bool(pos) == a->cardinalChildren().contains(b)); \
		TEST("Clearing entities (children(int): " #pos ")") FAILED_IF(!a->children(pos).contains(b)); \
		r->killAndDelete(); \
		TEST("Clearing entities (a pointer: " #pos ")") FAILED_IF(a); \
		TEST("Clearing entities (b pointer: " #pos ")") FAILED_IF(b); \
	}
	CLEAR_TEST(0)
	CLEAR_TEST(TestNegatives::NamedChildA)
	CLEAR_TEST(TestNegatives::NamedChildB)
#undef CLEAR_TEST
#define TEST_FOR(T, X) TEST(T) FAILED_IF(!(X))
	{
		Root r;
		SafePointer<TestNegativesB> a = new TestNegativesB;
		a->silentMove(r.back());
		TEST_FOR("Negatives: start incomplete", !a->isComplete());
		a->prepareChildren();
		TEST_FOR("Negatives: prepareChildren() makes complete", a->isComplete());
		TEST_FOR("Negatives: no entities", a->cardinalChildCount() == 0);
		TEST_FOR("Negatives: no As", a->childCount(TestNegativesB::NamedChildA) == 0);
		TEST_FOR("Negatives: one B", a->childCount(TestNegativesB::NamedChildB) == 1);
		TEST_FOR("Negatives: two Cs", a->childCount(TestNegativesB::NamedChildC) == 2);
		TEST_FOR("Negatives: no 'D's", a->childCount(TestNegativesB::NamedChildC + 1) == 0);
	mInfo() << "News/Deletes/Remaining = " << s_news << "/" << s_deletes << "/" << (s_news - s_deletes);
		a->child(TestNegativesB::NamedChildC)->replace(new Label);
		TEST_FOR("Negatives: bad replacement makes incomplete", !a->isComplete());
	mInfo() << "News/Deletes/Remaining = " << s_news << "/" << s_deletes << "/" << (s_news - s_deletes);
		a->validifyChildren();
		TEST_FOR("Negatives: validifyChildren() makes complete", a->isComplete());
	mInfo() << "News/Deletes/Remaining = " << s_news << "/" << s_deletes << "/" << (s_news - s_deletes);
		a->validifyChildren();
		TEST_FOR("Negatives: 2nd validifyChildren() and still complete", a->isComplete());
	}
	mInfo() << "News/Deletes/Remaining = " << s_news << "/" << s_deletes << "/" << (s_news - s_deletes);
	/*TEST("Negatives save/load")
	{
		Root r;
		SafePointer<TestNegativesB> a = new TestNegativesB;
		a->silentMove(r.back());
		a->prepareChildren();

		QDomDocument doc;
		QDomElement prj = doc.createElement("project");
		doc.appendChild(prj);
		QDomElement ele = doc.createElement("entity");
		ele.setAttribute("kind", a->kind().name().toCString());
		prj.appendChild(ele);
		
		r.archivePtrs();
		a->exportDom(ele);
		a->killAndDelete();
		r.importDom(doc.documentElement());
		r.restorePtrs();
		a = r.childAs<TestNegativesB>(0);
		
		FAILED_IF(a->cardinalChildCount() != 0);
		FAILED_IF(a->childCount(TestNegativesB::NamedChildA) != 0);
		FAILED_IF(a->childCount(TestNegativesB::NamedChildB) != 1);
		FAILED_IF(a->childCount(TestNegativesB::NamedChildC) != 2);
		FAILED_IF(a->childCount(TestNegativesB::NamedChildC + 1) != 0);
	}*/
	
	return failed;
}































