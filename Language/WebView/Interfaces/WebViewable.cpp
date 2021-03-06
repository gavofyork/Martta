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

#include "WebStylist.h"
#include "WebStylistRegistrar.h"
#include "EditDelegate.h"
#include "Concept.h"
#include "WebViewable.h"

namespace Martta
{

MARTTA_NOTION_CPP(WebViewable);

String WebViewable::cssBorder(String const& _name, Rgb _col)
{
	return String(L"%4head { background-image: -webkit-gradient(linear, left top, left bottom, from(%1), to(%2)); border-top: 1px %3 solid; border-left: 1px %2 solid; border-right: 1px %2 solid; padding: 1px 4px; } %4body { border-bottom: 2px %2 solid; border-left: 2px %2 solid; border-right: 2px %2 solid; background-color: transparent; }")
		.arg(_col.interpolated(50, Rgb(0xffffff)).name())
		.arg(_col.name())
		.arg(_col.interpolated(40, Rgb(0xffffff)).name())
		.arg(_name);
}

CssRegisterer::CssRegisterer(AuxilliaryFace const* _f, String const& _css):
	m_f(_f)
{
	WebStylistRegistrar::get()->registerCss(_f, _css);
}

CssRegisterer::~CssRegisterer()
{
	WebStylistRegistrar::get()->unregisterCss(m_f);
}

String WebViewable::defineEditHtml(CodeScene*) const
{
	return defineHtml().replace(L"<^>", L"");
}

String WebViewable::htmlEscape(String const& _s)
{
	return String(_s)
		.replace(L"&", L"&amp;")
		.replace(L"\\", L"&#92;")
		.replace(L"<", L"&lt;")
		.replace(L">", L"&gt;")
		.replace(L"'", L"&apos;")
		.replace(L"\"", L"&quot;");
}

String WebViewable::composeName(String const& _id, StringList const& _flags)
{
	AssertNR(WebStylist::current());
	return WebStylist::current()->composeName(_id, _flags);
}

String WebViewable::toHtml(Concept const* _e, String const& _tag)
{
	return WebStylist::current()->toHtml(_e, _tag);
}

String WebViewable::toHtml(List<Concept const*> const& _es, String const& _delimiter, String const& _tag, bool _delimiterOnLast)
{
	String ret;
	foreach (Concept const* e, _es)
	{
		ret += toHtml(e, _tag);
		if (_delimiterOnLast || e != _es.last())
			ret += _delimiter;
	}
	return ret;
}

}
