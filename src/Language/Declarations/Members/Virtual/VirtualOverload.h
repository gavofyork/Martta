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

class VirtualOverload: public VirtualMethod
{
	MARTTA_OBJECT(VirtualMethod)
	
public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e) { return simpleInsertionPointKeyPressHandler<VirtualOverload>(_p, _e, "L"); }
	
	VirtualMethod*						get() const { return m_base; }
	void								set(VirtualMethod* _m) { setDependency(m_base, _m); }
	QString								defineEditLayout(ViewKeys&, VirtualMethod*) const;
	QList<VirtualMethod*>				possibilities() const;
	
protected:
	virtual int							memberMinimumRequired() const { return 1; }
	virtual Kinds						memberAllowedKinds(int _i) const;
	virtual QString						memberLambdaDefineLayout(ViewKeys&) const;
	virtual QString						name() const { return m_base.isUsable() ? m_base->name() : QString(); }
	virtual QString						codeName() const { return m_base ? m_base->codeName() : QString(); }
	virtual bool						isInValidState() const { return m_base.isUsable(); }
	virtual bool						keyPressed(EntityKeyEvent const* _e);
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	
	virtual void						apresLoad() { M_ASSERT(m_base.isUsable()); addDependency(m_base); Super::apresLoad(); }
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);

	virtual inline int					argumentCount() const { return m_base.isUsable() ? m_base->argumentCount() : 0; }
	virtual inline Argument*			argument(int _index) const { M_ASSERT(_index < argumentCount()); return m_base.isUsable() ? m_base->argument(_index) : 0; }
	virtual Type						returns() const;
	virtual inline bool					isConst() const { return m_base.isUsable() ? m_base->isConst() : false; }
	
private:
	ModelPtr<VirtualMethod>				m_base;
};

}