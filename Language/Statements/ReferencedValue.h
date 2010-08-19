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

#ifndef M_API_Statements
#define M_API_Statements M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Statements ReferencedValue: public_super Typed, public_interface NameEntryPoint
{
	MARTTA_PLACEHOLDER(Typed)
	MARTTA_ALSO_INHERITS(NameEntryPoint, 0)

public:
	ReferencedValue(ValueDefiner* _v = 0);

	ModelPtr<ValueDefiner>				subject() const { return m_subject; }
	ModelPtr<ValueDefiner>				get() const { return m_subject; }
	void								set(int) { setDependency(m_subject, (ValueDefiner*)0); }
	void								set(ValueDefiner* _e) { setDependency(m_subject, _e); }
	void								set(Named* _n) { set(static_cast<Identifiable*>(_n)->asKind<ValueDefiner>()); }
	void								set(ModelPtr<ValueDefiner> const& _e) { set(&*_e); }

//	virtual List<Named*>				possibilities() const { return list_cast<Named*>(possibilities(over(), true)); }

	String								editHtmlHelper(ValueDefiner* _v, String const& _mid) const;
	virtual List<ValueDefiner*>			refPossibilities(Position const&) const { return List<ValueDefiner*>(); }

	virtual Type						type() const;
	virtual Type						bareType() const;

protected:
	virtual String						code() const;
	virtual bool						isInValidState() const;
	virtual String						defineHtml() const;
	virtual String						defineEditHtml(CodeScene* _cs) const;
	virtual bool						isSuperfluous() const;
	virtual void						apresLoad() { if (m_subject) addDependency(m_subject->self()); Super::apresLoad(); }
	virtual void						onDependencyChanged(int _a, Concept* _d) { if (_d->tryKind<ValueDefiner>() == m_subject) changed(_a); Super::onDependencyChanged(_a, _d); }
	virtual void						onDependencySwitched(Concept* _t, Concept* _old);
	virtual void						onDependencyRemoved(Concept* _old, int);
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"subject"] = m_subject.key(); _p[L"hopeful"] = m_hopeful; }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_subject.restoreFrom(_p[L"subject"]); m_hopeful = _p[L"hopeful"]; }
	virtual EditDelegateFace*			newDelegate(CodeScene* _s) { return NameEntryPoint::newDelegate<ReferencedValue>(_s); }
	virtual bool						keyPressed(KeyEvent const* _e)
	{
		if (NameEntryPoint::keyPressed(_e))
			return true;
		return Super::keyPressed(_e);
	}

	String								m_hopeful;
	ModelPtr<ValueDefiner>				m_subject;
};

template<class T>
class ReferencedValueSet: public_super IdentifierSet
{
public:
	virtual String						setId() const { return Kind::of<T>().name(); }
	virtual List<Named*>				identifiableAt(Position const& _p) { if (_p.allowedToBeKind<T>()) return list_cast<Named*>(concepts_cast<Identifiable>(T::staticRefPossibilities(_p))); return List<Named*>(); }
	virtual void						acceptAt(Position const& _p, Named* _i, CodeScene* _cs) { ValueDefiner* v = static_cast<Identifiable*>(_i)->asKind<ValueDefiner>(); _cs->setCurrent(_p.place(new T(v))); }
	virtual String						defineEditHtml(Named* _i, String const& _mid) { ValueDefiner* v = static_cast<Identifiable*>(_i)->asKind<ValueDefiner>(); return ReferencedValue().editHtmlHelper(v, _mid); }
};

class M_CLASS M_API_Statements ScopedReferencedValue: public_super ReferencedValue
{
	MARTTA_PLACEHOLDER(ReferencedValue)

public:
	ScopedReferencedValue() {}
	ScopedReferencedValue(ValueDefiner* _v) { set(_v); }
};

}
