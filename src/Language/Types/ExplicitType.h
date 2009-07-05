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
#include "TypeEntity.h"

namespace Martta
{

class ExplicitType: public TypeEntity
{
	MARTTA_OBJECT(TypeEntity)

	friend class TypeDefinition;

public:
	ExplicitType(TypeDefinition* _subject = 0): m_subject(0) { setSubject(_subject); }

	// QUICK workout which is better and eliminate other.
	TypeDefinition*						subject() const { return m_subject; }
	void								setSubject(TypeDefinition* _subject = 0) { setDependency(m_subject, _subject); }
	TypeDefinition*						get() const { return m_subject; }
	void								set(TypeDefinition* _m) { setSubject(_m); }
	
	List<TypeDefinition*>				possibilities();
	virtual String						defineEditLayout(ViewKeys const&, TypeDefinition*) const;
	
	bool								haveSingleCastOperator(TypeEntity const* _t, bool _const = false) const;
	bool								haveSingleConversionConstructor(TypeEntity const* _f) const;

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);
	
	virtual bool						isNull() const { return !m_subject.isUsable(); }
	
protected:
	virtual bool						hasDefaultConstructor() const;
	virtual Types						assignableTypes() const;
	virtual List<ValueDefiner*>			applicableMembers(Entity* _s = 0, bool _isConst = false) const;
	virtual String						code(String const& _middle) const { return (m_subject ? m_subject->asKind<Identifiable>()->reference() : "") + _middle; }
	virtual bool						contentsEquivalentTo(TypeEntity const* _t) const { return _t->asKind<ExplicitType>()->m_subject == m_subject; }
	virtual Rgb							idColour() const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual TypeEntity*					newClone() const { return new ExplicitType(m_subject); }
	virtual String						defineLayout(ViewKeys const&) const;
	virtual bool						keyPressed(KeyEvent const* _e);
//	virtual bool						isSuperfluous() const;
	virtual bool						canStandAlone() const;
	virtual bool						defineSimilarityTo(TypeEntity const* _t, Castability _c) const;
	virtual bool						defineSimilarityFrom(TypeEntity const* _from, Castability _c) const;
	virtual void						apresLoad() { addDependency(m_subject->self()); Super::apresLoad(); }
	virtual List<Declaration*>			utilised() const;
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"subject"] = m_subject.key(); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_subject.restoreFrom(_p[L"subject"]); }
	
	ModelPtr<TypeDefinition>			m_subject;
};

}
