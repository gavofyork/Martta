/* ***** BEGIN LICENSE BLOCK *****
 * Version: MarttaSupport License version 1.0
 *
 * The contents of this file are subject to the MarttaSupport License
 * version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You should have received a copy of the
 * MarttaSupport License "COPYING.MarttaSupport" along with Martta; if not
 * you may obtain a copy of the License at http://quidprocode.co.uk/Martta/
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

#include "msString.h"
#include "msList.h"

namespace MarttaSupport
{

class StringList: public List<String>
{
public:
	StringList() {}
	StringList(String const& _s1) { append(_s1); }
	StringList(String const& _s1, String const& _s2) { append(_s1); append(_s2); }
	StringList(String const& _s1, String const& _s2, String const& _s3) { append(_s1); append(_s2); append(_s3); }
	StringList(String const& _s1, String const& _s2, String const& _s3, String const& _s4) { append(_s1); append(_s2); append(_s3); append(_s4); }
	StringList(String const& _s1, String const& _s2, String const& _s3, String const& _s4, String const& _s5) { append(_s1); append(_s2); append(_s3); append(_s4); append(_s5); }

	StringList startsWith(String const& _s) const;
	String join(String const& _sep) const;
	String join(Char _sep) const;
};

}
