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

#include "TopLevel.h"

namespace Martta
{

class Namespace: public TopLevel
{
	MARTTA_OBJECT(TopLevel)

public:
	virtual bool						isGloballyIdentifiable() const { return true; }
	virtual Kinds						allowedKinds(int _i) const;
	virtual String						defineLayout(ViewKeys const&) const;
	
	String								interfaceCode() const;
	String								implementationCode() const;

	virtual bool						keyPressed(KeyEvent const* _e);

protected:
	virtual inline String				defineLabelCode(String const& _text) const { return _text[0].isNumber() ? L"_" + camelCase(_text, true) : camelCase(_text, true); }
	virtual inline String				defineLabelName(String const& _text) const { return camelCase(_text, true); }
};

}
