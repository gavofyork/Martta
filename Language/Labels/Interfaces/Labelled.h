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

#include "Concept.h"

#ifndef M_API_Labels
#define M_API_Labels M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Labels Labelled: public_interface Concept
{
	MARTTA_NOTION(Concept)

public:
	virtual ~Labelled() {}

	inline String labelHtml(String const& _text) const
	{
		if (!this)
			return Labelled::defineLabelHtml(_text);
		else
			return defineLabelHtml(_text);
	}

	inline String labelCode(String const& _text) const
	{
		if (!this)
			return Labelled::defineLabelCode(_text);
		else
			return defineLabelCode(_text);
	}

protected:
	virtual inline String defineLabelHtml(String const& _text) const { return _text; }
	virtual inline String defineLabelCode(String const& _text) const { return camelCase(_text); }
};

}
