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
#include "msSupport.h"

namespace MarttaSupport
{

class TextStream
{
public:
	TextStream(String* _buffer): m_buffer(*_buffer) {}
	~TextStream() {}
	
	String const& buffer() const { return m_buffer; }
	
	inline TextStream& operator<<(float _c) { m_buffer += String::number(_c); return *this; }
	inline TextStream& operator<<(double _c) { m_buffer += String::number(_c); return *this; }
	inline TextStream& operator<<(unsigned char _c) { m_buffer += String::number((unsigned long)_c); return *this; }
	inline TextStream& operator<<(short _c) { m_buffer += String::number((long)_c); return *this; }
	inline TextStream& operator<<(unsigned short _c) { m_buffer += String::number((unsigned long)_c); return *this; }
	inline TextStream& operator<<(int _c) { m_buffer += String::number((long)_c); return *this; }
	inline TextStream& operator<<(unsigned int _c) { m_buffer += String::number((unsigned long)_c); return *this; }
	inline TextStream& operator<<(long _c) { m_buffer += String::number(_c); return *this; }
	inline TextStream& operator<<(unsigned long _c) { m_buffer += String::number(_c); return *this; }
	
	inline TextStream& operator<<(void const* _c) { m_buffer += String::number((unsigned long)_c); return *this; }
	
	inline TextStream& operator<<(char _c) { m_buffer += _c; return *this; }
	inline TextStream& operator<<(wchar_t _c) { m_buffer += _c; return *this; }
	inline TextStream& operator<<(char const* _c) { m_buffer += String(_c); return *this; }
	inline TextStream& operator<<(wchar_t const* _c) { m_buffer += _c; return *this; }
	inline TextStream& operator<<(String const& _c) { m_buffer += _c; return *this; }
	
private:
	String& m_buffer;
};

}
