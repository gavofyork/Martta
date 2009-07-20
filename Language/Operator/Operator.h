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

#pragma once

#include <limits>

#include <msSupport.h>
using namespace MarttaSupport;

namespace Martta
{

typedef double Precedence;
static const Precedence NoPrecedence = std::numeric_limits<double>::quiet_NaN();

enum Associativity
{
	NoAssociativity = 0,
	LeftAssociativity,
	RightAssociativity
};

class Operator
{
	friend inline uint hashOf(Operator const&);

public:
	enum Symbol { NoOperator = -1, PlusPlusX = 0, MinusMinusX, PlusX, MinusX, BangX, StarX, TildeX, LastPrefix = TildeX, XPlusPlus, XMinusMinus, LastConfusable = XMinusMinus, XArrow, LastUnary = XArrow,
		Plus, Minus, Star, Slash, AmpersAmpers, BarBar, Equals, EqualsEquals, BangEquals, PlusEquals, MinusEquals, StarEquals, SlashEquals,
		GreaterThan, LessThan, GreaterThanEquals, LessThanEquals, LeftShift, RightShift, LeftShiftEquals, RightShiftEquals,
		Ampers, Bar, Hat, AmpersEquals, BarEquals, HatEquals, Percent, PercentEquals, Comma, Squares, LastBinary = Squares, Parentheses, SymbolCount };

	Operator(Symbol _s = NoOperator): m_symbol(_s) {}
	enum Arity { Unknown = -1, UnaryPrefix = 0, UnaryPostfix, Binary };
	Operator(String const& _symbol, Arity _a = Unknown);

	Symbol								symbol() const { return m_symbol; }
	void								setSymbol(Symbol _s) { m_symbol = _s; }
	
	bool								isNull() const { return m_symbol == NoOperator; }
	bool								isPostfix() const { return (int)m_symbol > (int)LastPrefix && (int)m_symbol <= (int)LastUnary; }
	bool								isPrefix() const { return isUnary() && !isPostfix(); }
	bool								isConfusablePostfix() const { return (int)m_symbol > (int)LastPrefix && (int)m_symbol <= (int)LastConfusable; }
	bool								isUnary() const { return (int)m_symbol <= (int)LastUnary; }
	bool								isBinary() const { return (int)m_symbol > (int)LastUnary; }
	inline char const*					code() const;
	inline Precedence					precedence() const;
	inline Associativity				associativity() const;

	int									key(int _type) const { return (int)m_symbol + _type * 0x100; } 
	int									key(int _left, int _right) const { return (int)m_symbol + _left * 0x100 + _right * 0x10000; } 
	
	inline bool							operator==(Operator _c) const { return m_symbol == _c.m_symbol; }
	inline bool							operator!=(Operator _c) const { return !operator==(_c); }
	
private:
	Symbol								m_symbol;
};

inline uint hashOf(Operator const& _o) { return (uint)_o.m_symbol; }

}

inline char const* Martta::Operator::code() const
{
	switch (m_symbol)
	{
		case PlusPlusX: case XPlusPlus: return "++";
		case MinusMinusX: case XMinusMinus: return "--";
		case PlusX: case Plus: return "+";
		case MinusX: case Minus: return "-";
		case StarX: case Star: return "*";
		case BangX: return "!";
		case TildeX: return "~";
		case XArrow: return "->";
		case Slash: return "/";
		case AmpersAmpers: return "&&";
		case BarBar: return "||";
		case Equals: return "=";
		case EqualsEquals: return "==";
		case BangEquals: return "!=";
		case PlusEquals: return "+=";
		case MinusEquals: return "-=";
		case StarEquals: return "*=";
		case SlashEquals: return "/=";
		case GreaterThan: return ">";
		case LessThan: return "<";
		case GreaterThanEquals: return ">=";
		case LessThanEquals: return "<=";
		case LeftShift: return "<<";
		case RightShift: return ">>";
		case LeftShiftEquals: return "<<=";
		case RightShiftEquals: return ">>=";
		case Ampers: return "&";
		case Bar: return "|";
		case Hat: return "^";
		case AmpersEquals: return "&=";
		case BarEquals: return "|=";
		case HatEquals: return "^=";
		case Percent: return "%";
		case PercentEquals: return "%=";
		case Comma: return ",";
		case Squares: return "[]";
		case Parentheses: return "()";
		default: return "";
	}
};

inline Martta::Precedence Martta::Operator::precedence() const
{
	switch (m_symbol)
	{
		case Parentheses:
		case Squares:
		case XArrow:
		case XPlusPlus:
		case XMinusMinus: return 2;
		case BangX:
		case TildeX:
		case PlusPlusX:
		case MinusMinusX:
		case MinusX:
		case PlusX:
		case StarX: return 3;
		case Star:
		case Slash:
		case Percent: return 5;
		case Plus:
		case Minus: return 6;
		case LeftShift:
		case RightShift: return 7;
		case GreaterThan:
		case LessThan:
		case GreaterThanEquals:
		case LessThanEquals: return 8;
		case EqualsEquals:
		case BangEquals: return 9;
		case Ampers: return 10;
		case Hat: return 11;
		case Bar: return 12;
		case AmpersAmpers: return 13;
		case BarBar: return 14;
		case Equals:
		case PlusEquals:
		case MinusEquals:
		case StarEquals:
		case SlashEquals:
		case PercentEquals:
		case AmpersEquals:
		case HatEquals:
		case BarEquals:
		case LeftShiftEquals:
		case RightShiftEquals: return 16;
		case Comma: return 17;
		default: return NoPrecedence;
	}
};

inline Martta::Associativity Martta::Operator::associativity() const
{
	switch (m_symbol)
	{
		case Parentheses:
		case Squares:
		case XArrow:
		case XPlusPlus:
		case XMinusMinus: return LeftAssociativity;
		case BangX:
		case TildeX:
		case PlusPlusX:
		case MinusMinusX:
		case MinusX:
		case PlusX:
		case StarX: return RightAssociativity;
		case Star:
		case Slash:
		case Percent:
		case Plus:
		case Minus:
		case LeftShift:
		case RightShift:
		case GreaterThan:
		case LessThan:
		case GreaterThanEquals:
		case LessThanEquals:
		case EqualsEquals:
		case BangEquals:
		case Ampers:
		case Hat:
		case Bar:
		case AmpersAmpers:
		case BarBar: return LeftAssociativity;
		case Equals:
		case PlusEquals:
		case MinusEquals:
		case StarEquals:
		case SlashEquals:
		case PercentEquals:
		case AmpersEquals:
		case HatEquals:
		case BarEquals:
		case LeftShiftEquals:
		case RightShiftEquals: return RightAssociativity;
		case Comma: return LeftAssociativity;
		default: return NoAssociativity;
	}
};
