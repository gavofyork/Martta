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

#include "LambdaNamer.h"
#include "Argument.h"
#include "CompletionDelegate.h"
#include "ArgumentReferenced.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ArgumentReferenced);

bool ArgumentReferenced::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text() == L"_")
	{
		ArgumentReferenced* r = new ArgumentReferenced;
		_p.place(r);
		r->setEditing(_e->codeScene());
	}
	else
		return false;
	return true;
}

List<ValueDefiner*> ArgumentReferenced::possibilities(Position const& _p)
{
	List<ValueDefiner*> ret;
	if (LambdaNamer* ln = _p->ancestor<LambdaNamer>())
		for (int i = 0; i < ln->argumentCount(); i++)
			ret << ln->argument(i);
	return ret;
}

}
