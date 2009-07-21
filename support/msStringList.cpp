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

#include "msStringList.h"

namespace MarttaSupport
{

StringList StringList::startsWith(String const& _s) const
{
	StringList ret;
	foreach (String i, *this)
		if (i.toLower().startsWith(_s.toLower()))
			ret << i;
	return ret;
}

String StringList::join(String const& _sep) const
{
	if (isEmpty())
		return String::null;

	String ret = at(0);
	for (int i = 1; i < count(); ++i)
		ret += _sep + at(i);
	return ret;
}

String StringList::join(Char _sep) const
{
	if (isEmpty())
		return String::null;

	String ret = at(0);
	for (int i = 1; i < count(); ++i)
		ret += _sep + at(i);
	return ret;
}

}

