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

namespace Martta
{

class MemberTemplateType: public TypeEntity
{
	MARTTA_OBJECT(TypeEntity)

public:
	MemberTemplateType(int _index = 0): m_index(_index) {}
	
	virtual QString						code(QString) const { return QString("<%1>").arg(m_index); }

	/// After calling substitute, this object may no longer exist.
	void								substitute();
	
protected:
	virtual TypeEntity*					newClone() const { return new MemberTemplateType(m_index); }
	virtual bool						defineSimilarityFrom(TypeEntity const* _f, Castability _c) const;
	virtual bool						defineEquivalenceFrom(TypeEntity const* _f) const;

private:
	int									m_index;
};

}
