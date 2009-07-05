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
#include "ValueDefiner.h"
#include "Typed.h"

namespace Martta
{

class ReferencedEdit;

class Referenced: public Typed
{
	MARTTA_OBJECT(Typed)

	friend class ReferencedEdit;

public:
	Referenced(ValueDefiner* _v = 0, bool _specific = false);

	virtual String						code() const;
	virtual Type						type() const;
	ModelPtr<ValueDefiner>				subject() const { return m_subject; }
	void								setSubject(ValueDefiner* _e) { setDependency(m_subject, _e); }
	
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual bool						isInValidState() const;
	virtual String						defineLayout(ViewKeys const&) const;
//	virtual void						decorate(DecorationContext const& _c) const;
	virtual bool						isSuperfluous() const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual void						apresLoad() { addDependency(m_subject->self()); Super::apresLoad(); }
	virtual Kinds						ancestralDependencies() const;
	virtual void						onDependencyChanged(Entity* _e) {  debugTree(); mDebug() << _e; mDebug() << &*m_subject; if (m_subject) changed(); }
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"subject"] = m_subject.key(); _p[L"specific"] = String::number(m_specific); _p[L"lastSet"] = String::number(m_lastSet); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_subject.restoreFrom(_p[L"subject"]); m_specific = _p[L"specific"].toBool(); m_lastSet = _p[L"lastSet"].toInt(); }
	
	ModelPtr<ValueDefiner>				m_subject;
	bool								m_specific;
	uint								m_lastSet;
};

}
