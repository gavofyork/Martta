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

#include <msRgb.h>
#include <msString.h>
#include <msStringList.h>
#include <msList.h>
using namespace MarttaSupport;

#include "Concept.h"

#ifndef M_API_WebView
#define M_API_WebView M_OUTAPI
#endif

namespace Martta
{

class CodeScene;

class M_CLASS M_API_WebView WebViewable: public_interface Concept
{
	MARTTA_NOTION(Concept)

public:
	virtual ~WebViewable() {}

	virtual String							defineHtml() const { return String::null; }
	virtual String							defineEditHtml(CodeScene* _cs) const;

	static String							htmlEscape(String const& _s);
	inline static String					br() { return L"<br/>"; }
	inline static String					mark() { return L"<^>"; }
	template<String const& Tag>
	inline static String					tagOf(String const& _classes, String const& _mid) { return L"<" + Tag + " class=\"" + _classes + "\">" + _mid + L"</" + Tag.section(L' ', 0, 0) + L">"; }
	inline static String					tagOf(String const& _classes, String const& _mid, String const& _tag = L"span") { return L"<" + _tag + " class=\"" + _classes + "\">" + _mid + L"</" + _tag.section(L' ', 0, 0) + L">"; }
	static String							cssBorder(String const& _name, Rgb _col);

	static String							composeName(String const& _id, StringList const& _flags);
	String									composedName(String const& _id) const { return composeName(_id, kind().realSupers().names().reversed()); }

protected:
	static String							toHtml(Concept const* _e, String const& _tag = L"");
	static String							toHtml(List<Concept const*> const& _es, String const& _delimiter = L" ", String const& _tag = L"", bool _delimiterOnLast = false);
	inline static String					toHtml(List<Concept*> const& _es, String const& _delimiter = L" ", String const& _tag = L"", bool _delimiterOnLast = false) { return toHtml(list_const_cast<Concept const*>(_es), _delimiter, _tag, _delimiterOnLast); }
};

#define MARTTA_REGISTER_CSS(ConceptClass, CSS) \
	static CssRegisterer s_css_ ## ConceptClass ## __LINE__ (ConceptClass::staticAuxilliary(), CSS)

class M_API_WebView CssRegisterer
{
public:
	CssRegisterer(AuxilliaryFace const* _f, String const& _css);
	~CssRegisterer();

private:
	AuxilliaryFace const* m_f;
};

}
