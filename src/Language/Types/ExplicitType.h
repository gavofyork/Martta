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
#include "TypeEntity.h"

namespace Martta
{

class ExplicitType: public TypeEntity
{
	MARTTA_OBJECT(TypeEntity)

	friend class TypeDefinition;

public:
	ExplicitType(TypeDefinition* _subject = 0): m_subject(0) { setSubject(_subject); }

	TypeDefinition*						subject() const { return m_subject; }
	void								setSubject(TypeDefinition* _subject = 0) { SET_DEPENDENCY(m_subject, _subject); }
	
	QList<TypeDefinition*>				possibilities();
	virtual QString						defineEditLayout(ViewKeys&, TypeDefinition*) const;
	TypeDefinition*						get() const { return m_subject; }
	void								set(TypeDefinition* _m) { setSubject(_m); }
	
	bool								haveSingleCastOperator(TypeEntity const* _t, bool _const = false) const;
	bool								haveSingleConversionConstructor(TypeEntity const* _f) const;

	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);
	
protected:
	virtual bool						hasDefaultConstructor() const;
	virtual Types						assignableTypes() const;
	virtual QList<ValueDefinition*>		applicableMembers(Entity* _s = 0, bool _isConst = false) const;
	virtual QString						code(QString const& _middle) const { return (m_subject ? m_subject->reference() : "") + _middle; }
	virtual bool						contentsEquivalentTo(TypeEntity const* _t) const { return _t->asKind<ExplicitType>()->m_subject == m_subject; }
	virtual QString						idColour() const;
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual TypeEntity*					newClone() const { return new ExplicitType(m_subject); }
	virtual QString						defineLayout(ViewKeys&) const;
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual bool						isSuperfluous() const;
	virtual bool						canStandAlone() const;
	virtual bool						defineSimilarityTo(TypeEntity const* _t, Castability _c) const;
	virtual bool						defineSimilarityFrom(TypeEntity const* _from, Castability _c) const;
	virtual void						apresLoad() { addDependency(m_subject); Super::apresLoad(); }
	
	ModelPtr<TypeDefinition>			m_subject;
};

}
