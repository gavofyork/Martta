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

#include "Operator.h"
#include "ModelPtr.h"
#include "UnaryOperation.h"

namespace Martta
{

class SimpleUnaryOperation: public UnaryOperation
{
	MARTTA_OBJECT(UnaryOperation)

public:
	SimpleUnaryOperation(Operator _o = Operator(), Type const& _type = Type()): m_operator(_o), m_symbolCache(0) { setOperation(_o, _type); }

	static bool							keyPressedOnPosition(Position const&, KeyEvent const*);

protected:
	virtual Operator					id() const { return m_operator; }
	virtual String						operatorHtml() const;
	virtual Types						allowedTypes(int _index) const;
	virtual Type						type() const;
	virtual String						code() const;
	virtual bool						isPostfix() const { return id().isPostfix(); }
	virtual Precedence					precedence() const { return id().precedence(); }
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(int, Entity*) { refreshOperation(); }
	virtual void						apresLoad() { refreshOperation(); }

private:
	Type								prototypeOf(int _index) const { if (!m_symbolCache) return Type(); return Operation::prototypeOf(m_symbolCache->type(), _index); }
	Type								protoReturn() const { if (!m_symbolCache) return Type(); return Operation::prototypeOf(m_symbolCache->type()); }
	// Note these don't call changed().
	void								refreshOperation() { setOperation(m_operator, operandType()); }
	void								setOperation(Operator _o, Type const& _type = Type());
	static List<ValueDefiner*>			findOperators(Operator _o, Type const& _type = Type());
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"operator"] = String::number(m_operator.symbol()); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_operator = Operator((Operator::Symbol)_p[L"operator"].toInt()); }

	Operator							m_operator;
	ValueDefiner*						m_symbolCache;
};

}
