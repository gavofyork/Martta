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

#include "OperatorLabel.h"
#include "RootEntity.h"
#include "TextLabel.h"
#include "Pointer.h"
#include "Reference.h"
#include "Const.h"
#include "FundamentalOperator.h"

namespace Martta
{

MARTTA_OBJECT_CPP(FundamentalOperator);

QMultiHash<Operator::Symbol, FundamentalOperator*> FundamentalOperator::s_catalogue;
	
FundamentalOperator::FundamentalOperator(Operator _id, Type const& _nr, Types const& _nargs, RootEntity* _root)
{
	int _left = _nargs[0]->isType<Pointer>() ? Ptr : _nargs[0]->ignore<Const>()->ignore<Reference>()->asType<SimpleType>()->id();
	int _right = (_nargs.size() > 1) ? _nargs[1]->isType<Pointer>() ? Ptr : _nargs[1]->isType<SimpleType>() ? _nargs[1]->asType<SimpleType>()->id() : _left : 0;
	m_key = (_nargs.size() > 1) ? _id.key(_left, _right) : _id.key(_left);
	s_catalogue.insertMulti(_id.symbol(), this);
	
	m_rootEntity = _root;
	back().place(new OperatorLabel(_id));
	back().place(new Compound);
	_nr.insertSilentCopy(back());
	M_ASSERT(body());
	
	foreach (Type q, _nargs)
	{
		Variable* v = new Variable;
		v->back().place(new TextLabel);
		back().place(v);
		q.insertSilentCopy(v->back());
	}
	M_ASSERT(isComplete());
	rootEntity()->registerDeclaration(this);
}

FundamentalOperator::~FundamentalOperator()
{
	rootEntity()->unregisterDeclaration(this);
	s_catalogue.remove(id().symbol(), this);
}

void FundamentalOperator::initialiseClass()
{
	QList<int> integral;
	integral << Char << (Short|Int) << Int << (Long|Int) << (Longlong|Int) << (Unsigned|Char) << (Unsigned|Short|Int) << (Unsigned|Int) << (Unsigned|Long|Int) << (Unsigned|Longlong|Int);
	QList<int> scalar;
	scalar << integral << Float << Double << (Long|Double);
	QList<int> numeric;
	numeric << scalar << (Complex|Float) << (Complex|Double) << (Complex|Long|Double);
	
	RootEntity* root = RootEntity::get();
	
	// integral types
	foreach (int d, integral)
	{
		// unit inc/decrement operators.
		foreach (QString s, QString("++,--").split(","))
		{	new FundamentalOperator(Operator(s, Operator::UnaryPrefix), Type(d).topWith(Reference()), Type(d).topWith(Reference()), root);
			new FundamentalOperator(Operator(s, Operator::UnaryPostfix), Type(d).topWith(Reference()), Type(d).topWith(Reference()), root);
		}
		// modulo/logical operators
		foreach (QString s, QString("%,&,|,^").split(","))
			new FundamentalOperator(Operator(s), Type(d), (Type(d), Type(d)), root);
		foreach (QString s, QString(">>,<<").split(","))
			new FundamentalOperator(Operator(s), Type(d), (Type(d), Type(Int)), root);
		foreach (QString s, QString("%=,&=,|=,^=").split(","))
			new FundamentalOperator(Operator(s), Type(d).topWith(Reference()), (Type(d).topWith(Reference()), Type(d)), root);
		foreach (QString s, QString(">>=,<<=").split(","))
			new FundamentalOperator(Operator(s), Type(d).topWith(Reference()), (Type(d).topWith(Reference()), Type(Int)), root);
		foreach (QString s, QString("~").split(","))
			new FundamentalOperator(Operator(s), Type(d), Type(d), root);
	}

	// all scalar types
	foreach (int d, scalar)
	{
		// comparison
		foreach (QString s, QString("<,>,<=,>=").split(","))
			new FundamentalOperator(Operator(s), Type(Bool), (Type(d), Type(d)), root);
		// simple arithmetic
		foreach (QString s, QString("+,-,*,/").split(","))
			new FundamentalOperator(Operator(s, Operator::Binary), Type(d), (Type(d), Type(d)), root);
		// increment
		foreach (QString s, QString("+=,-=,*=,/=").split(","))
			new FundamentalOperator(Operator(s), Type(d).topWith(Reference()), (Type(d).topWith(Reference()), Type(d)), root);
		// signing
		foreach (QString s, QString("+,-").split(","))
			new FundamentalOperator(Operator(s, Operator::UnaryPrefix), Type(d), Type(d), root);
	}

	// boolean
	foreach (QString s, QString("||,&&").split(","))
		new FundamentalOperator(Operator(s), Type(Bool), (Type(Bool), Type(Bool)), root);
	foreach (QString s, QString("!").split(","))
		new FundamentalOperator(Operator(s), Type(Bool), Type(Bool), root);

	// all types
	foreach (int d, QList<int>(numeric) << Bool << Wchar)
	{
		// (in)equality
		foreach (QString s, QString("==,!=").split(","))
			new FundamentalOperator(Operator(s), Type(Bool), (Type(d), Type(d)), root);
	}

	// pointer types (we use Ptr for them in the lookup)
	foreach (QString s, QString("++,--").split(","))
	{
		new FundamentalOperator(Operator(s, Operator::UnaryPrefix), Type(), Type(Void).topWith(Const()).topWith(Pointer()).topWith(Reference()), root);
		new FundamentalOperator(Operator(s, Operator::UnaryPostfix), Type(), Type(Void).topWith(Const()).topWith(Pointer()).topWith(Reference()), root);
	}
	foreach (QString s, QString("<,>,<=,>=,==,!=").split(","))
		new FundamentalOperator(Operator(s), Type(Bool), (Type(Void).topWith(Const()).topWith(Pointer()), Type().topWith(Pointer())), root);
	foreach (QString s, QString("+,-").split(","))
		new FundamentalOperator(Operator(s, Operator::Binary), Type().topWith(Pointer()), (Type(Void).topWith(Const()).topWith(Pointer()), Type(Int)), root);
	foreach (QString s, QString("+=,-=").split(","))
		new FundamentalOperator(Operator(s), Type().topWith(Pointer()).topWith(Reference()), (Type(Void).topWith(Const()).topWith(Pointer()).topWith(Reference()), Type(Int)), root);
}

void FundamentalOperator::finaliseClass()
{
	foreach (FundamentalOperator* i, s_catalogue.values())
		delete i;
}

}
