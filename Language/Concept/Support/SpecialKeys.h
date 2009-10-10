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

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

enum
{
	ShiftModifier = 0x01,
	ControlModifier = 0x02
};

M_API_Concept extern const wchar_t* DeleteKey;
M_API_Concept extern const wchar_t* EscapeKey;
M_API_Concept extern const wchar_t* InsertKey;
M_API_Concept extern const wchar_t* PageUpKey;
M_API_Concept extern const wchar_t* PageDownKey;
M_API_Concept extern const wchar_t* HomeKey;
M_API_Concept extern const wchar_t* EndKey;
M_API_Concept extern const wchar_t* ReturnKey;
M_API_Concept extern const wchar_t* BackspaceKey;
M_API_Concept extern const wchar_t* TabKey;
M_API_Concept extern const wchar_t* LeftKey;
M_API_Concept extern const wchar_t* UpKey;
M_API_Concept extern const wchar_t* DownKey;
M_API_Concept extern const wchar_t* RightKey;

}
