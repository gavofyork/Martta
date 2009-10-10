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

#include <msString.h>
using namespace MarttaSupport;

#include "Concept.h"

#ifndef M_API_WebView
#define M_API_WebView M_OUTAPI
#endif

namespace Martta
{

class M_API_WebView WebInformer: public_interface Concept
{
	MARTTA_NOTION(Concept)

public:
	virtual ~WebInformer() {}

	static String escape(String const& _s)
	{
		String ret = _s;
		return ret.replace(L'&', L"&amp;").replace(L'>', L"&gt;").replace(L'<', L"&lt;").replace(L'"', L"&quot;");
	}

	struct Pairs
	{
		inline Pairs(String const& _title, bool _autoEscape = false): m_title(_title), m_inRow(false), m_escape(_autoEscape) {}
		inline Pairs& operator<<(String const& _f)
		{
			String s = m_escape ? escape(_f) : _f;
			if (m_inRow)
				m_ret += L"<td>" + s + "</td></tr>";
			else
				m_ret += L"<tr><th>" + s + "</th>";
			m_inRow = !m_inRow;
			return *this;
		}
		inline Pairs& operator<<(Pairs const& _p)
		{
			if (m_inRow)
				m_ret += L"<td></td></tr>";
			String add = _p.m_ret;
			add.replace(L"<th>", L"<th><span style=\"padding-left:1em\"/>");
			add.replace(L"<th colspan=\"2\">", L"<th colspan=\"2\"><span style=\"padding-left:1em\"/>");
			m_ret += L"<tr><th colspan=\"2\"><span class=\"sh\">" + _p.m_title + L"</span></th></tr>" + add;
			m_inRow = _p.m_inRow;
			return *this;
		}
		inline String html() const
		{
			return (L"<h2 onclick=\"toggle('%1', this)\">%1</h2><table id=\"%1\">" + m_ret + L"</table>").arg(escape(m_title));
		}
		inline operator String() const
		{
			return html();
		}

	private:
		String m_ret;
		String m_title;
		bool m_inRow;
		bool m_escape;
		bool m_inPairs;
	};

	static String					compileKinds(Kinds const& _ks);

	static String					htmlFromTree(String const& _tree);
	static String					basicInformationHtml(Concept const* _e);
	virtual String					informationHtml() const { return String::null; }
	virtual String					superChildInformationHtml(Concept const*) const { return String::null; }
};

}
