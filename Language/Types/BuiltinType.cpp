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

#include <msStringList.h>
using namespace MarttaSupport;

#include "VariablePlacer.h"

#include "Const.h"
#include "Pointer.h"
#include "Reference.h"

#include "IdentifierSet.h"
#include "CodeScene.h"
#include "BuiltinOperator.h"
#include "CompletionDelegate.h"
#include "WebStylistRegistrar.h"
#include "BuiltinType.h"

namespace Martta
{

const int s_simpleIds[] = { Char, Int, Short|Int, Long|Int, Longlong|Int, Unsigned|Char, Unsigned|Int, Unsigned|Short|Int, Unsigned|Long|Int, Unsigned|Longlong|Int, Float, Double, Long|Double, Complex|Float, Complex|Double, Complex|Long|Double, Bool, Wchar, Void };
const int s_simpleIdsCount = sizeof(s_simpleIds) / sizeof(s_simpleIds[0]);

MARTTA_PROPER_CPP(BuiltinType);

List<BuiltinOperator*> BuiltinType::s_nonMembers;
Hash<String, Kind> BuiltinType::s_recognisedExtras;

class BuiltinNamed: public Named
{
public:
	BuiltinNamed(int _id): m_id(_id) {}
	virtual String nickFor() const { return BuiltinType::nickFor(m_id); }
	uint m_id;
};

class BuiltinTypeSet: public IdentifierSet
{
public:
	BuiltinTypeSet()
	{
		for (int i = 0; i < s_simpleIdsCount; i++)
			m_nameds.append(new BuiltinNamed(s_simpleIds[i]));
	}
	~BuiltinTypeSet()
	{
		while (m_nameds.size())
			delete m_nameds.takeLast();
	}
	virtual String						setId() const { return L"Martta::BuiltinType"; }
	virtual List<Named*>				identifiableAt(Position const& _p)
	{
		if (canPlaceVariable(_p))
			return m_nameds;
		return List<Named*>();
	}
	virtual void						acceptAt(Position const& _pos, Named* _i, CodeScene*)
	{
		placeVariable(_pos, new BuiltinType(static_cast<BuiltinNamed*>(_i)->m_id));
	}
	virtual String						defineEditHtml(Named*, String const& _mid)
	{
		return L"<^>" + BuiltinType().fullHtml(_mid);
	}
	List<Named*> m_nameds;
};

static BuiltinTypeSet s_builtinTypeSet;

bool BuiltinType::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text().length() == 1 && _e->text()[0].isLower())
	{
		BuiltinType* s = new BuiltinType;
		_p.place(s);
		_e->codeScene()->setEditing(s);
		if (_e->codeScene()->isEditing(s))
			_e->reinterpretLater();
	}
	else
		return false;
	return true;
}

bool BuiltinType::keyPressed(KeyEvent const* _e)
{
	if (_e->text().length() == 1 && (_e->text()[0].isLower() || _e->text()[0] == L':'))
	{
		_e->codeScene()->setEditing(this);
		if (_e->codeScene()->isEditing(this))
			_e->reinterpretLater();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

String BuiltinType::code(String const& _middle) const
{
	if (m_id == (uint)-1) return String::null;
	if (m_id == Wchar) return "wchar_t";
	return String((m_id & Unsigned) ? (m_id & Float || m_id & Double) ? "complex " : "unsigned " : "") +
			(((m_id & Longlong) == Longlong) ? "long long " : (m_id & Short) ? "short " : (m_id & Long) ? "long " : "") +
			((m_id & Float) ? "float" : (m_id & Bool) ? "bool" : (m_id & Double) ? "double" : (m_id & Char) ? "char" : (m_id & Int) ? "int" : "void") + _middle;
}

Types BuiltinType::assignableTypes() const
{
	return Type(*this);
}

bool BuiltinType::defineSimilarityTo(TypeConcept const* _t, Castability _c) const
{
	if (_t->isKind<BuiltinType>())
	{
		uint tId = _t->asKind<BuiltinType>()->m_id;
		if (tId != (uint)-1)
		{
			if (tId == m_id)
				return true;
			if (m_id != Void && (
				(isFairlyConvertibleAtMost(_c) && (
					((m_id & Int) && (tId & (Double | Float)) && !(tId & Complex))								// int -> scalar double/float
					|| ((m_id & (Double | Float)) && !(m_id & Complex) && (tId & Int))							// scalar double/float -> int
					|| ((m_id & (Double | Float)) && (tId & (Double | Float)) && ((m_id & Complex) == (tId & Complex)))	// double/float -> double/float (same scalar/complex)
				))
				||
				(isBasicallyConvertibleAtMost(_c) && (
					(m_id & (Int | Char)) && (tId & (Int | Bool))												// int/char -> int/bool
				))
				))
				return true;
		}
	}
	return Super::defineSimilarityTo(_t, _c);
}

bool BuiltinType::defineSimilarityFrom(TypeConcept const* _f, Castability _c) const
{
	return	(m_id == Void && !_f->isKind<ModifyingType>()) ||
			(isAnyConvertible(_c) && m_id == Bool && _f->isKind<AddressType>()) ||
		Super::defineSimilarityFrom(_f, _c);
}

String BuiltinType::defineHtml() const
{
	return L"<^><span class=\"TypeConcept\">" + typeHtml((id() == (uint)-1) ? L"&empty;" : nickFor(id())) + L"</span>";
}

}

namespace MarttaSupport
{
template<>
class NameTrait<int>
{
public:
	enum { ExtraIds = 0xff00 };
	static String name(int _val)
	{
		if (_val < ExtraIds)
			return ::Martta::BuiltinType::nickFor(_val);

		int i = ExtraIds;
		foreach (String s, ::Martta::BuiltinType::s_recognisedExtras.keys())
			if (_val == i++)
				return s;

		return String::null;
	}
};
}

namespace Martta
{

List<int> BuiltinType::possibilities() const
{
	List<int> ret;
	for (int i = 0; i < s_simpleIdsCount; i++)
		ret << s_simpleIds[i];
	for (int i = 0; i < s_recognisedExtras.count(); i++)
		ret << NameTrait<int>::ExtraIds + i;
	return ret;
}

String BuiltinType::defineEditHtml(CodeScene* _cs) const
{
	if (EditDelegateFace* d = _cs->editDelegate(this))
		return fullHtml(d->real() + tagOf(L"unreal", d->unreal())) + tagOf(L"minor", d->comment());
	return String::null;
}

EditDelegateFace* BuiltinType::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<BuiltinType, int>(this, _s);
}

void BuiltinType::committed(int, CodeScene*)
{
	// Switch if necessary...
	if (m_id >= NameTrait<int>::ExtraIds)
	{
		uint i = NameTrait<int>::ExtraIds;
		foreach (Kind k, s_recognisedExtras)
			if (m_id == i++)
			{
				replace(k.spawnPrepared());
				break;
			}
	}
}

void BuiltinType::initialiseClass()
{
//	WebStylistRegistrar::get()->registerCss<BuiltinType>();

	List<uint> integral;
	integral << Char << (Short|Int) << Int << (Long|Int) << (Longlong|Int) << (Unsigned|Char) << (Unsigned|Short|Int) << (Unsigned|Int) << (Unsigned|Long|Int) << (Unsigned|Longlong|Int);
	List<uint> scalar;
	scalar << integral << Float << Double << (Long|Double);
	List<uint> numeric;
	numeric << scalar << (Complex|Float) << (Complex|Double) << (Complex|Long|Double);

	// integral types
	foreach (uint d, integral)
	{
		// unit inc/decrement operators.
		foreach (String s, String("++,--").split(","))
		{	BuiltinOperator::create<BuiltinType>(Operator(s, Operator::UnaryPrefix), Type(d)/*.topWith(Reference())*/, Type(d).topWith(Reference()));
			BuiltinOperator::create<BuiltinType>(Operator(s, Operator::UnaryPostfix), Type(d)/*.topWith(Reference())*/, Type(d).topWith(Reference()));
		}
		// modulo/logical operators
		foreach (String s, String("%,&,|,^").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s), Type(d), (Type(d), Type(d)));
		foreach (String s, String(">>,<<").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s), Type(d), (Type(d), Type(Int)));
		foreach (String s, String("%=,&=,|=,^=").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s), Type(d).topWith(Reference()), (Type(d).topWith(Reference()), Type(d)));
		foreach (String s, String(">>=,<<=").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s), Type(d).topWith(Reference()), (Type(d).topWith(Reference()), Type(Int)));
		foreach (String s, String("~").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s), Type(d), Type(d));
	}

	// all scalar types
	foreach (uint d, scalar)
	{
		// comparison
		foreach (String s, String("<,>,<=,>=").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s), Type(Bool), (Type(d), Type(d)));
		// simple arithmetic
		foreach (String s, String("+,-,*,/").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s, Operator::Binary), Type(d), (Type(d), Type(d)));
		// increment
		foreach (String s, String("+=,-=,*=,/=").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s), Type(d).topWith(Reference()), (Type(d).topWith(Reference()), Type(d)));
		// signing
		foreach (String s, String("+,-").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s, Operator::UnaryPrefix), Type(d), Type(d));
	}

	// boolean
	foreach (String s, String("||,&&").split(","))
		BuiltinOperator::create<BuiltinType>(Operator(s), Type(Bool), (Type(Bool), Type(Bool)));
	foreach (String s, String("!").split(","))
		BuiltinOperator::create<BuiltinType>(Operator(s), Type(Bool), Type(Bool));

	// all types
	foreach (uint d, List<uint>(numeric) << Bool << Wchar)
	{
		// (in)equality
		foreach (String s, String("==,!=").split(","))
			BuiltinOperator::create<BuiltinType>(Operator(s), Type(Bool), (Type(d), Type(d)));
	}

	// pointer types (we use Ptr for them in the lookup)
	foreach (String s, String("++,--").split(","))
	{
		BuiltinOperator::create<BuiltinType>(Operator(s, Operator::UnaryPrefix), Type().topWith(Pointer()), Type(Void).topWith(Const()).topWith(Pointer()).topWith(Reference()));
		BuiltinOperator::create<BuiltinType>(Operator(s, Operator::UnaryPostfix), Type().topWith(Pointer()), Type(Void).topWith(Const()).topWith(Pointer()).topWith(Reference()));
	}
	foreach (String s, String("<,>,<=,>=,==,!=").split(","))
		BuiltinOperator::create<BuiltinType>(Operator(s), Type(Bool), (Type(Void).topWith(Const()).topWith(Pointer()), Type().topWith(Pointer())));
	foreach (String s, String("+,-").split(","))
		BuiltinOperator::create<BuiltinType>(Operator(s, Operator::Binary), Type().topWith(Pointer()), (Type(Void).topWith(Const()).topWith(Pointer()), Type(Int)));
	foreach (String s, String("+=,-=").split(","))
		BuiltinOperator::create<BuiltinType>(Operator(s), Type().topWith(Pointer()).topWith(Reference()), (Type(Void).topWith(Const()).topWith(Pointer()).topWith(Reference()), Type(Int)));
}

void BuiltinType::finaliseClass()
{
//	WebStylistRegistrar::get()->unregisterCss<BuiltinType>();

	while (s_nonMembers.size())
		s_nonMembers.takeLast()->destruct();
}

}
