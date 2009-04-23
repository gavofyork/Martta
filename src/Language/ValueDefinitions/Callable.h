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

#include "Variable.h"
#include "Compound.h"
#include "ValueDefinition.h"

namespace Martta
{

class Callable: public ValueDefinition
{
	MARTTA_OBJECT_INTERFACE(ValueDefinition)

public:
	Callable(): m_qualifiers(Qualifiers(0)) {}
	
	/// Used for default implementation of implementationCode. Can be safely ignored if implementationCode is reimplemented.
	virtual Compound*					body() const { return entityAs<Compound>(1); }
	virtual bool						ellipsis() const { return false; }
	virtual Qualifiers					qualifiers() const { return m_qualifiers; }
	
	enum FunctionCodeScope { InsideScope = 0, OutsideScope = 1 };
	
	/// Basic code for this function call. Is essentially the interfaceCode without the final ';'.
	virtual QString						code(FunctionCodeScope _ref) const;
	virtual QString						interfaceCode() const { return code(Callable::InsideScope) + ";\n"; }
	virtual QString						implementationCode() const;

	/// @note this only returns the number of nameable arguments. If ellipsis() is true, this could be different to the number of parameters that are passed to it in an invocation.
	/// @note argument() may not work for all arguments within argumentCount(); argumentType() is the only one guaranteed to work. This is the case in, e.g., postfix versions of ++ and -- unary operators.
	virtual int							argumentCount() const { return entities().size() - firstArgumentIndex(); }
	virtual Variable*					argument(int _index) const;
	virtual inline Type					returns() const { return entityIs<TypeEntity>(2) ? *entityAs<TypeEntity>(2) : Type(); }
	virtual inline Type					argumentType(int _index) const { return argument(_index)->actualType() ? *argument(_index)->actualType() : Type(); }
	virtual inline QString				argumentName(int _index) const { return argument(_index)->name(); }
	virtual inline QString				argumentCodeName(int _index) const { return argument(_index)->codeName(); }

protected:
	/// This is a basic implementation that works as Compound, ReturnType, ArgumentVariables, ...
	/// This will need reimplementing unless the derived uses this format.
	virtual Kinds						allowedKinds(int _i) const;
	virtual Kinds						deniedKinds(int _i) const;
	/// The index of the child entity that represents the first argument (if one could exist, hypothetically).
	/// minimumRequired() is defined as being the same as this by default, so of the two, you only need to change
	/// this in derived types that need more Label entities.
	/// This doesn't stop you from overloading both if you really want to, or if you want to force a minimum number
	/// of arguments.
	virtual int							firstArgumentIndex() const { return 3; }
	virtual int							minimumRequired() const { return firstArgumentIndex(); }
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual bool						activated(CodeScene* _s);
	virtual QString						identity() const;
	virtual Type						type() const;
	virtual QString						defineLayout(ViewKeys& _v) const;
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); }
	virtual void						onDependencyRemoved(Entity*) { changed(); }
	
	/// Gives a parameter list of the call. This depends on argumentType/CodeName, ellipsis, reference and codeName.
	QString								callingCode(FunctionCodeScope _ref) const;
	
	Qualifiers							m_qualifiers;
};

}
