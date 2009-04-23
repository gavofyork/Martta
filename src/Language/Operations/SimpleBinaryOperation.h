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

#include "ValueDefinition.h"
#include "Operator.h"
#include "BinaryOperation.h"

namespace Martta
{

class MethodOperator;
class BasicOperator;
class FundamentalOperator;
	
class SimpleBinaryOperation: public BinaryOperation
{
	MARTTA_OBJECT(BinaryOperation)

public:
	SimpleBinaryOperation(Operator _o = Operator(), Type const& _left = Type(), Type const& _right = Type()): m_operator(_o), m_symbolCache(0) { setOperation(_o, _left, _right); }
	
	static bool							keyPressedOnInsertionPoint(InsertionPoint const&, EntityKeyEvent const*);

protected:
	virtual Operator					id() const { return m_operator; }
	virtual QString						operatorLayout() const;
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual Types						allowedTypes(int _index) const;
	virtual Type						type() const;
	virtual QString						code() const;
	virtual Precedence					precedence() const { return id().precedence(); }
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity* _e);
	virtual void						apresLoad() { setOperation(m_operator); }
	
private:
	/// The Type of the argument _index according to the symbol.
	Type								prototypeOf(int _index) const { if (!m_symbolCache.isUsable()) return Type(); return Operation::prototypeOf(m_symbolCache->type(), _index); }
	Type								protoReturn() const { if (!m_symbolCache.isUsable()) return Type(); return Operation::prototypeOf(m_symbolCache->type(), -1); }
	void								setOperation(Operator _o, Type const& _left = Type(), Type const& _right = Type());
	void								refreshOperation() { setOperation(m_operator, leftType(), rightType()); }
	
	static QList<ValueDefinition*>		findOperators(Operator _o, Type const& _left = Type(), Type const& _right = Type());
	
	// Change to OperatorLabel?
	Operator							m_operator;
	ModelPtr<ValueDefinition>			m_symbolCache;
};

}
