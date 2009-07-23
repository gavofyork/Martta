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

#include "IdentifierSet.h"
#include "ModelPtr.h"
#include "ValueDefiner.h"
#include "Typed.h"

namespace Martta
{

class Referenced: public Typed
{
	MARTTA_PLACEHOLDER(Typed)

public:
	Referenced(ValueDefiner* _v = 0);

	virtual String						code() const;
	virtual Type						type() const;
	ModelPtr<ValueDefiner>				subject() const { return m_subject; }
	void								setSubject(ValueDefiner* _e) { setDependency(m_subject, _e); }
	ModelPtr<ValueDefiner>				get() const { return m_subject; }
	void								set(ValueDefiner* _e) { setDependency(m_subject, _e); }
	virtual String						defineEditLayout(ViewKeys const&, ValueDefiner* _v);
	virtual List<ValueDefiner*>			possibilities() const { return List<ValueDefiner*>(); }

protected:
	virtual bool						isInValidState() const;
	virtual String						defineLayout(ViewKeys const&) const;
//	virtual void						decorate(DecorationContext const& _c) const;
	virtual bool						isSuperfluous() const;
	virtual void						apresLoad() { addDependency(m_subject->self()); Super::apresLoad(); }
	virtual void						onDependencyChanged(Entity*) {  if (m_subject) changed(); }
	virtual void						onDependencySwitched(Entity* _t, Entity* _old);
	virtual void						onDependencyRemoved(Entity* _old, int);
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"subject"] = m_subject.key(); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_subject.restoreFrom(_p[L"subject"]); }
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);

	ModelPtr<ValueDefiner>				m_subject;
};

template<class T>
class ReferencedValueSet: public IdentifierSet
{
public:
	virtual List<Named*>				identifiableAt(Position const& _p) { return castEntities<Named>(T::possibilities(_p)); }
	virtual void						acceptAt(Position const& _p, Named* _i) { ValueDefiner* v = _i->asKind<ValueDefiner>(); _p.place(new T(v)); }
};

}