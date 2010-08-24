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

#include "Type.h"
#include "PhysicalType.h"

#ifndef M_API_Types
#define M_API_Types M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Types InvocableType: public_super PhysicalType
{
	MARTTA_PLACEHOLDER(PhysicalType)

public:
	virtual bool						isUltimatelyNull() const { return true; }
	virtual Type						returnType() const { return Type(); }
	virtual Type						argumentType(int) const { return Type(); }
	virtual int							minimumArgCount() const { return 0; }
	virtual bool						hasArgumentAt(int) const { return false; }
	virtual bool						isWellDefined() const { return false; }
	virtual Rgb							idColour() const { return 0xbb77ff; }
	virtual bool						canStandAlone() const { return false; }

private:
	virtual TypeConcept*				newClone() const { return new InvocableType; }
};

class M_CLASS M_API_Types WildInvocableType: public_super InvocableType
{
	MARTTA_PROPER(InvocableType)

protected:
	virtual bool						hasArgumentAt(int) const { return true; }
	virtual bool						isWellDefined() const { return false; }
	virtual bool						canStandAlone() const { return true; }
	virtual String						code(String const& _middle) const;
	virtual TypeConcept*				newClone() const { return new WildInvocableType; }
	virtual bool						defineSimilarityFrom(TypeConcept const* _f, Castability _c) const;
};

class M_CLASS M_API_Types ContainedInvocableType: public_super InvocableType
{
	MARTTA_PLACEHOLDER(InvocableType)

public:
	MARTTA_NAMED(Returned)

	ContainedInvocableType(bool _ellipsis = false): m_ellipsis(_ellipsis) {}

	void								setEllipsis(bool _on = true) { m_ellipsis = _on; }
	virtual bool						isUltimatelyNull() const { return false; }
	virtual Type						returnType() const { return *childAs<TypeConcept>(Returned); }
	virtual Type						argumentType(int _i) const { return childIs<TypeConcept>(_i) ? *childAs<TypeConcept>(_i) : Type(); }
	virtual int							minimumArgCount() const { return cardinalChildCount(); }
	virtual bool						hasArgumentAt(int _i) const { return m_ellipsis || _i < cardinalChildCount(); }
	virtual bool						isWellDefined() const { for (int i = 0; i < minimumArgCount(); ++i) if (!argumentType(i)->isWellDefined()) return false; return returnType()->isWellDefined(); }
	virtual bool						canStandAlone() const;

protected:
	bool m_ellipsis;

private:
	virtual Types						assignableTypes() const;
	virtual int							minRequired(int _i) const { return _i == Returned ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int) const { return Kinds(Kind::of<TypeConcept>()); }
	virtual String						code(String const& _middle) const;
	virtual TypeConcept*				newClone() const { return new ContainedInvocableType(m_ellipsis); }
	virtual List<Declaration*>			utilisedX() const;
	virtual void						setProperties(Hash<String, String> const& _ps) { Super::setProperties(_ps); m_ellipsis = _ps[L"ellipsis"].toBool(); }
};

class M_CLASS M_API_Types FunctorType: public_super ContainedInvocableType
{
	MARTTA_PROPER(ContainedInvocableType)

public:
	FunctorType(): ContainedInvocableType(false) {}

protected:
	virtual String						code(String const& _middle) const;
	virtual TypeConcept*				newClone() const { return new FunctorType; }
};

// Arguments children.
// child(Returned) is return type.
// child(n) is argument n
// For ... functions, m_ellipsis is set.
class M_CLASS M_API_Types FunctionType: public_super ContainedInvocableType
{
	MARTTA_PROPER(ContainedInvocableType)

public:
	FunctionType(bool _ellipsis = false): ContainedInvocableType(_ellipsis) {}

protected:
	virtual bool						canStandAlone() const { return false; }
	virtual TypeConcept*				newClone() const { return new FunctionType(m_ellipsis); }
	virtual bool						defineSimilarityTo(TypeConcept const* _t, Castability _c) const;
};

}
