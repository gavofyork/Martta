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

#include <msString.h>
#include <msStringList.h>
using namespace MarttaSupport;

#include "Concept.h"

#ifndef M_API_Language
#define M_API_Language M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Language Module: public_interface Concept
{
	MARTTA_NOTION(Concept)

public:
	virtual ~Module() {}

	String const&						filename() const { return m_filename; }
	void								setFilename(String const& _fn) { m_filename = _fn; }

	virtual void						save() const {}
	virtual List<StringList>			steps() const;
	virtual String						finalCode() const { return String::null; }

protected:
	String const&						supportPath() const;

private:
	String								m_filename;
};

}
