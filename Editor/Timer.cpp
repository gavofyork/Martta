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

#include <QKeyEvent>

#include <Entity.h>
#include <SpecialKeys.h>
#include <KeyEvent.h>

#include "Timer.h"

namespace Martta
{

int s_timeTotals[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int s_timeCount[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
QTime s_timeTimers[16];

int translateMods(int _e)
{
	int r = 0;
	if (_e & Qt::ShiftModifier)
		r |= ::Martta::ShiftModifier;
	if (_e & Qt::ControlModifier)
		r |= ::Martta::ControlModifier;
	return r;
}

wchar_t const* translateKey(int _e)
{
	switch (_e)
	{
	case Qt::Key_Delete:
		return ::Martta::DeleteKey;
	case Qt::Key_Escape:
		return ::Martta::EscapeKey;
	case Qt::Key_Insert:
		return ::Martta::InsertKey;
	case Qt::Key_PageUp:
		return ::Martta::PageUpKey;
	case Qt::Key_PageDown:
		return ::Martta::PageDownKey;
	case Qt::Key_Home:
		return ::Martta::HomeKey;
	case Qt::Key_End:
		return ::Martta::EndKey;
	case Qt::Key_Return:
		return ::Martta::ReturnKey;
	case Qt::Key_Backspace:
		return ::Martta::BackspaceKey;
	case Qt::Key_Tab:
	case Qt::Key_Backtab:
		return ::Martta::TabKey;
	case Qt::Key_Left:
		return ::Martta::LeftKey;
	case Qt::Key_Up:
		return ::Martta::UpKey;
	case Qt::Key_Down:
		return ::Martta::DownKey;
	case Qt::Key_Right:
		return ::Martta::RightKey;
	default:
		return 0;
	}
}

}
