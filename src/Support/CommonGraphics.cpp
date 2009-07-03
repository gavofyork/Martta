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

#include "CommonGraphics.h"

namespace Martta
{

QColor colourByName(String const& _n)
{
	QColor ret;
	if (_n.startsWith("#") && (_n.size() - 1) % 4 == 0)
	{
		ret = QColor(_n.mid(0, (_n.size() - 1) / 4 * 3 + 1).toQString());
		ret.setAlpha(QColor(String("#%1%1%1").arg(_n.mid((_n.size() - 1) / 4 * 3 + 1, (_n.size() - 1) / 4)).toQString()).red());
	}
	else
		ret = QColor(_n.toQString());
	return ret;
}

}
