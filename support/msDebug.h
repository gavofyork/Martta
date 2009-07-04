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

#include "msSupport.h"
#include "msTextStream.h"

namespace MarttaSupport
{

#if defined(RELEASE) || defined(PROFILE)

struct NullStream
{
	inline NullStream& template<class T> operator<<(T) { return *(NullStream*)0; }
};

#define mInfo() (*(MarttaSupport::NullOut*)0)
#define mDebug() (*(MarttaSupport::NullOut*)0)
#define mWarning() (*(MarttaSupport::NullOut*)0)
#define mCritical() (*(MarttaSupport::NullOut*)0)

#else

void debugOutput(wchar_t const* _t);

class DebugStream
{
public:
	enum Channel { Info = 0, Debug, Warning, Critical, FailedAssert };
	
	inline DebugStream(Channel _ch = Debug):
		m_buffer	(_ch == Warning ? L"*** WARNING " : _ch == Critical ? L"!!! CRITICAL " : _ch == FailedAssert ? L"!!! ASSERT FAILED " : L""),
		m_ts		(&m_buffer),
		m_channel	(_ch)
	{
	}
	inline DebugStream(char const* _context, Channel _ch = Debug):
		m_buffer	(_ch == Warning ? L"*** WARNING " : _ch == Critical ? L"!!! CRITICAL " : L""),
		m_ts		(&m_buffer),
		m_channel	(_ch)
	{
		m_buffer += _context;
	}
	inline ~DebugStream()
	{
		if (!m_buffer.isEmpty() && !m_buffer.endsWith("\n"))
			m_buffer.append(L'\n');
		debugOutput(m_buffer.data());
	}
	
	template<typename T>
	DebugStream& operator<<(T const& _t) { ensureSpace(); m_ts << _t; return *this; }
	
private:
	inline void ensureSpace()
	{
		if (!m_buffer.isEmpty() && !m_buffer.endsWith(" "))
			m_buffer.append(L' ');
	}

	String m_buffer;
	TextStream m_ts;
	int m_channel;
};

#define mInfo() MarttaSupport::DebugStream(MarttaSupport::DebugStream::Info) << ""
#define mDebug() MarttaSupport::DebugStream(MarttaSupport::DebugStream::Debug) << ""
#define mWarning() MarttaSupport::DebugStream(MarttaSupport::DebugStream::Warning) << ""
#define mCritical() MarttaSupport::DebugStream(MarttaSupport::DebugStream::Critical) << ""

#endif
}
