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

#include "CompletionDelegate.h"
#include "WebViewable.h"
#include "Named.h"

#ifndef M_API_Declaration
#define M_API_Declaration M_OUTAPI
#endif

namespace Martta
{

class IdentifierSet;

class M_CLASS M_API_Declaration NameEntryPoint: public_super_interface WebViewable
{
	MARTTA_NOTION(WebViewable)

public:
	Named*								get() const { return 0; }
	void								set(Named*) const {}
	void								committed(Named*, CodeScene* _cs);	// TODO!!! actually use (passing _cs)
	List<Named*>						possibilities() const;
	IdentifierSet*						ownerOf(Named* _n) const;

protected:
	bool								keyPressed(KeyEvent const*);

	template<class T> EditDelegateFace*	newDelegate(CodeScene* _s) { return isPlaceholder() ? new CompletionDelegate<T, Named*>(asKind<T>(), _s, false) : 0; }

	virtual String						defineHtml() const { return L"<^>&empty;"; }
	virtual String						defineEditHtml(CodeScene* _cs) const;

	virtual ~NameEntryPoint() {}
};

}
