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

#include <QDebug>

#include "Common.h"
using namespace Martta;

class TestHelper
{
public:
	TestHelper(char const* _n, int* _c):
		m_name			(_n),
		m_failed		(false),
		m_done			(false),
		m_oldAsserted	(s_asserted),
		m_failedCounter	(_c)
	{
		s_asserted = false;
		qInformation() << "Testing" << m_name;
	}
	~TestHelper()
	{
		if (m_failed || s_asserted)
		{
			qInformation() << "FAILED! " << (m_failed ? m_failed : s_asserted);
			exit(1);
			(*m_failedCounter)++;
		}
		s_asserted = m_oldAsserted;
	}
	void failed() { m_failed = "Unknown"; }
	void failed(char const* _problem) { m_failed = _problem; }
	bool isDone() const { return m_done; }
	void done() { m_done = true; }
	
private:
	char const* m_name;
	char const* m_failed;
	bool m_done;
	char const* m_oldAsserted;
	int* m_failedCounter;
};
#define TEST(S) for (TestHelper _h(S, &failed); !_h.isDone(); _h.done()) 
#define FAILED_IF(X) if (X) _h.failed(#X); else (void)0
#define UNLESS(X) if (X) (void)0; else _h.failed
#define IF(X) if (!(X)) (void)0; else _h.failed
