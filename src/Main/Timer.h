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

#include <QTime>

#include <msDebug.h>
#include <msString.h>
using MarttaSupport::String;

namespace Martta
{

class Timer
{
public:
	Timer(String const& _name): m_name(_name), m_time(QTime::currentTime()) {}
	~Timer() { mInfo() << m_name << ":" << m_time.elapsed() << "ms"; }
	String m_name;
	QTime m_time;
};

#define TIME_START(i) s_timeTimers[i].start()
#define TIME_STOP(i) if (true) { s_timeTotals[i] += s_timeTimers[i].elapsed(); s_timeCount[i]++; } else void(0)
#define TIME_TOTAL(i) s_timeTotals[i]
#define TIME_COUNT(i) s_timeCount[i]
#define TIME_FUNCTION Timer __x(__FUNCTION__)
#define TIME_STATEMENT(N) for (int __i = 0; !__i;) for (Timer __x(#N); !__i; ++__i)

extern int s_timeTotals[16];
extern int s_timeCount[16];
extern QTime s_timeTimers[16];

}