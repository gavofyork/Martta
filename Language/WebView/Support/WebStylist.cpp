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

#include "Entity.h"
#include "CodeScene.h"
#include "WebStylistRegistrar.h"
#include "WebViewable.h"
#include "WebStylist.h"

namespace Martta
{

WebStylist* WebStylist::s_current = 0;

class KeepCurrent
{
public:
	KeepCurrent(WebStylist* _s): m_s(_s->s_current ? 0 : _s) { if (m_s) m_s->s_current = m_s; }
	~KeepCurrent() { if (m_s) m_s->s_current = 0; }

private:
	WebStylist* m_s;
};

String WebStylist::toHtml(Entity const* _e, String const& _tag)
{
	KeepCurrent k(this);
	if (!_e)
		return String::null;
	if (m_htmlCache.contains(_e))
		return m_htmlCache[_e];
	return String("<%1 entity=\"true\" ondblclick=\"if (CodeView.attemptEdit(%2)) event.stopPropagation();\" id=\"%2\">%3</%4>").arg(_tag).arg((int)_e).arg(refineHtml(defineHtml(_e), !_e->isUsurped())).arg(_tag.section(L' ', 0, 0));
}

String WebStylist::rejiggedHtml(Entity const* _e)
{
	KeepCurrent k(this);
	String ret = refineHtml(defineHtml(_e), !_e->isUsurped());
	m_htmlCache.clear();
	return ret;
}

String WebStylist::editHtml(Entity const* _e, CodeScene* _cs)
{
	KeepCurrent k(this);
	return refineHtml(defineEditHtml(_e, _cs), true);
}

String WebStylist::refineHtml(String const& _html, bool _allowThis)
{
	String ret = _html;
	ret.replace(L"<^>", _allowThis ? L"<span id=\"this\"></span>" : L"");
	int i;
	if ((i = ret.indexOf(L"<^")) > -1)
	{
		int t = min((uint)ret.indexOf(L' ', i), (uint)ret.indexOf(L'>', i));
		// abc<^div >
		// 0123456789
		//    i    t
		if (t != -1 && _allowThis)
			ret.replace(t, 0, L" id=\"this\"");
		ret.replace(i + 1, 1, String::null);
	}
	while ((i = ret.indexOf(L"=\"data://")) != -1)
	{
		int t = ret.indexOf(L'\"', i+6);
		if (t == -1)
			break;
		// abc<@xyz>d
		// 0123456789
		//    i ---t
		// i+2, t-i-2
		// i, t-i+1
		ret.replace(i+2, t-i-2, L"file://" + DataFinder::get()->fullPathOf(ret.mid(i+9, t-i-9)));
	}
	return ret;
}

String WebStylist::defineHtml(Entity const* _e)
{
	if (WebViewable const* v = _e->tryKind<WebViewable>())
		return v->defineHtml();

	String ret = L"<span class=\"keyword\">" + _e->kind().name() + L"</span><span class=\"minor\">";
	Hash<String, String> p;
	_e->properties(p);
	foreach (String s, p.keys())
		ret += L"[" + s + L"=" + p[s] + L"]";
	ret += L"</span><span>{";
	foreach (Entity* e, _e->children())
		 ret += L" " + toHtml(e) + L"@" + e->indexName();
	ret += L" }</span>";

	return ret;
}

String WebStylist::defineEditHtml(Entity const* _e, CodeScene* _cs)
{
	String ret;
	if (WebViewable const* v = _e->tryKind<WebViewable>())
		ret = v->defineEditHtml(_cs);
	return ret;
}

}
