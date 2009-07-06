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

#include "TestHelper.h"
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

int typeTests()
{
	int failed = 0;
	
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
		Root r;
		Class* X = new Class;
		X->prepareChildren();
		r.back().place(X);
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
		Root r;
		Enumeration* X = new Enumeration;
		X->prepareChildren();
		X->setUnnamed();
		r.back().place(X);
		EnumValue* Xv = new EnumValue;
		Xv->prepareChildren();
		X->back().place(Xv);
		Enumeration* Y = new Enumeration;
		Y->prepareChildren();
		Y->setNamed();
		Y->childAs<TextLabel>(Identifiable::Identity)->setText("Y");
		r.back().place(Y);
		EnumValue* Yv = new EnumValue;
		Yv->prepareChildren();
		Y->back().place(Yv);
		Type x = Xv->type();
		Type y = Yv->type();
		Type i = Type(Int);
		mDebug() << x->code();
		mDebug() << y->code();
		mDebug() << i->code();
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
		Root r;
		Class* X = new Class;
		X->prepareChildren();
		r.back().place(X);
		Type x = Type(X);
	}
	{
#define CAST_TEST(F, T, R) TEST("Reference casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		Root r;
		Class* X = new Class;
		X->prepareChildren();
		r.back().place(X);
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
#undef CAST_TEST
	}
	{
#define CAST_TEST(F, T, R) TEST("Memberify casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		Root r;
		Class* B = new Class;
		Class* D = new Class;
		B->prepareChildren();
		B->childAs<TextLabel>(Identifiable::Identity)->setText("B");
		D->prepareChildren();
		D->childAs<TextLabel>(Identifiable::Identity)->setText("D");
		r.back().place(B);
		r.back().place(D);
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
#undef CAST_TEST
	}
	{
#define CAST_TEST(F, T, R) TEST("String casting... " #F "->" #T) TEST_THIS_CAST(F, T, R)
		{
		Type s = Type(StringType());
		Type scr = Type(StringType()).topWith(Const()).topWith(Reference());
		Type ccs = Type(Martta::Char).topWith(Const()).topWith(Pointer());
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
	
	return failed;
}
