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

#include "ModelPtr.h"
#include "VirtualMethod.h"

namespace Martta
{

inline String stripId(String const& _html)
{
	return String(_html).replace(L"id=", L"iXd=");
}

class VirtualOverload: public VirtualMethod
{
	MARTTA_OBJECT(VirtualMethod)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<VirtualOverload>(_p, _e, "L"); }

	VirtualMethod*						get() const { return m_base; }
	void								set(VirtualMethod* _m) { setDependency(m_base, _m); }
	List<VirtualMethod*>				possibilities() const;

protected:
	virtual int							minRequired(int _i) const { return _i == Identity || _i == Constness || _i == Returned ? 0 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual String						name() const { return m_base.isUsable() ? m_base->name() : String::null; }
	virtual String						codeName() const { return m_base ? m_base->codeName() : String::null; }
	virtual bool						isInValidState() const { return m_base.isUsable(); }
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);

	virtual String						defineNameHtml() const;
	virtual String						defineReturnHtml() const;
	virtual String						defineArgListHtml() const;

	virtual String						defineEnclosureHtml(String const& _part, String const& _middle) const { return Super::defineEnclosureHtml(_part, (_part == L"head" ? L"<span class=\"minor\">OVERRIDE</span> " : L"") + _middle); }
	virtual String						defineEditHtml(CodeScene*) const;

	virtual void						apresLoad() { if (m_base) set(m_base); Super::apresLoad(); }

	virtual inline int					argumentCount() const { return m_base.isUsable() ? m_base->argumentCount() : 0; }
	virtual inline Argument*			argument(int _index) const { AssertNR(_index < argumentCount()); return m_base.isUsable() ? m_base->argument(_index) : 0; }
	virtual Type						returns() const;
	virtual inline bool					isConst() const { return m_base.isUsable() ? m_base->isConst() : false; }

	virtual void						onDependencyChanged(int _a, Entity* _e) { if (_e->tryKind<VirtualMethod>() == m_base) changed(); else return Super::onDependencyChanged(_a, _e); }

	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"base"] = m_base.key(); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_base.restoreFrom(_p[L"base"]); }

private:
	ModelPtr<VirtualMethod>				m_base;
};

}
