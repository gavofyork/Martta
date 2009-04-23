/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "Common.h"

void assertFailed(char const* _c)
{
	if (Martta::s_testing)
		Martta::s_asserted = _c;
	else
	{
		qCritical("Assertion failed: %s", _c);
		int a = 69;
		(void)a;
	}
}

namespace Martta
{

bool s_testing = false;
char const* s_asserted = 0;

int s_timeTotals[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int s_timeCount[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
QTime s_timeTimers[16];

const Precedence NoPrecedence = log(0);

QString code(Qualifiers _q, WhitespacePosition _p)
{
	QString ret;
	for (uint i = 1; (uint)QualifierMask > i; i <<= 1)
		if (_q & i)
			ret += ((!ret.isEmpty() && _p == NoWhitespace || _p == AtStart) ? " " : "") + code((Qualifier)i) + ((_p == AtEnd) ? " " : "");
	return ret;
}

QColor colourByName(QString const& _n)
{
	QColor ret;
	if (_n.startsWith("#") && (_n.size() - 1) % 4 == 0)
	{
		ret = QColor(_n.mid(0, (_n.size() - 1) / 4 * 3 + 1));
		ret.setAlpha(QColor(QString("#%1%1%1").arg(_n.mid((_n.size() - 1) / 4 * 3 + 1, (_n.size() - 1) / 4))).red());
	}
	else
		ret = QColor(_n);
	return ret;
}

}
