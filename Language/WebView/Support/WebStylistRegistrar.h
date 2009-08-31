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
#include <msStringList.h>
#include <msList.h>
#include <msHash.h>
using namespace MarttaSupport;

#ifndef M_API_WebView
#define M_API_WebView M_OUTAPI
#endif

namespace Martta
{

class AuxilliaryFace;

class M_API_WebView WebStylistRegistrar
{
public:
	WebStylistRegistrar(): m_changed(false) {}

	inline static WebStylistRegistrar*	get() { if (!s_this) s_this = new WebStylistRegistrar; return s_this; }

	void								registerCss(AuxilliaryFace const* _f, String const& _css) { m_css[_f] = _css; m_changed = true; }
	void								unregisterCss(AuxilliaryFace const* _f) { m_css.remove(_f); m_changed = true; }
	template<class T> void				registerCss(String const& _css) { m_css[T::staticKind.auxilliary()] = _css; m_changed = true; }
	template<class T> void				unregisterCss() { m_css.remove(T::staticKind.auxilliary()); m_changed = true; }

	bool								hasChanged() const { return m_changed; }
	String								css() const { m_changed = false; return StringList(m_css.values()).join(L' '); }
	Hash<AuxilliaryFace const*, String> const& registered() const { return m_css; }

private:
	static WebStylistRegistrar*			s_this;										///< Us.

	Hash<AuxilliaryFace const*, String> m_css;										///< All our CSS elements.
	mutable bool						m_changed;
};

}

