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

#include <QtXml>

#include "Type.h"
#include "SimpleOperator.h"
#include "Const.h"
#include "Pointer.h"
#include "Reference.h"
#include "CodeScene.h"
#include "CompletionDelegate.h"
#include "StringType.h"
#include "SimpleType.h"

namespace Martta
{

const int s_simpleIds[] = { Char, Int, Short|Int, Long|Int, Longlong|Int, Unsigned|Char, Unsigned|Int, Unsigned|Short|Int, Unsigned|Long|Int, Unsigned|Longlong|Int, Float, Double, Long|Double, Complex|Float, Complex|Double, Complex|Long|Double, Bool, Wchar, Void };
const int s_simpleIdsCount = sizeof(s_simpleIds) / sizeof(s_simpleIds[0]);

MARTTA_OBJECT_CPP(SimpleType);

List<SimpleOperator*> SimpleType::s_nonMembers;

bool SimpleType::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text().length() == 1 && _e->text()[0].isLower())
	{
		_e->reinterpretLater();
		SimpleType* s = new SimpleType;
		_p.place(s);
		s->setEditing(_e->codeScene());
	}
	else
		return false;
	return true;
}

bool SimpleType::keyPressed(KeyEvent const* _e)
{
	if (_e->text().length() == 1 && (_e->text()[0].isLower() || _e->text()[0] == L':'))
	{
		_e->reinterpretLater();
		setEditing(_e->codeScene());
	}
	else
		return Super::keyPressed(_e);
	return true;
}

String SimpleType::code(String const& _middle) const
{
	if (m_id == -1) return String();
	if (m_id == Wchar) return "wchar_t";
	return String((m_id & Unsigned) ? (m_id & Float || m_id & Double) ? "complex " : "unsigned " : "") +
			(((m_id & Longlong) == Longlong) ? "long long " : (m_id & Short) ? "short " : (m_id & Long) ? "long " : "") +
			((m_id & Float) ? "float" : (m_id & Bool) ? "bool" : (m_id & Double) ? "double" : (m_id & Char) ? "char" : (m_id & Int) ? "int" : "void") + _middle;
}

Types SimpleType::assignableTypes() const
{
	return Type(*this);
}

bool SimpleType::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	if (_t->isKind<SimpleType>())
	{
		int tId = _t->asKind<SimpleType>()->m_id;
		if (tId != -1)
		{
			if (tId == m_id)
				return true;
			if (m_id != Void && (
				isFairlyConvertibleAtMost(_c) && (
					m_id & Int && tId & (Double | Float) && !(tId & Complex)								// int -> scalar double/float
					|| m_id & (Double | Float) && !(m_id & Complex) && tId & Int							// scalar double/float -> int
					|| m_id & (Double | Float) && tId & (Double | Float) && ((m_id & Complex) == (tId & Complex))	// double/float -> double/float (same scalar/complex)
				)
				||
				isBasicallyConvertibleAtMost(_c) && (
					m_id & (Int | Char) && tId & (Int | Bool)												// int/char -> int/bool
				)
				))
				return true;
		}
	}
	return Super::defineSimilarityTo(_t, _c);
}

bool SimpleType::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	return m_id == Void && !_f->isKind<ModifyingType>() ||
		Super::defineSimilarityFrom(_f, _c);
}

String SimpleType::defineLayout(ViewKeys const&) const
{
	return typeLayout() + "^;'" + ((id() == -1) ? String("[]") : name(id())) + "'";
}

void SimpleType::importDom(QDomElement const& _element)
{
	Super::importDom(_element);
	setId(_element.attribute("id").toInt());
}

void SimpleType::exportDom(QDomElement& _element) const
{
	Super::exportDom(_element);
	_element.setAttribute("id", id());
}

template<>
class NameTrait<int>
{
public:
	enum { StringId = 0xff00 };
	static String name(int _val) { return (_val == StringId) ? "string" : SimpleType::name(_val); }
};

List<int> SimpleType::possibilities()
{
	List<int> ret;
	for (int i = 0; i < s_simpleIdsCount; i++)
		ret << s_simpleIds[i];
	ret << NameTrait<int>::StringId;
	return ret;
}

String SimpleType::defineEditLayout(ViewKeys const&, int) const
{
	return "^;c;s" + idColour().name() + ";fb;%1";
}

EditDelegateFace* SimpleType::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<SimpleType, int>(this, _s);
}

void SimpleType::committed()
{
	// Switch to string if necessary...
	if (m_id == NameTrait<int>::StringId)
	{
		replace(new StringType);
	}
}

void SimpleType::initialiseClass()
{
	List<int> integral;
	integral << Char << (Short|Int) << Int << (Long|Int) << (Longlong|Int) << (Unsigned|Char) << (Unsigned|Short|Int) << (Unsigned|Int) << (Unsigned|Long|Int) << (Unsigned|Longlong|Int);
	List<int> scalar;
	scalar << integral << Float << Double << (Long|Double);
	List<int> numeric;
	numeric << scalar << (Complex|Float) << (Complex|Double) << (Complex|Long|Double);
	
	// integral types
	foreach (int d, integral)
	{
		// unit inc/decrement operators.
		foreach (String s, String("++,--").split(","))
		{	SimpleOperator::create<SimpleType>(Operator(s, Operator::UnaryPrefix), Type(d)/*.topWith(Reference())*/, Type(d).topWith(Reference()));
			SimpleOperator::create<SimpleType>(Operator(s, Operator::UnaryPostfix), Type(d)/*.topWith(Reference())*/, Type(d).topWith(Reference()));
		}
		// modulo/logical operators
		foreach (String s, String("%,&,|,^").split(","))
			SimpleOperator::create<SimpleType>(Operator(s), Type(d), (Type(d), Type(d)));
		foreach (String s, String(">>,<<").split(","))
			SimpleOperator::create<SimpleType>(Operator(s), Type(d), (Type(d), Type(Int)));
		foreach (String s, String("%=,&=,|=,^=").split(","))
			SimpleOperator::create<SimpleType>(Operator(s), Type(d).topWith(Reference()), (Type(d).topWith(Reference()), Type(d)));
		foreach (String s, String(">>=,<<=").split(","))
			SimpleOperator::create<SimpleType>(Operator(s), Type(d).topWith(Reference()), (Type(d).topWith(Reference()), Type(Int)));
		foreach (String s, String("~").split(","))
			SimpleOperator::create<SimpleType>(Operator(s), Type(d), Type(d));
	}

	// all scalar types
	foreach (int d, scalar)
	{
		// comparison
		foreach (String s, String("<,>,<=,>=").split(","))
			SimpleOperator::create<SimpleType>(Operator(s), Type(Bool), (Type(d), Type(d)));
		// simple arithmetic
		foreach (String s, String("+,-,*,/").split(","))
			SimpleOperator::create<SimpleType>(Operator(s, Operator::Binary), Type(d), (Type(d), Type(d)));
		// increment
		foreach (String s, String("+=,-=,*=,/=").split(","))
			SimpleOperator::create<SimpleType>(Operator(s), Type(d).topWith(Reference()), (Type(d).topWith(Reference()), Type(d)));
		// signing
		foreach (String s, String("+,-").split(","))
			SimpleOperator::create<SimpleType>(Operator(s, Operator::UnaryPrefix), Type(d), Type(d));
	}

	// boolean
	foreach (String s, String("||,&&").split(","))
		SimpleOperator::create<SimpleType>(Operator(s), Type(Bool), (Type(Bool), Type(Bool)));
	foreach (String s, String("!").split(","))
		SimpleOperator::create<SimpleType>(Operator(s), Type(Bool), Type(Bool));

	// all types
	foreach (int d, List<int>(numeric) << Bool << Wchar)
	{
		// (in)equality
		foreach (String s, String("==,!=").split(","))
			SimpleOperator::create<SimpleType>(Operator(s), Type(Bool), (Type(d), Type(d)));
	}

	// pointer types (we use Ptr for them in the lookup)
	foreach (String s, String("++,--").split(","))
	{
		SimpleOperator::create<SimpleType>(Operator(s, Operator::UnaryPrefix), Type().topWith(Pointer()), Type(Void).topWith(Const()).topWith(Pointer()).topWith(Reference()));
		SimpleOperator::create<SimpleType>(Operator(s, Operator::UnaryPostfix), Type().topWith(Pointer()), Type(Void).topWith(Const()).topWith(Pointer()).topWith(Reference()));
	}
	foreach (String s, String("<,>,<=,>=,==,!=").split(","))
		SimpleOperator::create<SimpleType>(Operator(s), Type(Bool), (Type(Void).topWith(Const()).topWith(Pointer()), Type().topWith(Pointer())));
	foreach (String s, String("+,-").split(","))
		SimpleOperator::create<SimpleType>(Operator(s, Operator::Binary), Type().topWith(Pointer()), (Type(Void).topWith(Const()).topWith(Pointer()), Type(Int)));
	foreach (String s, String("+=,-=").split(","))
		SimpleOperator::create<SimpleType>(Operator(s), Type().topWith(Pointer()).topWith(Reference()), (Type(Void).topWith(Const()).topWith(Pointer()).topWith(Reference()), Type(Int)));
}

void SimpleType::finaliseClass()
{
	while (s_nonMembers.size())
		s_nonMembers.takeLast()->destruct();
}

}
