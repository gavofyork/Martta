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

#include "Concept.h"
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

String WebStylist::toHtml(Concept const* _e, String const& _tag)
{
	KeepCurrent k(this);
	String t = _tag;
	if (t.section(L' ', 0, 0) == L"div")
		t = t.section(L' ', 1);
	else
	{
		if (t.section(L' ', 0, 0) == L"span")
			t = t.section(L' ', 1);
		int i = t.indexOf("class=\"");
		if (i != -1)
			t.insert(i + 7, L"layout ");
		else
			t += " class=\"layout\"";
	}

	if (!_e)
		return String::null;
	if (m_htmlCache.contains(_e))
		return m_htmlCache[_e];
	return String("<div %1 entity=\"true\" ondblclick=\"if (CodeView.attemptEdit(%2)) event.stopPropagation();\" id=\"%2\">%3</div>").arg(t).arg((int)_e).arg(refineHtml(defineHtml(_e), !_e->isUsurped()))/*.arg(_tag.section(L' ', 0, 0))*/;
}

String WebStylist::rejiggedHtml(Concept const* _e)
{
	KeepCurrent k(this);
	String ret = refineHtml(defineHtml(_e), !_e->isUsurped());
	m_htmlCache.clear();
	return ret;
}

String WebStylist::editHtml(Concept const* _e, CodeScene* _cs)
{
	KeepCurrent k(this);
	return refineHtml(defineEditHtml(_e, _cs), true, true, L"editing");
}

String WebStylist::refineHtml(String const& _html, bool _allowThis, bool _forceThis, String const&)
{
	String ret = _html;
	if (!ret.contains(L"<^>") && _forceThis)
		ret = L"<span id=\"this\"></span>" + ret;
	else
		ret.replace(L"<^>", _allowThis ? L"<span id=\"this\"></span>" : L"");
	int i;
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

StringList analyse(String const& _fb)
{
	String lfb = _fb.toLower();
	int fi = lfb.indexOf(L"foo");
	int bi = lfb.indexOf(L"bar");
	if (fi < 0 || bi < fi + 3)
		return StringList(L"", L"", L"", L"");
	bool fIsLow = _fb[fi].isLower();
	bool oIsLow = _fb[fi + 1].isLower();
	bool bIsLow = _fb[bi].isLower();
	String pre = _fb.left(fi);
	String mid = _fb.mid(fi + 3, bi - (fi + 3));
	String post = _fb.mid(bi + 3);
	return StringList(pre + (fIsLow ? L"\a" : L"\b"), oIsLow ? L"\a" : L"\b", mid + (bIsLow ? L"\a" : L"\b"), post);
}

void WebStylist::setProperties(Hash<String, String> const& _p)
{
	foreach (String s, _p.keys())
		if (s.startsWith(L"Id-") || s == L"Id")
		{
			StringList a = analyse(_p[s]);
			m_properties[s + "-pre"] = a[0];
			m_properties[s + "-norm"] = a[1];
			m_properties[s + "-break"] = a[2];
			m_properties[s + "-post"] = a[3];
		}
		else
		{
			if (s == L"CSS-Simple")
			{
				m_css = _p[s].toBool() ?
					"body { color: black; font-size: 12px; font-family: Mono; background-color: rgba(255, 255, 255, 1); }"
					".layout { display: inline-block; }"
					".keyword { font-weight: bold; }"
					"._ms_unreal { color: #888; }"
					"._ms_minor { color: #888; font-size: 10px; font-family: Sans; }"
					".symbol { font-weight: bold; }"
					".block { display: block; position: relative; margin-left: 20px; }"
					".deblock { display: block; position: relative; margin-left: -20px; }"
				//	".editing { background-color: rgba(255, 0, 0, 0.25); }"
					".badge { margin: 0px 0px; padding: 0px 0.3em; border-top: 1px solid rgba(0,0,0,0.4); border-left: 1px solid rgba(0,0,0,0.2); border-bottom: 1px solid rgba(255,255,255,0.4); border-right: 1px solid rgba(255,255,255,0.2); background-image: -webkit-gradient(linear, left top, left bottom, from(rgba(0,0,0,0.2)), to(rgba(255,255,255,0.2))); font-weight: 900; color: rgba(255,255,255,0.85); text-shadow: 1px 1px 2px rgba(0,0,0,0.3); }"
				:
					"body { color: #666; font-size: 12px; font-family: Sans; background-color: rgba(255, 255, 255, 1); }"
					".layout { display: inline-block; }"
					".keyword { font-weight: bold; }"
					"._ms_unreal { color: #888; }"
					"._ms_minor { color: #888; font-size: 8px; }"
					".unreal { color: #888; }"
					".minor { color: #888; font-size: 8px; }"
					".symbol { font-weight: bold; }"
					".block { display: block; position: relative; margin-left: 20px; }"
					".deblock { display: block; position: relative; margin-left: -20px; }"
				//	".editing { background-color: rgba(255, 0, 0, 0.25); }"
					".badge { margin: 0px 0px; padding: 0px 0.3em; border-top: 1px solid rgba(0,0,0,0.4); border-left: 1px solid rgba(0,0,0,0.2); border-bottom: 1px solid rgba(255,255,255,0.4); border-right: 1px solid rgba(255,255,255,0.2); background-image: -webkit-gradient(linear, left top, left bottom, from(rgba(0,0,0,0.2)), to(rgba(255,255,255,0.2))); font-weight: 900; color: rgba(255,255,255,0.85); text-shadow: 1px 1px 2px rgba(0,0,0,0.3); }";
			}
			m_properties[s] = _p[s];
		}

	m_htmlCache.clear();
}


String WebStylist::defineCss() const
{
	if (m_properties[L"CSS-Simple"].toBool())
		return m_css;
	else
		return m_css + WebStylistRegistrar::get()->css();
}

String WebStylist::composeName(String const& _id, StringList const& _flags) const
{
	String flag;
	foreach (flag, _flags)
		if (m_properties.contains("Id-" + flag + "-norm"))
			break;

	String ret;
	bool upperCaseMode = false;
	bool onBreak = false;
	for (int i = 0; i <= _id.length(); i++)
	{
		if (_id[i].isSpace())
		{
			onBreak = true;
			continue;
		}
		String tag = (i == 0) ? L"pre" : (i == _id.length()) ? L"post" : onBreak ? L"break" : L"norm";
		onBreak = false;
		String s = m_properties.value("Id-" + flag + "-" + tag);
		for (int j = 0; j < s.length(); j++)
			if (s[j] == '\a')
				upperCaseMode = false;
			else if (s[j] == '\b')
				upperCaseMode = true;
			else
				ret += s[j];
		if (i < _id.length())
			ret += upperCaseMode ? _id[i].toUpper() : _id[i].toLower();
	}
	mInfo() << _flags << ":" << flag << _id << ret;
	return ret;
}

String WebStylist::defineHtml(Concept const* _e)
{
	String ret;
	if (WebViewable const* v = _e->tryKind<WebViewable>())
		ret = v->defineHtml();
	else
	{
		ret = L"<span class=\"keyword\">" + _e->kind().name() + L"</span><span class=\"minor\">";
		Hash<String, String> p;
		_e->properties(p);
		foreach (String s, p.keys())
			ret += L"[" + s + L"=" + p[s] + L"]";
		ret += L"</span><span>{";
		foreach (Concept* e, _e->children())
			 ret += L" " + toHtml(e) + L"@" + e->indexName();
		ret += L" }</span>";
	}

	return ret;
}

String WebStylist::defineEditHtml(Concept const* _e, CodeScene* _cs)
{
	if (WebViewable const* v = _e->tryKind<WebViewable>())
		return v->defineEditHtml(_cs);
	return String::null;
}

}

/*
  <StringLiteral>
  <here/><span class="symbol">&ldquo;</span><span class="StringLiteral Literal"><deref attrib="value"/></span><span class="symbol">&rdquo;</span>;
  </StringLiteral>
  <ReferencedType>
  <here/><redirect attrib="subject" repot="TypeConcept"><typed>&empty;</typed></reuse>;
  </ReferencedType>
*/
