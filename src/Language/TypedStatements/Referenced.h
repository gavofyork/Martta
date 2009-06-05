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

#include "Typed.h"
#include "ValueDefinition.h"

namespace Martta
{

class ReferencedEdit;

class Referenced: public Typed
{
	MARTTA_OBJECT(Typed)

	friend class ReferencedEdit;

public:
	Referenced(ValueDefinition* _v = 0, bool _specific = false);

	virtual QString						code() const;
	virtual Type						type() const;
	ModelPtr<ValueDefinition>			subject() const { return m_subject; }
	void								setSubject(ValueDefinition* _e) { SET_DEPENDENCY(m_subject, _e); }
	
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);

protected:
	virtual bool						onChanged();
	virtual QString						defineLayout(ViewKeys&) const;
	virtual void						decorate(DecorationContext const& _c) const;
	virtual bool						isSuperfluous() const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	virtual void						apresLoad() { addDependency(m_subject); Super::apresLoad(); }
	virtual Kinds						ancestralDependencies() const;
	virtual void						onDependencyChanged(Entity*) { changed(); }
	
	ModelPtr<ValueDefinition>			m_subject;
	bool								m_specific;
	uint								m_lastSet;
};

}
