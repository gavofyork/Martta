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

#include <msSupport.h>
#include <msHash.h>
#include <msString.h>
using namespace MarttaSupport;

#include "EntitySupport.h"

namespace Martta
{

class KeepCurrent;

class WebStylist
{
	friend class KeepCurrent;

public:
	WebStylist(): m_stack(0) {}
	virtual ~WebStylist() {}

	/// The stylist which is currently recalculating HTML.
	static WebStylist*					current() { return s_current; }

	/// The big function---get the HTML representation of an entity.
	/// @returns the @b outer HTML to represent entity @a _e. @a _tag specifies the HTML for the
	/// enclosing tag. Responsible for checking/using the cache.
	String								toHtml(Entity const* _e, String const& _tag = L"span");

	/// @returns just the @b inner portion of the HTML for entity @a _e.
	/// The HTML for that entity is recomputed and reflects any changes made in the object since
	/// the last call of this method (if any). Children are, however, unaffected if they have had
	/// their outer HTML added through addToHtmlCache().
	String								rejiggedHtml(Entity const* _e);

	/// Caches an element's outer HTML for later use in toHtml.
	inline void							addToHtmlCache(Entity const* _e, String const& _outerHtml) { m_htmlCache.insert(_e, _outerHtml); }

	/// @returns the @b inner HTML to represent entity @a _e under the scene @a _cs.
	String								editHtml(Entity const* _e, CodeScene* _cs);

protected:
	/// Overridable virtual for specifying novel style.
	/// @returns the inner portion of HTML for entity @a _e .
	virtual String						defineHtml(Entity const* _e);

	/// Overridable virtual for specifying novel style.
	/// @returns the inner portion of HTML for entity @a _e , when the edit delegate for this
	/// entity is active in the scene @a _cs .
	virtual String						defineEditHtml(Entity const* _e, CodeScene* _cs);

private:
	/// Refines an HTML string, conducting any special substitutions required.
	/// These substitutions include:
	/// - Replacing "<^>" pseudo-tag with a span element of id "this".
	/// - Interpreting "data://" URIs.
	String								refineHtml(String const& _html, bool _allowThis = true, bool _forceThis = false);

	int									m_stack;
	Hash<Entity const*, String>			m_htmlCache;

	static WebStylist*					s_current;
};

}
