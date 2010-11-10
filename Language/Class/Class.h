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

#include "Constructor.h"
#include "AccessLabel.h"
#include "TopLevelType.h"

#ifndef M_API_Class
#define M_API_Class M_OUTAPI
#endif

namespace Martta
{

class Member;


class M_CLASS M_API_Members InitialiserItem: public_super Concept
{
	MARTTA_PROPER(Concept)

public:
	MARTTA_NAMED(TheField)
	MARTTA_NAMED(Value)

	virtual String						code() const;

protected:
	virtual int							minRequired(int _i) const { return _i == TheField || _i == Value ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
};

class M_CLASS M_API_Members InitialiserList: public_super ConstructionHelper
{
	MARTTA_PROPER(ConstructionHelper)

protected:
	virtual int							minRequired(int) const { return 0; }
	virtual Kinds						allowedKinds(int _i) const { return _i >= 0 ? Kind::of<InitialiserItem>() : Kinds(); }
	virtual String						code() const;
};

class M_CLASS M_API_Class Class: public_super TopLevelType
{
	MARTTA_PROPER(TopLevelType)

public:
	MARTTA_NAMED(Artificials)

	Class(): m_whackerCount(0) {}

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

	List<Member*>						members(bool _isConst = false, Access _access = Private) const;
	template<class T> inline List<T*>	membersOf(bool _isConst = false, Access _access = Private) const { return filterConcepts<T>(members(_isConst, _access)); }

	Access								baseAccess(Class* _c) const;

protected:
	virtual Rgb							idColour() const { return Rgb(0xff7777); }
	virtual Kinds						allowedKinds(int) const;
	virtual String						interfaceCode() const;
	virtual String						implementationCode() const;
	virtual String						defineHtml() const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual List<Declaration*>			utilised() const;
	virtual void						onDependencyChanged(int, Concept* _e);
	virtual void						apresLoad() { rejigDeps(); checkImplicitConstructors(); updateWhacked(); }
	virtual void						onDependencyAdded(Concept* _e);
	virtual void						onDependencyRemoved(Concept* _e, int);
	// A child has been added to the class somewhere in the middle, probably. Either way we need to rejig deps to get the new access label tracked.
	virtual void						onDependencySwitched(Concept* _e, Concept*) { onDependencyAdded(_e); }
	virtual void						onChildrenInitialised();
	virtual bool						hasDefaultConstructor() const;
	virtual bool						hasSingleCastOperator(TypeConcept const* _t, bool _const) const;
	virtual bool						hasSingleConversionConstructor(TypeConcept const* _f) const;
	virtual bool						isCallable(Type* = 0, bool = false) const;
	virtual bool						defineSimilarityTo(TypeConcept const* _t, TypeConcept::Castability _c) const;
	virtual Types						assignableTypes() const;
	virtual List<ValueDefiner*>			applicableMembers(Concept const* _s, bool _isConst) const;
	virtual bool						canStandAlone() const;
	virtual String						informationHtml() const;

	String								memberedImplementationCode(String const& _preamble) const;

private:
	bool								checkImplicitConstructors();
	void								rejigDeps();
	bool								updateWhacked() const;

	mutable int							m_whackerCount;
};


}
