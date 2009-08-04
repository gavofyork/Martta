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

namespace Martta
{

enum
{
	ShiftModifier = 0x01,
	ControlModifier = 0x02
};

extern const wchar_t* DeleteKey;
extern const wchar_t* EscapeKey;
extern const wchar_t* InsertKey;
extern const wchar_t* PageUpKey;
extern const wchar_t* PageDownKey;
extern const wchar_t* HomeKey;
extern const wchar_t* EndKey;
extern const wchar_t* ReturnKey;
extern const wchar_t* BackspaceKey;
extern const wchar_t* TabKey;
extern const wchar_t* LeftKey;
extern const wchar_t* UpKey;
extern const wchar_t* DownKey;
extern const wchar_t* RightKey;

}
