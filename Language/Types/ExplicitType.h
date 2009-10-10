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

#include "TypeDefinition.h"
#include "ModelPtr.h"
#include "Type.h"
#include "PhysicalType.h"

#ifndef M_API_Types
#define M_API_Types M_OUTAPI
#endif

namespace Martta
{

class ExplicitTypeSet;

class M_API_Types ExplicitType: public_super PhysicalType
{
	MARTTA_PROPER(PhysicalType)

	friend class TypeDefinition;
	friend class ExplicitTypeSet;

public:
	ExplicitType(TypeDefinition* _subject = 0): m_subject(0) { set(_subject); }

	TypeDefinition*						subject() const { return m_subject; }
	TypeDefinition*						get() const { return m_subject; }
	void								set(TypeDefinition* _subject) { setDependency(m_subject, _subject); }

	List<TypeDefinition*>				possibilities() const;

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual inline bool					hasDefaultConstructor() const { return m_subject ? m_subject->hasDefaultConstructor() : false; }
	virtual inline Types				assignableTypes() const { return m_subject ? m_subject->assignableTypes() : Types(); }
	virtual inline List<ValueDefiner*>	applicableMembers(Concept const* _s = 0, bool _isConst = false) const { return m_subject ? m_subject->applicableMembers(_s, _isConst) : List<ValueDefiner*>(); }
	virtual inline bool					canStandAlone() const { return m_subject && m_subject->canStandAlone(); }
	virtual inline bool					isCastableTo(TypeConcept const* _t, bool _const) { return m_subject && m_subject->hasSingleCastOperator(_t, _const); }
	virtual inline TypeConcept*			newClone() const { return new ExplicitType(m_subject); }

	virtual inline bool					contentsEquivalentTo(TypeConcept const* _t) const { if (ExplicitType const* e = _t->tryKind<ExplicitType>()) return e->m_subject == m_subject; return false; }
	virtual inline bool					defineSimilarityTo(TypeConcept const* _t, Castability _c) const { return (m_subject && m_subject->defineSimilarityTo(_t, _c)) || Super::defineSimilarityTo(_t, _c); }
	virtual inline bool					defineSimilarityFrom(TypeConcept const* _f, Castability _c) const { return (m_subject && _c == Convertible && m_subject->hasSingleConversionConstructor(_f)) || Super::defineSimilarityFrom(_f, _c); }

	virtual inline bool					isNull() const { return !m_subject.isUsable(); }

	virtual inline String				code(String const& _middle) const { return (m_subject ? m_subject->asKind<Identifiable>()->reference() : "") + _middle; }
	virtual inline List<Declaration*>	utilised() const { return m_subject ? m_subject->utilisedInUse() : Super::utilised(); }	// TODO: define for other types.

	virtual inline Rgb					idColour() const { return m_subject ? m_subject->idColour() : Rgb(0x444444); }
	virtual String						defineHtml() const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual String						defineEditHtml(CodeScene* _cs) const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
//	virtual bool						isSuperfluous() const;

	virtual void						apresLoad();
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"subject"] = m_subject.key(); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_subject.restoreFrom(_p[L"subject"]); }

	virtual void						onDependencyRemoved(Concept* _s, int) { if (_s->tryKind<TypeDefinition>() == m_subject) set(0); }
	virtual void						onDependencySwitched(Concept* _s, Concept* _o) { if (_o->tryKind<TypeDefinition>() == m_subject) set(_s->tryKind<TypeDefinition>()); }
	virtual void						onDependencyChanged(int, Concept* _s) { if (_s->tryKind<TypeDefinition>() == m_subject) changed(); }

	ModelPtr<TypeDefinition>			m_subject;
};

// S is a TypeDefinition;
#ifndef _MSC_VER
template<>
#endif
template<class S>
struct TypeConstructor<S*>
{
	static inline TypeDefinition* typeComb(S* _x) { return _x; }
	static inline void construct(Type* _this, S* _subject) { _this->m_top = new ExplicitType(_subject); _this->m_top->setOwner(_this); }
};

}
