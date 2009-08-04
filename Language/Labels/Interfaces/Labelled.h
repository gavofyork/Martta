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

#include "EntitySupport.h"
#include "Meta.h"

namespace Martta
{

class Labelled
{
	MARTTA_INTERFACE

public:
	virtual ~Labelled() {}

	inline String labelLayout(String const& _middle, ViewKeys const& _k) const
	{
		if (!this)
			return Labelled::defineLabelLayout(_middle, _k);
		else
			return defineLabelLayout(_middle, _k);
	}

	inline String labelHtml(String const& _middle) const
	{
		if (!this)
			return Labelled::defineLabelHtml(_middle);
		else
			return defineLabelHtml(_middle);
	}

	inline String labelCode(String const& _text) const
	{
		if (!this)
			return Labelled::defineLabelCode(_text);
		else
			return defineLabelCode(_text);
	}

	inline String labelName(String const& _text) const
	{
		if (!this)
			return Labelled::defineLabelName(_text);
		else
			return defineLabelName(_text);
	}

protected:
	virtual inline String defineLabelHtml(String const& _middle) const { return _middle; }
	virtual inline String defineLabelLayout(String const& _middle, ViewKeys const&) const { return _middle; }
	virtual inline String defineLabelCode(String const& _text) const { return labelName(_text); }
	virtual inline String defineLabelName(String const& _text) const { return camelCase(_text); }
};

}
