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

#include "Simple.h"

namespace Martta
{

class SimpleMethod: public Simple
{
	MARTTA_OBJECT(Simple)

public:
	template<class T> inline static SimpleMethod* create(QString const& _name, bool _isConst, Type const& _returns, Types const& _args)
	{
		SimpleMethod* s = new SimpleMethod;
		s->m_name = _name;
		T t;
		s->construct(&t, T::s_members.count(), _isConst, _returns, _args, T::staticAuxilliary()->name());
		T::s_members.append(s);
		return s;
	}
	virtual QString						name() const { return m_name; }
	virtual QString						codeName() const { return m_name; }
	virtual QString						reference() const { return m_name; }
	virtual QString						key() const { return "@" + m_key + "@" + QString::number(m_myId); }
	
private:
	QString								m_name;
};

}

