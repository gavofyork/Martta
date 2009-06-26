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

#include "msList.h"
#include "msString.h"
#include "msHash.h"
#include "msSupport.h"
using namespace MarttaSupport;

#include "Meta.h"

#include "Type.h"
#include "TypeEntity.h"
#include "Const.h"
#include "Base.h"
#include "Reference.h"
#include "Array.h"
#include "Memberify.h"
#include "SimpleType.h"
#include "ExplicitType.h"
#include "FunctionType.h"
#include "ModifyingType.h"
#include "TextLabel.h"
#include "AccessLabel.h"
#include "ListType.h"
#include "StringType.h"
#include "BasicRoot.h"
#include "Pointer.h"
#include "Enumeration.h"
#include "EnumValue.h"
#include "Argument.h"

#include "Method.h"
#include "Variable.h"
#include "Referenced.h"
#include "NamespaceEntity.h"
#include "Compound.h"
using namespace Martta;

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
typename GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey>::Box returnMeBox(GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey> const& _me)
{
	return _me;
}

template<typename Key, typename T, t::uint Min, bool AlwaysMulti, bool ImplicitKey>
GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey> const returnMe(GeneralHash<Key, T, Min, AlwaysMulti, ImplicitKey> const& _me)
{
	return _me;
}

template<typename T>
typename List<T>::Box returnMeBox(List<T> const& _me)
{
	return _me;
}

template<typename T>
List<T> returnMe(List<T> const& _me)
{
	return _me;
}

String::Box returnMeBox(String const& _me) { return _me; }
String returnMe(String const& _me) { return _me; }

class TestHelper
{
public:
	TestHelper(char const* _n, int* _c):
		m_name			(_n),
		m_failed		(false),
		m_done			(false),
		m_oldAsserted	(s_asserted),
		m_failedCounter	(_c)
	{
		s_asserted = false;
		qInformation() << "Testing" << m_name;
	}
	~TestHelper()
	{
		if (m_failed || s_asserted)
		{
			qInformation() << "FAILED! " << s_asserted;
			exit(1);
			(*m_failedCounter)++;
		}
		s_asserted = m_oldAsserted;
	}
	void failed() { m_failed = true; }
	bool isDone() const { return m_done; }
	void done() { m_done = true; }
private:
	char const* m_name;
	bool m_failed;
	bool m_done;
	char const* m_oldAsserted;
	int* m_failedCounter;
};
#define TEST(S) for (TestHelper _h(S, &failed); !_h.isDone(); _h.done()) 
#define FAILED_IF(X) if (X) _h.failed(); else (void)0

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

	TEST("Auxilliaries Initialisation")
	{
		AuxilliaryRegistrar::get()->jigCache();
	}
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
/*		qDebug() << (void*)static_cast<Entity*>((TestEntity*)v);
		qDebug() << (void*)static_cast<TagA*>((TestEntity*)v);
		qDebug() << (void*)static_cast<TagE*>((TestEntity*)v);
		qDebug() << t << t->tryKind<TagA>() << " " << t->tryKind<TagB>() << " " << t->tryKind<TagC>() << " " << t->tryKind<TagD>() << " " << t->tryKind<TagE>();
		qDebug() << (void*)static_cast<Entity*>((TestEntityB*)v);
		qDebug() << (void*)static_cast<TagA*>((TestEntityB*)v);
		qDebug() << (void*)static_cast<TagB*>((TestEntityB*)v);
		qDebug() << (void*)static_cast<TagC*>((TestEntityB*)v);
		qDebug() << (void*)static_cast<TagD*>((TestEntityB*)v);
		qDebug() << (void*)static_cast<TagE*>((TestEntityB*)v);
		qDebug() << b << b->tryKind<TagA>() << " " << b->tryKind<TagB>() << " " << b->tryKind<TagC>() << " " << b->tryKind<TagD>() << " " << b->tryKind<TagE>();
		qDebug() << b->asKind<TagA>()->tagAVirtual();
		qDebug() << b->asKind<TagB>()->tagBVirtual();
		qDebug() << b->asKind<TagC>()->tagCVirtual();
		qDebug() << b->asKind<TagD>()->tagDVirtual();
		qDebug() << b->asKind<TagE>()->tagEVirtual();
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
	TEST("Pointer archival/restoration")
	{
		BasicRoot* r = new BasicRoot;
		NamespaceEntity* n = new NamespaceEntity;
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
		M->child(LambdaNamer::Returned)->replace(new SimpleType(Void));
		
		Argument* v = new Argument;
		v->middle(Identifiable::Identity).place(new TextLabel("a"));
		v->middle(VariableNamer::OurType).place(new ExplicitType(Y));
		M->back().place(v);
		
		Referenced* f = new Referenced(v); 
		f->prepareChildren();
		M->childOf<Compound>()->back().place(f);
		
		r->archivePtrs(true);
		r->restorePtrs();
		QDomDocument doc;
		QDomElement prj = doc.createElement("project");
		doc.appendChild(prj);
		QDomElement ele = doc.createElement("entity");
		ele.setAttribute("kind", n->kind().name());
		prj.appendChild(ele);
		
		r->archivePtrs();
		n->exportDom(ele);
		n->killAndDelete();
		r->importDom(doc.documentElement());
		r->restorePtrs();
	
		FAILED_IF(!r->child(0)->childOf<Class>()->childOf<Method>()->childOf<Compound>()->childIs<Referenced>(1));
		qDebug() << &*r->child(0)->childOf<Class>()->childOf<Method>()->childOf<Compound>()->childAs<Referenced>(1)->subject();
		FAILED_IF(!r->child(0)->childOf<Class>()->childOf<Method>()->childOf<Compound>()->childAs<Referenced>(1)->subject());
		
		r->killAndDelete();
	}
	TEST("Pre-test")
	{
		BasicRoot* r = new BasicRoot;
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
		BasicRoot* r = new BasicRoot; \
		SafePointer<TestNegatives> a = new TestNegatives; \
		SafePointer<TestNegatives> b = new TestNegatives; \
		a->silentMove(r->back()); \
		b->silentMove(pos ? a->middle(pos) : a->back()); \
		TEST("Clearing entities (children: " #pos ")") FAILED_IF(!a->children().contains(b)); \
		TEST("Clearing entities (cardinalChildren(): " #pos ")") FAILED_IF(bool(pos) == a->cardinalChildren().contains(b)); \
		TEST("Clearing entities (childrenAt(int): " #pos ")") FAILED_IF(!a->childrenAt(pos).contains(b)); \
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
		BasicRoot* r = new BasicRoot;
		SafePointer<TestNegativesB> a = new TestNegativesB;
		a->silentMove(r->back());
		TEST_FOR("Negatives: start incomplete", !a->isComplete());
		a->prepareChildren();
		TEST_FOR("Negatives: prepareChildren() makes complete", a->isComplete());
		TEST_FOR("Negatives: no entities", a->cardinalChildCount() == 0);
		TEST_FOR("Negatives: no As", a->childCountAt(TestNegativesB::NamedChildA) == 0);
		TEST_FOR("Negatives: one B", a->childCountAt(TestNegativesB::NamedChildB) == 1);
		TEST_FOR("Negatives: two Cs", a->childCountAt(TestNegativesB::NamedChildC) == 2);
		TEST_FOR("Negatives: no 'D's", a->childCountAt(TestNegativesB::NamedChildC + 1) == 0);
		a->child(TestNegativesB::NamedChildC)->replace(new Label);
		TEST_FOR("Negatives: bad replacement makes incomplete", !a->isComplete());
		a->validifyChildren();
		TEST_FOR("Negatives: validifyChildren() makes complete", a->isComplete());
		a->validifyChildren();
		TEST_FOR("Negatives: 2nd validifyChildren() and still complete", a->isComplete());
		r->clearEntities();
	}
	TEST("Negatives save/load")
	{
		BasicRoot* r = new BasicRoot;
		SafePointer<TestNegativesB> a = new TestNegativesB;
		a->silentMove(r->back());
		a->prepareChildren();

		QDomDocument doc;
		QDomElement prj = doc.createElement("project");
		doc.appendChild(prj);
		QDomElement ele = doc.createElement("entity");
		ele.setAttribute("kind", a->kind().name());
		prj.appendChild(ele);
		
		r->archivePtrs();
		a->exportDom(ele);
		a->killAndDelete();
		r->importDom(doc.documentElement());
		r->restorePtrs();
		a = r->childAs<TestNegativesB>(0);
		
		FAILED_IF(a->cardinalChildCount() != 0);
		FAILED_IF(a->childCountAt(TestNegativesB::NamedChildA) != 0);
		FAILED_IF(a->childCountAt(TestNegativesB::NamedChildB) != 1);
		FAILED_IF(a->childCountAt(TestNegativesB::NamedChildC) != 2);
		FAILED_IF(a->childCountAt(TestNegativesB::NamedChildC + 1) != 0);
		r->clearEntities();
	}
	TEST("Type construction adoption")
	{
		SafePointer<TypeEntity> p;
		{
			Type t(Void);
			p = &*t;
			{
				Type t(*p);
			}
			FAILED_IF(!p);
		}
		FAILED_IF(p);
	}
	TEST("Type assignment adoption")
	{
		SafePointer<TypeEntity> p;
		{
			Type t(Void);
			p = &*t;
			{
				Type t;
				t = *p;
			}
			FAILED_IF(!p);
		}
		FAILED_IF(p);
	}
	TEST("Complex type construction non-adoption")
	{
		SafePointer<TypeEntity> p = new Reference;
		SafePointer<TypeEntity> q = new SimpleType(Void);
		p->back().insertSilent(q);
		{
			Type t(*q);
		}
		FAILED_IF(!q);
		q->killAndDelete();
		p->killAndDelete();
	}
	TEST("Type assignment non-adoption")
	{
		SafePointer<TypeEntity> p = new Reference;
		SafePointer<TypeEntity> q = new SimpleType(Void);
		p->back().insertSilent(q);
		{
			Type t;
			t = *q;
		}
		FAILED_IF(!q);
		q->killAndDelete();
		p->killAndDelete();
	}
	TEST("Type construction value non-adoption")
	{
		SafePointer<TypeEntity> p = new SimpleType(Void);
		{
			Type t(*p);
		}
		FAILED_IF(!p);
		p->killAndDelete();
		if (!failed)
			TEST("Type creation syntax")
			{
				Type t = Type(SimpleType(Void));
			}
	}
	
	TEST("Class creation...")
	{
		BasicRoot* r = new BasicRoot;
		Class* X = new Class;
		X->prepareChildren();
		r->back().place(X);
		X->changed();
		FAILED_IF(!X->isValid());
		foreach (Entity* e, X->children())
			FAILED_IF(!e->isValid());
	}
#define TEST_THIS_CAST(F, T, R) \
	if (TypeEntity::R == TypeEntity::Logical) \
		FAILED_IF(!F.isSimilarTo(T, TypeEntity::Convertible) || !F.isSimilarTo(T, TypeEntity::Physical)); \
	else if (TypeEntity::R == TypeEntity::Convertible) \
		FAILED_IF(!F.isSimilarTo(T, TypeEntity::Convertible) || F.isSimilarTo(T, TypeEntity::Physical)); \
	else if (TypeEntity::R == TypeEntity::Physical) \
		FAILED_IF(F.isSimilarTo(T, TypeEntity::Convertible) || !F.isSimilarTo(T, TypeEntity::Physical)); \
	else \
		FAILED_IF(F.isSimilarTo(T, TypeEntity::Convertible) || F.isSimilarTo(T, TypeEntity::Physical));

	{
#define CAST_TEST(F, T, R) TEST("FunctionType casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		// ? (t1)(?)
		Type f = Type(FunctionType(false, true));
		// void (t2)()
		Type fv = Type(FunctionType()).place(SimpleType(Void), FunctionType::Returned);
		// int (t3)()
		Type fi = Type(FunctionType()).place(SimpleType(Int), FunctionType::Returned);
		CAST_TEST(f, f, Logical);
		CAST_TEST(f, fv, Unrelated);
		CAST_TEST(f, fi, Unrelated);
		CAST_TEST(fv, f, Logical);
		CAST_TEST(fv, fv, Logical);
		CAST_TEST(fv, fi, Unrelated);
		CAST_TEST(fi, f, Logical);
		CAST_TEST(fi, fv, Unrelated);
		CAST_TEST(fi, fi, Logical);
#undef CAST_TEST
	}
	{
#define CAST_TEST(F, T, R) TEST("SimpleType Conversion casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		Type b = Type(Bool);
		Type bc = Type(Bool).topWith(Const());
		Type i = Type(Int);
		Type ir = Type(Int).topWith(Reference());
		CAST_TEST(b, b, Logical);
		CAST_TEST(b, bc, Logical);
		CAST_TEST(b, i, Unrelated);
		CAST_TEST(b, ir, Unrelated);
		CAST_TEST(bc, b, Convertible);
		CAST_TEST(bc, bc, Logical);
		CAST_TEST(bc, i, Unrelated);
		CAST_TEST(bc, ir, Unrelated);
		CAST_TEST(i, b, Convertible);
		CAST_TEST(i, bc, Convertible);
		CAST_TEST(i, i, Logical);
		CAST_TEST(i, ir, Unrelated);
		CAST_TEST(ir, b, Convertible);
		CAST_TEST(ir, bc, Convertible);
		CAST_TEST(ir, i, Convertible);
		CAST_TEST(ir, ir, Logical);
#undef CAST_TEST
	}
	{
#define CAST_TEST(F, T, R) TEST("EnumValue Conversion casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		BasicRoot* r = new BasicRoot;
		Enumeration* X = new Enumeration;
		X->prepareChildren();
		X->setUnnamed();
		r->back().place(X);
		EnumValue* Xv = new EnumValue;
		Xv->prepareChildren();
		X->back().place(Xv);
		Enumeration* Y = new Enumeration;
		Y->prepareChildren();
		Y->setNamed();
		Y->childAs<TextLabel>(Identifiable::Identity)->setText("Y");
		r->back().place(Y);
		EnumValue* Yv = new EnumValue;
		Yv->prepareChildren();
		Y->back().place(Yv);
		Type x = Xv->type();
		Type y = Yv->type();
		Type i = Type(Int);
		qDebug() << x->code();
		qDebug() << y->code();
		qDebug() << i->code();
//		CAST_TEST(x, x, Logical);
		CAST_TEST(x, y, Unrelated);
		CAST_TEST(x, i, Logical);
//		CAST_TEST(y, x, Unrelated);
		CAST_TEST(y, y, Logical);
		CAST_TEST(y, i, Unrelated);
//		CAST_TEST(i, x, Logical);
		CAST_TEST(i, y, Unrelated);
		CAST_TEST(i, i, Logical);
#undef CAST_TEST
	}
	{
#define CAST_TEST(F, T, R) TEST("Const casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		Type i = Type(Int);
		Type ir = Type(Int).topWith(Reference());
		Type ic = Type(Int).topWith(Const());
		Type icr = Type(Int).topWith(Const()).topWith(Reference());
		Type vr = Type(Void).topWith(Reference());
		CAST_TEST(i, i, Logical);
		CAST_TEST(i, ir, Unrelated);
		CAST_TEST(i, ic, Logical);
		CAST_TEST(i, icr, Convertible);
		CAST_TEST(i, vr, Unrelated);
		CAST_TEST(ir, i, Convertible);
		CAST_TEST(ir, ir, Logical);
		CAST_TEST(ir, ic, Convertible);
		CAST_TEST(ir, icr, Logical);
		CAST_TEST(ir, vr, Logical);
		CAST_TEST(ic, i, Convertible);
		CAST_TEST(ic, ir, Unrelated);
		CAST_TEST(ic, ic, Logical);
		CAST_TEST(ic, icr, Convertible);
		CAST_TEST(ic, vr, Unrelated);
		CAST_TEST(icr, i, Convertible);
		CAST_TEST(icr, ir, Unrelated);
		CAST_TEST(icr, ic, Convertible);
		CAST_TEST(icr, icr, Logical);
		CAST_TEST(icr, vr, Unrelated);
		CAST_TEST(vr, i, Unrelated);
		CAST_TEST(vr, ir, Unrelated);
		CAST_TEST(vr, ic, Unrelated);
		CAST_TEST(vr, icr, Unrelated);
		CAST_TEST(vr, vr, Logical);
#undef CAST_TEST
	}
	TEST("Class construction testing.")
	{
		BasicRoot* r = new BasicRoot;
		Class* X = new Class;
		X->prepareChildren();
		r->back().place(X);
		Type x = Type(X);
		delete r;
	}
	{
#define CAST_TEST(F, T, R) TEST("Reference casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		BasicRoot* r = new BasicRoot;
		Class* X = new Class;
		X->prepareChildren();
		r->back().place(X);
		{
		Type x = Type(X);
		Type xr = Type(X).topWith(Reference());
		Type xcr = Type(X).topWith(Const()).topWith(Reference());
		CAST_TEST(x, x, Logical);
		CAST_TEST(x, xr, Unrelated);
		CAST_TEST(x, xcr, Convertible);
		CAST_TEST(xr, x, Convertible);
		CAST_TEST(xr, xr, Logical);
		CAST_TEST(xr, xcr, Logical);
		CAST_TEST(xcr, x, Convertible);
		CAST_TEST(xcr, xr, Unrelated);
		CAST_TEST(xcr, xcr, Logical);
		}
		X->killAndDelete();
		delete r;
#undef CAST_TEST
	}
	{
#define CAST_TEST(F, T, R) TEST("Memberify casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		BasicRoot* r = new BasicRoot;
		Class* B = new Class;
		Class* D = new Class;
		B->prepareChildren();
		B->childAs<TextLabel>(Identifiable::Identity)->setText("B");
		D->prepareChildren();
		D->childAs<TextLabel>(Identifiable::Identity)->setText("D");
		r->back().place(B);
		r->back().place(D);
		Base* b = new Base;
		b->middle(Base::Accessibility).place(new AccessLabel(Public));
		b->middle(Base::Superclass).place(new ExplicitType(B));
		D->back().place(b);
		{
		// void (B:: b)()
		Type b = Type(FunctionType()).place(SimpleType(Void), FunctionType::Returned).topWith(Memberify(Type(B)));
		// void (B:: bc)() const
		Type bc = Type(FunctionType()).place(SimpleType(Void), FunctionType::Returned).topWith(Memberify(Type(B).topWith(Const())));
		// void (D:: d)()
		Type d = Type(FunctionType()).place(SimpleType(Void), FunctionType::Returned).topWith(Memberify(Type(D)));
		// void (D:: dc)() const
		Type dc = Type(FunctionType()).place(SimpleType(Void), FunctionType::Returned).topWith(Memberify(Type(D).topWith(Const())));
		CAST_TEST(b, b, Logical);
		CAST_TEST(b, bc, Unrelated);
		CAST_TEST(b, d, Logical);
		CAST_TEST(b, dc, Unrelated);
		CAST_TEST(bc, b, Logical);
		CAST_TEST(bc, bc, Logical);
		CAST_TEST(bc, d, Logical);
		CAST_TEST(bc, dc, Logical);
		CAST_TEST(d, b, Unrelated);
		CAST_TEST(d, bc, Unrelated);
		CAST_TEST(d, d, Logical);
		CAST_TEST(d, dc, Unrelated);
		CAST_TEST(dc, b, Unrelated);
		CAST_TEST(dc, bc, Unrelated);
		CAST_TEST(dc, d, Logical);
		CAST_TEST(dc, dc, Logical);
		}
		D->killAndDelete();
		B->killAndDelete();
		delete r;
#undef CAST_TEST
	}
	{
#define CAST_TEST(F, T, R) TEST("String casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		{
		Type s = Type(StringType());
		Type scr = Type(StringType()).topWith(Const()).topWith(Reference());
		Type ccs = Type(Char).topWith(Const()).topWith(Pointer());
		CAST_TEST(s, s, Logical);
		CAST_TEST(s, scr, Convertible);
		CAST_TEST(s, ccs, Convertible);
		CAST_TEST(scr, s, Convertible);
		CAST_TEST(scr, scr, Logical);
		CAST_TEST(scr, ccs, Convertible);
		CAST_TEST(ccs, s, Convertible);
		CAST_TEST(ccs, scr, Convertible);
		CAST_TEST(ccs, ccs, Logical);
		}
#undef CAST_TEST
	}

	List<int> l;
	TEST("List append(), iterators")
	{
		for (int i = 0; i < 6; i++)
			l.append(i);
		int x = 0;
		for(List<int>::ConstIterator i = l.constBegin(); i != l.constEnd(); ++i)
			FAILED_IF(*i != x++);
		FAILED_IF(x != 6);
	}
	TEST("List count()...")
	{
		FAILED_IF(l.count() != 6);
		FAILED_IF(l.count(0) != 1);
		FAILED_IF(l.count(-1) != 0);
	}
	TEST("List at()")
	{
		for (int i = 0; i < 6; i++)
			FAILED_IF(l.at(i) != i);
	}
	TEST("List operator==, operator<<")
	{
		FAILED_IF(l == (List<int>() << 0 << 1 << 2 << 3 << 4));
		FAILED_IF(l == (List<int>() << 0 << 1 << 2 << 3 << 4 << 69));
		FAILED_IF(!(l == (List<int>() << 0 << 1 << 2 << 3 << 4 << 5)));
	}
	TEST("List operator!=")
	{
		FAILED_IF(!(l != (List<int>() << 0 << 1 << 2 << 3 << 4)));
		FAILED_IF(!(l != (List<int>() << 0 << 1 << 2 << 3 << 4 << 69)));
		FAILED_IF(l != (List<int>() << 0 << 1 << 2 << 3 << 4 << 5));
	}
	TEST("List List(List&)()")
	{
		List<int> m(l);
		FAILED_IF(m != l);
	}
	TEST("List swap()")
	{
		l.swap(2, 0);
		FAILED_IF(l != List<int>() << 2 << 1 << 0 << 3 << 4 << 5);
	}
	TEST("List move()")
	{
		l.move(0, 4);
		FAILED_IF(l != List<int>() << 1 << 0 << 3 << 4 << 2 << 5);
	}
	TEST("List operator=()")
	{
		l = (List<int>() << 0 << 1 << 2 << 3 << 4 << 5);
		FAILED_IF(l != List<int>() << 0 << 1 << 2 << 3 << 4 << 5);
	}
	TEST("List reversed()")
	{
		l = l.reversed();
		FAILED_IF(l != (List<int>() << 5 << 4 << 3 << 2 << 1 << 0));
	}
	TEST("List reverse()")
	{
		l.reverse();
		FAILED_IF(l != List<int>() << 0 << 1 << 2 << 3 << 4 << 5);
	}
	TEST("List operator+()")
	{
		l = l + l.reversed();
		FAILED_IF(l != l.reversed());
	}
	TEST("List indexOf()")
	{
		FAILED_IF(l.indexOf(4) != 4);
		FAILED_IF(l.indexOf(2, 3) != 9);
	}
	TEST("List lastIndexOf()")
	{
		FAILED_IF(l.lastIndexOf(2) != 9);
		FAILED_IF(l.lastIndexOf(2, 3) != 2);
	}
	TEST("List erase()")
	{
		l.erase(l.begin() + 2);
		FAILED_IF(l.indexOf(2) != 8);
		l.erase(l.begin(), l.end());
		FAILED_IF(l != List<int>());
	}
	TEST("List isEmpty()")
	{
		FAILED_IF(!l.isEmpty());
		l << 5 << 6 << 7 << 8;
		FAILED_IF(l.isEmpty());
	}
	TEST("List mid()")
	{
		FAILED_IF(l.mid(1, 2) != (List<int>() << 6 << 7));
	}
	TEST("List last()")
	{
		FAILED_IF(l.last() != 8);
	}
	TEST("List first()")
	{
		FAILED_IF(l.first() != 5);
	}
	TEST("List takeLast()")
	{
		FAILED_IF(l.takeLast() != 8);
		FAILED_IF(l != (List<int>() << 5 << 6 << 7));
	}
	TEST("List insert()")
	{
		l.insert(1, 69);
		FAILED_IF(l != (List<int>() << 5 << 69 << 6 << 7));
	}
	TEST("List removeOne()")
	{
		l.removeOne(69);
		FAILED_IF(l != (List<int>() << 5 << 6 << 7));
	}
	TEST("List prepend()")
	{
		l.prepend(4);
		FAILED_IF(l != (List<int>() << 4 << 5 << 6 << 7));
	}
	TEST("List takeFirst()")
	{
		FAILED_IF(l.takeFirst() != 4);
		FAILED_IF(l != (List<int>() << 5 << 6 << 7));
	}
	TEST("List clear()")
	{
		l.clear();
		FAILED_IF(!l.isEmpty());
	}
	TEST("List operator<<(), count()")
	{
		List<int> t;
		for (int i = 0; i < 10000; ++i)
		{
			t << i;
			if (!(i % 37))
				for (int j = 0; j <= i; j += 109)
					FAILED_IF(t.count(j) != 1);
		}
	}
	TEST("List Box")
	{
		List<int> x;
		for (int i = 0; i < 16; ++i)
			x << i;
		int count = 2048;
		List<List<int> > t1;
		for (int i = 0; i < count; ++i)
			t1 << x;
		List<List<int> > t2;
		t2 = returnMeBox(t1);
		FAILED_IF(!t1.isEmpty());
		FAILED_IF(t2.count() != count);
		List<List<int> > t3(returnMeBox(t2));
		FAILED_IF(!t2.isEmpty());
		FAILED_IF(t3.count() != count);
	}

	TEST("String default construction")
	{
		String s;
		FAILED_IF(s.length() != 0);
		FAILED_IF(strcmp(s.toCString(), ""));
	}
	TEST("String filled construction")
	{
		String s(3, L'.');
		FAILED_IF(s.length() != 3);
		FAILED_IF(strcmp(s.toCString(), "..."));
	}
	TEST("String char const* construction")
	{
		String s("Hello world!");
		FAILED_IF(s.length() != 12);
		FAILED_IF(strlen(s.toCString()) != 12);
		FAILED_IF(strcmp(s.toCString(), "Hello world!"));
	}
	TEST("String copy construction")
	{
		String* s = new String("Hello world!");
		String t(*s);
		delete s;
		FAILED_IF(t.length() != 12);
		FAILED_IF(strlen(t.toCString()) != 12);
		FAILED_IF(strcmp(t.toCString(), "Hello world!"));
	}
	TEST("String assignment")
	{
		String* s = new String("Hello world!");
		String t;
		t = *s;
		delete s;
		FAILED_IF(t.length() != 12);
		FAILED_IF(strlen(t.toCString()) != 12);
		FAILED_IF(strcmp(t.toCString(), "Hello world!"));
	}
	TEST("String assignment (char const*)")
	{
		String t;
		t = "Hello world!";
		FAILED_IF(t.length() != 12);
		FAILED_IF(strlen(t.toCString()) != 12);
		FAILED_IF(strcmp(t.toCString(), "Hello world!"));
	}
	TEST("String isEmpty()")
	{
		String s;
		String t("X");
		FAILED_IF(!s.isEmpty());
		FAILED_IF(t.isEmpty());
	}
	TEST("String clear()")
	{
		String s("Hello world!");
		s.clear();
		FAILED_IF(!s.isEmpty());
	}
	TEST("String operator==()")
	{
		String s("Hello world!");
		String t;
		FAILED_IF(t == s);
		FAILED_IF(s == t);
		FAILED_IF(t != "");
		t = s;
		FAILED_IF(t != s);
		FAILED_IF(s != t);
		FAILED_IF(s != "Hello world!");
	}
	TEST("String mid(), left(), right()")
	{
		String s("0123456789");
		FAILED_IF(s.mid(2, 6) != "234567");
		FAILED_IF(s.mid(6) != "6789");
		FAILED_IF(s.left(6) != "012345");
		FAILED_IF(s.right(6) != "456789");
	}
	TEST("String chop()")
	{
		String s("0123456789");
		s.chop(7);
		FAILED_IF(s != "012");
	}
	TEST("String trimmed()")
	{
		String s = "  lots\t of\nwhitespace\r\n ";
		s = s.trimmed();
		FAILED_IF(s != "lots\t of\nwhitespace");
	}
	TEST("String simplified()")
	{
		String s = "  lots\t of\nwhitespace\r\n ";
		s = s.simplified();
		FAILED_IF(s != "lots of whitespace");
	}
	TEST("String append()")
	{
		String s = "free";
		String t = "dom";
		s.append(t);
		FAILED_IF(s != "freedom");
		s.append(" is best");
		FAILED_IF(s != "freedom is best");
		s.append(L'!');
		FAILED_IF(s != "freedom is best!");
		s.append('!');
		FAILED_IF(s != "freedom is best!!");
	}
	TEST("String contains()")
	{
		FAILED_IF(String("Freedom").contains("free"));
		FAILED_IF(!String("Freedom").contains("Free"));
		FAILED_IF(String("Freedom").contains(L'f'));
		FAILED_IF(!String("Freedom").contains(L'F'));
	}
	TEST("String count()")
	{
		String s = "Free as in speech, not as in beer!";
		FAILED_IF(s.count("ee") != 3);
		FAILED_IF(s.count(L' ') != 7);
	}
	TEST("String startsWith(), endsWith()")
	{
		String s = "Free as in speech, not as in beer!";
		FAILED_IF(!s.startsWith("Free"));
		FAILED_IF(s.startsWith("beer!"));
		FAILED_IF(!s.endsWith("beer!"));
		FAILED_IF(s.endsWith("Free"));
	}
	TEST("String fill()")
	{
		String s;
		s.fill(L'$', 5);
		FAILED_IF(s != "$$$$$");
		s.fill(L'!', 0);
		FAILED_IF(!s.isEmpty());
	}
	TEST("String indexOf()")
	{
		String s = "sticky question stitch-up";
		FAILED_IF(s.indexOf(s) != 0);
		FAILED_IF(s.indexOf(s, -1) != 0);
		FAILED_IF(s.indexOf(s, 1) != -1);
		FAILED_IF(s.indexOf(s, -2) != 0);
		String t = "sti";
		FAILED_IF(s.indexOf(t) != 0);
		FAILED_IF(s.indexOf(t, 1) != 10);
		FAILED_IF(s.indexOf(t, 10) != 10);
		FAILED_IF(s.indexOf(t, 11) != 16);
		FAILED_IF(s.indexOf(t, 17) != -1);
		FAILED_IF(s.indexOf(s.right(3), -1) != s.length() - 3);
		t.clear();
		FAILED_IF(t.lastIndexOf(s) != -1);
		FAILED_IF(s.lastIndexOf(t) != -1);
		String u = "Charming or tedious?";
		wchar_t v = L'o';
		FAILED_IF(u.indexOf(v, -2) != -1);
		FAILED_IF(u.indexOf(v, -4) != 16);
		FAILED_IF(u.indexOf(v, 9) != 9);
		FAILED_IF(u.indexOf(v, 10) != 16);
		FAILED_IF(u.indexOf(v) != 9);
	}
	TEST("String lastIndexOf()")
	{
		String s = "crazy azimuths";
		String t = "az";
		FAILED_IF(s.lastIndexOf(t) != 6);
		FAILED_IF(s.lastIndexOf(t, 6) != 6);
		FAILED_IF(s.lastIndexOf(t, 5) != 2);
		FAILED_IF(s.lastIndexOf(t, 1) != -1);
		t.clear();
		FAILED_IF(t.lastIndexOf(s) != -1);
		FAILED_IF(s.lastIndexOf(t) != -1);
		t = "z";
		FAILED_IF(s.lastIndexOf(t, 14) != 7);
		FAILED_IF(s.lastIndexOf(t, -13) != -1);
		FAILED_IF(s.lastIndexOf(t, -14) != -1);
		FAILED_IF(s.lastIndexOf(t, -15) != -1);
		FAILED_IF(s.lastIndexOf(L'z', 14) != 7);
		FAILED_IF(s.lastIndexOf(L'z', -13) != -1);
		FAILED_IF(s.lastIndexOf(L'z', -14) != -1);
		FAILED_IF(s.lastIndexOf(L'z', -15) != -1);
	}
	TEST("String split()")
	{
		String s = ",x,y,z,";
		FAILED_IF(s.split(L',') != StringList() << "" << "x" << "y" << "z" << "");
		FAILED_IF(s.mid(1, 5).split(L',') != StringList() << "x" << "y" << "z");
		FAILED_IF(s.split(",") != StringList() << "" << "x" << "y" << "z" << "");
		FAILED_IF(s.mid(1, 5).split(",") != StringList() << "x" << "y" << "z");
		String t = "M...A...R...T..T...A";
		FAILED_IF(t.split("...") != StringList() << "M" << "A" << "R" << "T..T" << "A");
		FAILED_IF(t.mid(1, 18).split("..") != StringList() << "" << ".A" << ".R" << ".T" << "T" << ".");
	}
	TEST("String indexOfNth(wchar_t)")
	{
		String s = "";
		FAILED_IF(s.indexOfNth(L'.', 0) != -1);
		FAILED_IF(s.indexOfNth(L'.', 1) != 0);
		FAILED_IF(s.indexOfNth(L'.', 2) != -1);
		s = ".";
		FAILED_IF(s.indexOfNth(L'.', 0) != -1);
		FAILED_IF(s.indexOfNth(L'.', 1) != 0);
		FAILED_IF(s.indexOfNth(L'.', 2) != 1);
		FAILED_IF(s.indexOfNth(L'.', 3) != -1);
		s = "-";
		FAILED_IF(s.indexOfNth(L'.', 0) != -1);
		FAILED_IF(s.indexOfNth(L'.', 1) != 1);
		FAILED_IF(s.indexOfNth(L'.', 2) != -1);
		s = ".x.yy..";
		FAILED_IF(s.indexOfNth(L'.', 0) != -1);
		FAILED_IF(s.indexOfNth(L'.', 1) != 0);
		FAILED_IF(s.indexOfNth(L'.', 2) != 2);
		FAILED_IF(s.indexOfNth(L'.', 3) != 5);
		FAILED_IF(s.indexOfNth(L'.', 4) != 6);
		FAILED_IF(s.indexOfNth(L'.', 5) != 7);
		FAILED_IF(s.indexOfNth(L'.', 6) != -1);
	}
	TEST("String indexOfNth(String)")
	{
		String s = "";
		String t = ".";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != -1);
		s = ".";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != 1);
		FAILED_IF(s.indexOfNth(t, 3) != -1);
		s = "-";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 1);
		FAILED_IF(s.indexOfNth(t, 2) != -1);
		s = ".x.yy..";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != 2);
		FAILED_IF(s.indexOfNth(t, 3) != 5);
		FAILED_IF(s.indexOfNth(t, 4) != 6);
		FAILED_IF(s.indexOfNth(t, 5) != 7);
		FAILED_IF(s.indexOfNth(t, 6) != -1);
		t = "..";
		s = "";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != -1);
		s = "..";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != 2);
		FAILED_IF(s.indexOfNth(t, 3) != -1);
		s = "-";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 1);
		FAILED_IF(s.indexOfNth(t, 2) != -1);
		s = "..x..yy....";
		FAILED_IF(s.indexOfNth(t, 0) != -1);
		FAILED_IF(s.indexOfNth(t, 1) != 0);
		FAILED_IF(s.indexOfNth(t, 2) != 3);
		FAILED_IF(s.indexOfNth(t, 3) != 7);
		FAILED_IF(s.indexOfNth(t, 4) != 9);
		FAILED_IF(s.indexOfNth(t, 5) != 11);
		FAILED_IF(s.indexOfNth(t, 6) != -1);
	}
	TEST("String lastIndexOfNth(wchar_t)")
	{
		String s = "";
		FAILED_IF(s.lastIndexOfNth(L'.', 0) != 0);
		FAILED_IF(s.lastIndexOfNth(L'.', 1) != -1);
		s = ".";
		FAILED_IF(s.lastIndexOfNth(L'.', 0) != 1);
		FAILED_IF(s.lastIndexOfNth(L'.', 1) != 0);
		FAILED_IF(s.lastIndexOfNth(L'.', 2) != -1);
		s = "-";
		FAILED_IF(s.lastIndexOfNth(L'.', 0) != 1);
		FAILED_IF(s.lastIndexOfNth(L'.', 1) != -1);
		s = ".x.yy..";
		FAILED_IF(s.lastIndexOfNth(L'.', 0) != 7);
		FAILED_IF(s.lastIndexOfNth(L'.', 1) != 6);
		FAILED_IF(s.lastIndexOfNth(L'.', 2) != 5);
		FAILED_IF(s.lastIndexOfNth(L'.', 3) != 2);
		FAILED_IF(s.lastIndexOfNth(L'.', 4) != 0);
		FAILED_IF(s.lastIndexOfNth(L'.', 5) != -1);
	}
	TEST("String lastIndexOfNth(String)")
	{
		String s = "";
		String t = ".";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 1) != -1);
		s = ".";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 1);
		FAILED_IF(s.lastIndexOfNth(t, 1) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 2) != -1);
		s = "-";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 1);
		FAILED_IF(s.lastIndexOfNth(t, 1) != -1);
		s = ".x.yy..";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 7);
		FAILED_IF(s.lastIndexOfNth(t, 1) != 6);
		FAILED_IF(s.lastIndexOfNth(t, 2) != 5);
		FAILED_IF(s.lastIndexOfNth(t, 3) != 2);
		FAILED_IF(s.lastIndexOfNth(t, 4) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 5) != -1);
		t = "..";
		s = "";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 1) != -1);
		s = "..";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 2);
		FAILED_IF(s.lastIndexOfNth(t, 1) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 2) != -1);
		s = "-";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 1);
		FAILED_IF(s.lastIndexOfNth(t, 1) != -1);
		s = "..x..yy....";
		FAILED_IF(s.lastIndexOfNth(t, 0) != 11);
		FAILED_IF(s.lastIndexOfNth(t, 1) != 9);
		FAILED_IF(s.lastIndexOfNth(t, 2) != 7);
		FAILED_IF(s.lastIndexOfNth(t, 3) != 3);
		FAILED_IF(s.lastIndexOfNth(t, 4) != 0);
		FAILED_IF(s.lastIndexOfNth(t, 5) != -1);
	}
	TEST("String section()")
	{
		String s = "apples,oranges,pears,grapes";
		FAILED_IF(s.section(L',', 0, 0) != "apples");
		FAILED_IF(s.section(L',', 1, 3) != "oranges,pears,grapes");
		FAILED_IF(s.section(L',', 2, -2) != "pears");
		FAILED_IF(s.section(L',', -4, -3) != "apples,oranges");
		FAILED_IF(s.section(L',', -1, -1) != "grapes");
		FAILED_IF(s.section(L',', 3, 3) != "grapes");
		String t = ",";
		FAILED_IF(s.section(t, 0, 0) != "apples");
		FAILED_IF(s.section(t, 1, 3) != "oranges,pears,grapes");
		FAILED_IF(s.section(t, 2, -2) != "pears");
		FAILED_IF(s.section(t, -4, -3) != "apples,oranges");
		FAILED_IF(s.section(t, -1, -1) != "grapes");
		FAILED_IF(s.section(t, 3, 3) != "grapes");
		s = "X....Y";
		t = "..";
		FAILED_IF(s.section(t, 0, 0) != "X");
		FAILED_IF(s.section(t, 1, 1) != "");
		FAILED_IF(s.section(t, 2, 2) != "Y");
		FAILED_IF(s.section(t, 0, 1) != "X..");
		FAILED_IF(s.section(t, 1, 2) != "..Y");
		FAILED_IF(s.section(t, -3, -1) != "X....Y");
	}
	TEST("String toUpper(), toLower()")
	{
		FAILED_IF(String("Hello world!").toUpper() != String("HELLO WORLD!"));
		FAILED_IF(String("EbenEEZER GOOD!").toLower() != String("ebeneezer good!"));
	}
	TEST("String replace()")
	{
		String str = "rock and roll";
		str.replace(5, 3, L'&');
		FAILED_IF(str != "rock & roll");
		str.replace(5, 1, "and");
		FAILED_IF(str != "rock and roll");
		str.replace("and", "&&&");
		FAILED_IF(str != "rock &&& roll");
		str.replace(L'&', L'+');
		FAILED_IF(str != "rock +++ roll");
		str.replace(L'+', "'");
		FAILED_IF(str != "rock ''' roll");
	}
	TEST("String prepend(), remove(), insert()")
	{
		String str = "XaXaaaX";
		str.remove(L'X');
		FAILED_IF(str != "aaaa");
		str.prepend("ock");
		FAILED_IF(str != "ockaaaa");
		str.prepend(L'R');
		FAILED_IF(str != "Rockaaaa");
		str.insert(6, " and rol");
		FAILED_IF(str != "Rockaa and rolaa");
		str.remove("aa");
		FAILED_IF(str != "Rock and rol");
		str.insert(11, L'l');
		FAILED_IF(str != "Rock and roll");
	}
	TEST("String toInt(), toUint(), toDouble(), toFloat()")
	{
		bool ok;
		FAILED_IF(String("-1").toInt() != -1);
		FAILED_IF(String("0x7d4a").toInt(&ok, 16) != 0x7d4a);
		FAILED_IF(!ok);
		String().toInt(&ok);
		FAILED_IF(ok);
		FAILED_IF(String("69").toUint() != 69u);
		FAILED_IF(String("101010").toUint(&ok, 2) != 42u);
		FAILED_IF(!ok);
		String().toUint(&ok);
		FAILED_IF(ok);
		FAILED_IF(String("56.2").toDouble() != 56.2);
		FAILED_IF(String("0.5e-9").toDouble(&ok) != 0.5e-9);
		FAILED_IF(!ok);
		String().toDouble(&ok);
		FAILED_IF(ok);
		FAILED_IF(String("69.69").toFloat() != 69.69f);
		FAILED_IF(String("-96").toFloat(&ok) != -96.f);
		FAILED_IF(!ok);
		String().toFloat(&ok);
		FAILED_IF(ok);
	}
	TEST("String operator<, >, <=, >=")
	{
		String n;
		String ab("ab");
		String aa("aa");
		String zz("zz");
		FAILED_IF(n < n);
		FAILED_IF(n > n);
		FAILED_IF(!(n <= n));
		FAILED_IF(!(n >= n));
		FAILED_IF(!(n < ab));
		FAILED_IF(n > ab);
		FAILED_IF(!(n <= ab));
		FAILED_IF(n >= ab);
		FAILED_IF(ab < aa);
		FAILED_IF(ab > zz);
		FAILED_IF(!(ab < "ac"));
		FAILED_IF(zz > "zz");
		FAILED_IF(zz < "zz");
		FAILED_IF(zz >= "zzz");
		FAILED_IF(zz <= "zy");
		FAILED_IF("zz" > zz);
		FAILED_IF("zz" < zz);
		FAILED_IF("zy" >= zz);
		FAILED_IF("zzz" <= zz);
	}
	TEST("String operator+")
	{
		String a("foo");
		String b("bar");
		FAILED_IF(a + ' ' != "foo ");
		FAILED_IF(' ' + b != " bar");
		FAILED_IF(a + b != "foobar");
		FAILED_IF("foo " + b != "foo bar");
		FAILED_IF(a + " bar" != "foo bar");
	}
	TEST("String number()")
	{
		FAILED_IF(String::number(0.696969) != "0.696969");
		FAILED_IF(String::number(-69) != "-69");
		FAILED_IF(String::number(42u) != "42");
		FAILED_IF(String::number(42u, 16) != "2a");
		FAILED_IF(String::number(42u, 8) != "52");
	}
	TEST("String arg()")
	{
		FAILED_IF(String("%2%3%1%4").arg("world").arg("Hello").arg(" ").arg("!") != "Hello world!");
		FAILED_IF(String("%4 %3 %1%2").arg(69u, 0, 10).arg('!').arg(L'a', -6, L'.').arg("Make mine") != "Make mine a..... 69!");
		FAILED_IF(String("%4 %3 %1%2").arg(69).arg(L'!').arg('a').arg("Make mine") != "Make mine a 69!");
		String s = String("%1 / %2 = %3").arg(69.f, 0, 'g', 4).arg(999999u, 0, 16).arg(69.f/999999, 0, 'e', 1);
		FAILED_IF(s != "69 / f423f = 6.9e-05" && s != "69 / f423f = 6.9e-005");
	}
	TEST("String hashOf()")
	{
		FAILED_IF(hashOf("BBC") == hashOf("CBB"));
		FAILED_IF(hashOf("CGHQ") == hashOf("GCHQ"));
		FAILED_IF(hashOf("XYZ") != hashOf("XYZ"));
	}
	TEST("String Box")
	{
		int count = 65536;
		String s1(count, 'x');
		String s2;
		s2 = returnMeBox(s1);
		FAILED_IF(s2.length() != count);
		FAILED_IF(!s1.isEmpty());
		String s3(returnMeBox(s2));
		FAILED_IF(s3.length() != count);
		FAILED_IF(!s2.isEmpty());
	}
	
	TEST("Hash construction")
	{
		Hash<int, int> h;
		FAILED_IF(!h.isEmpty());
	}
	TEST("HashOf int")
	{
		char const* x = "Hello world!";
		uint i = (uint)x;
		FAILED_IF(hashOf(i) != i);
		int const j = (int)x;
		FAILED_IF((int)hashOf(j) != j);
	}
	TEST("Hash insert()")
	{
		Hash<int, int> h;
		h.setAutoResize(false);
		for (int i = 0; i < 1024; ++i)
			h.insert(i, i + 1);
		for (int i = 0; i < 1024; ++i)
			FAILED_IF(h.value(i) != i + 1);
	}
	TEST("Hash isEmpty()")
	{
		Hash<String, int> h;
		FAILED_IF(!h.isEmpty());
		h.insert("0", 0);
		FAILED_IF(h.isEmpty());
	}
	TEST("Hash resize(), Iterator")
	{
		MultiHash<int, int> h;
		int count = 16384;
		int b = max(5u, floorLog2(count) + 1);
		int resizes = 15;
		for (int i = 0; i < count; ++i)
			h.insert(i % (count / 8), i);
		for (int i = 0; i < resizes * (b - 5) * 2; ++i)
			h.resize(1 << (abs(b - 6 - i % ((b - 5) * 2)) + 5));
		for (int i = count / 8 - 1; i >= 0; --i)
		{
			int k = count - count / 8 + i;
			for (MultiHash<int, int>::Iterator j = h.find(i); j != h.end() && j.key() == i; ++j, k -= count / 8)
				FAILED_IF(j.value() != k);
			FAILED_IF(k >= 0);
		}
	}
	TEST("Hash values()")
	{
		Hash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h.insert(i % 10, i);
		FAILED_IF(h.count() != 10);
		for (int i = 0; i < 10; ++i)
			FAILED_IF(h.values(i) != List<int>() << 90 + i);
	}
	TEST("Hash insertMulti()")
	{
		MultiHash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h.insert(i % 10, i);
		FAILED_IF(h.count() != 100);
		for (int i = 0; i < 10; ++i)
			FAILED_IF(h.values(i) != List<int>() << (90 + i) << (80 + i) << (70 + i) << (60 + i) << (50 + i) << (40 + i) << (30 + i) << (20 + i) << (10 + i) << i);
	}
	TEST("Hash keys(), uniqueKeys()")
	{
		MultiHash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h.insert(i % 10, i);
		List<int> k = h.keys();
		List<int> u = h.uniqueKeys();
		FAILED_IF(k.count() != 100);
		FAILED_IF(u.count() != 10);
		for (int i = 0; i < 10; ++i)
		{
			FAILED_IF(k.count(i) != 10);
			FAILED_IF(u.count(i) != 1);
		}
	}
	TEST("Hash remove(), removeOne(), take()")
	{
		MultiHash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h.insert(i % 10, i);
		for (int i = 80; i >= 0; i -= 10)
		{
			h.removeOne(0);
			FAILED_IF(h[0] != i);
		}
		FAILED_IF(h.take(0) != 0);
		for (int i = 1; i < 10; ++i)
			h.remove(i);
		FAILED_IF(!h.isEmpty());
	}
	TEST("Hash keys(T)")
	{
		Hash<int, int> h;
		for (int i = 0; i < 100; ++i)
			h[i] = i % 10;
		for (int i = 0; i < 10; ++i)
		{
			List<int> l = h.keys(i);
			FAILED_IF(l.count() != 10);
			for (int j = 0; j < 10; ++j)
				FAILED_IF(!l.contains(j * 10 + i));
		}
	}
	{
	Hash<int, int> h;
	h[0] = 69;
	h[1] = 42;
	h[32] = 1;
	h[64] = 2;
	h[96] = 3;
	{
	int ks[] = {96, 64, 32, 0, 1};
	int vs[] = {3, 2, 1, 69, 42};
	TEST("Hash constBegin(), constEnd(), ConstIterator::operator++, ConstIterator::operator--")
	{
		int j = 0;
		for (Hash<int, int>::ConstIterator i = h.constBegin(); i != h.constEnd(); ++i, ++j)
		{
			FAILED_IF(i.key() != ks[j]);
			FAILED_IF(i.value() != vs[j]);
		}
		for (Hash<int, int>::ConstIterator i = h.constEnd(); i != h.constBegin();)
		{
			--i;
			--j;
			FAILED_IF(i.key() != ks[j]);
			FAILED_IF(i.value() != vs[j]);
		}
	}
	TEST("Hash begin(), end(), Iterator::operator++, Iterator::operator--")
	{
		int j = 0;
		for (Hash<int, int>::Iterator i = h.begin(); i != h.end(); ++i, ++j)
		{
			FAILED_IF(i.key() != ks[j]);
			FAILED_IF(i.value() != vs[j]);
		}
		for (Hash<int, int>::Iterator i = h.end(); i != h.begin();)
		{
			--i;
			--j;
			FAILED_IF(i.key() != ks[j]);
			FAILED_IF(i.value() != vs[j]);
		}
	}
	}
	TEST("Hash Iterator erase()")
	{
		FAILED_IF(*h.erase(h.find(0)) != 42);		// SbNa, = 42
		FAILED_IF(*h.erase(h.find(64)) != 1);		// SbSa, = 1
		FAILED_IF(*h.erase(h.find(96)) != 1);		// NbSa, = 1
		FAILED_IF(*h.erase(h.find(32)) != 42);		// NbNa, = 42
		FAILED_IF(h.erase(h.find(1)) != h.end());	// NbNa, end().
	}
	}
	TEST("Hash operator==(), operator!=()")
	{
		MultiHash<int, int> h1;
		MultiHash<int, int> h2;
		h1.insert(0, 0);
		h1.insert(1, 1);
		h1.insert(32, 2);
		h1.insert(0, 3);
		h2.insert(0, 4);
		h2.insert(1, 1);
		h2.insert(32, 2);
		h2.insert(0, 3);
		FAILED_IF(h1 == h2);
		FAILED_IF(!(h1 != h2));
		h2.clear();
		h2.insert(0, 0);
		h2.insert(1, 1);
		h2.insert(32, 2);
		h2.insert(0, 3);
		FAILED_IF(h1 != h2);
		FAILED_IF(!(h1 == h2));
	}
	TEST("Hash unite(), values(), operator=(), Hash(Hash)")
	{
		MultiHash<int, int> h1;
		MultiHash<int, int> h2;
		h1.insert(0, 0);
		h1.insert(0, 1);
		h1.insert(32, 0);
		h1.insert(32, 1);
		h2.insert(0, 2);
		h2.insert(0, 3);
		h2.insert(64, 0);
		h2.insert(64, 1);
		h1.unite(h2);
		FAILED_IF(h1.values(0) != List<int>() << 3 << 2 << 1 << 0);
		FAILED_IF(h1.values(32) != List<int>() << 1 << 0);
		FAILED_IF(h1.values(64) != List<int>() << 1 << 0);
		h2 = h1;
		FAILED_IF(h2 != h1);
		Hash<int, int, 16> h3(h1);
		FAILED_IF(h3 != h1);
		Hash<int, int, 16> h4;
		h4 = h1;
		FAILED_IF(h4 != h1);
	}
	TEST("Hash Box")
	{
		int count = 16384;
		MultiHash<int, int> h1;
		for (int i = 0; i < count; ++i)
			h1.insert(i * 8 % (count / 128), i + 1);
		MultiHash<int, int> h2;
		h2 = returnMeBox(h1);
		FAILED_IF(!h1.isEmpty());
		FAILED_IF(h2.count() != count);
		MultiHash<int, int> h3(returnMeBox(h2));
		FAILED_IF(!h2.isEmpty());
		FAILED_IF(h3.count() != count);
	}
	
	s_testing = false;
	qInformation() << "PASSED :-)";
	return failed;
}































