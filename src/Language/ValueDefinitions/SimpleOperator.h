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

#include "TypeEntity.h"
#include "ValueDefiner.h"
#include "DeclarationEntity.h"

namespace Martta
{

class SimpleOperator: public DeclarationEntity, public_interface ValueDefiner
{
	MARTTA_OBJECT(DeclarationEntity)
	MARTTA_INHERITS(ValueDefiner, 0)
	
public:
	SimpleOperator() {}
	template<class T> static SimpleOperator* create(Operator _o, Type const& _returns, Types const& _args, RootEntity* _root)
	{
		SimpleOperator* s = new SimpleOperator;
		s->construct(T::s_nonMembers.count(), _o, _returns, _args, _root, T::staticAuxilliary()->name());
		T::s_nonMembers.append(s);
		return s;
	}
	virtual ~SimpleOperator();
	virtual Operator					id() const { return m_operator; }
	virtual Type						type() const { return *entityAs<TypeEntity>(0); }
	virtual QString						key() const { return "@" + m_key + "@N" + QString::number(m_myId); }

private:
	void								construct(int _id, Operator _o, Type const& _returns, Types const& _args, RootEntity* _root, char const* _key);
	
	Operator							m_operator;
	QString								m_key;
	int									m_myId;
};

}
