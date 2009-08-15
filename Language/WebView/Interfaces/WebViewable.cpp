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
#include "Entity.h"
#include "WebViewable.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(WebViewable);

MARTTA_REGISTER_CSS(WebViewable,
	"body { color: #666; font-size: 12px; font-family: Lucida Grande; background-color: white; }"
	".keyword { font-weight: bold; }"
	".unreal { color: #888; }"
	".minor { color: #888; font-size: 8px; }"
	".symbol { font-weight: bold; }"
	".block { position: relative; margin-left: 20px; }"
	".deblock { position: relative; margin-left: -20px; }"
	".badge { margin: 0px 0px; padding: 0px 0.3em; border-top: 1px solid rgba(0,0,0,0.4); border-left: 1px solid rgba(0,0,0,0.2); border-bottom: 1px solid rgba(255,255,255,0.4); border-right: 1px solid rgba(255,255,255,0.2); background-image: -webkit-gradient(linear, left top, left bottom, from(rgba(0,0,0,0.2)), to(rgba(255,255,255,0.2))); font-weight: 900; color: rgba(255,255,255,0.85); text-shadow: 1px 1px 2px rgba(0,0,0,0.3); }"

	".TypeEntity { font-weight: bold; color: #000; }"
	".Referenced { font-weight: normal; color: #000; }"
	".Namespace-label { color: #000; font-weight: bold; }"
	".Class-label { color: #000; font-weight: bold; text-shadow: -1px -1px 0px #f77; }"
	".Label { font-weight: normal; }"
	".ConstLabel-constblock { -webkit-border-image: -webkit-gradient(linear, left top, right top, from(#fff), to(#bdf), color-stop(0.5, #bdf)) 100% 100% 100% 100% repeat repeat; border-width: 0 0 0 12px; padding-left: 8px; margin-left: -20px; }"
	".ConstLabel-nonconstblock { }"
	".AccessLabel-publicblock { margin-left: -8px; border-width: 0 0 0 4px; border-color: #efefbf; border-style: solid; padding-left: 4px; }"
	".AccessLabel-protectedblock { margin-left: -8px; border-width: 0 0 0 4px; border-color: #ffdfbf; border-style: solid; padding-left: 4px; }"
	".AccessLabel-privateblock { margin-left: -8px; border-width: 0 0 0 4px; border-color: #ffbfbf; border-style: solid; padding-left: 4px; }"
	".AccessLabel-public { font-weight: bold; text-shadow: -1px -1px 0px #dedebb; }"
	".AccessLabel-protected { font-weight: bold; text-shadow: -1px -1px 0px #ffddbb; }"
	".AccessLabel-private { font-weight: bold; text-shadow: -1px -1px 0px #ffbbbb; }"
	".Literal { color: black; }"
	".TypeEntity { color: black; font-weight: bold; text-shadow: -1px -1px 0px #888; }"
	".SimpleType { text-shadow: -1px -1px 0px #fb0; }"
	".FunctionType { text-shadow: -1px -1px 0px #84f; }"
	".HashType { text-shadow: -1px -1px 0px #8f4; }"
	".Operation { background-color: rgba(0, 0, 0, 0.03); padding: 0 2px 0 2px; }"
	".Statement { background-color: transparent; padding: 0; }"
);

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

String WebViewable::toHtml(Entity const* _e, String const& _tag)
{
	return WebStylist::current()->toHtml(_e, _tag);
}

String WebViewable::toHtml(List<Entity const*> const& _es, String const& _delimiter, String const& _tag)
{
	String ret;
	bool first = true;
	foreach (Entity const* e, _es)
	{
		if (first)
			first = false;
		else
			ret += _delimiter;
		ret += toHtml(e, _tag);
	}
	return ret;
}

}
