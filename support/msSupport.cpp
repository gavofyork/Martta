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

#include "msDebug.h"

namespace MarttaSupport
{

AssertionHandler s_alternativeAssertionHandler = 0;

#if defined(DEBUG)
void assertFailed(int _line, char const* _file, char const* _function, char const* _failed, char const* _reason)
{
	if (s_alternativeAssertionHandler)
		(*s_alternativeAssertionHandler)(_line, _file, _function, _failed, _reason);
	else
	{
		DebugStream(DebugStream::FailedAssert) << _line << "@" << _file << "(" << _function << "): Assert(" << _failed << ") " << _reason;
#ifndef M_WIN
		asm ("int $0x03");
#else
		__asm
		{
			int 3
		}
#endif
	}
}
#endif

}

