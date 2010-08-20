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
	MARTTA_PROPER(PhysicalType)

public:
	MARTTA_NAMED(Returned)

	InvocableType(bool _ellipsis = false, bool _wild = false): m_ellipsis(_ellipsis), m_wild(_wild) {}

	void								setEllipsis(bool _on = true) { m_ellipsis = _on; }
	virtual bool						ellipsis() const { return m_ellipsis; }
	virtual bool						isUltimatelyNull() const { return m_wild; }
	virtual Type						returnType() const { return m_wild ? Type() : *childAs<TypeConcept>(Returned); }
	virtual Type						argumentType(int _i) const { return !m_wild && childIs<TypeConcept>(_i) ? *childAs<TypeConcept>(_i) : Type(); }
	virtual int							minimumArgCount() const { return m_wild ? 0 : cardinalChildCount(); }
	virtual bool						hasArgumentAt(int _i) const { return m_wild || m_ellipsis || _i < cardinalChildCount(); }
	virtual bool						isWellDefined() const { for (int i = 0; i < minimumArgCount(); ++i) if (!argumentType(i)->isWellDefined()) return false; return returnType()->isWellDefined(); }
	virtual Rgb							idColour() const { return 0xbb77ff; }

protected:
	bool m_ellipsis;
	bool m_wild;

private:
	virtual Types						assignableTypes() const;
	virtual int							minRequired(int _i) const { return _i == Returned ? m_wild ? 0 : 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int) const { return m_wild ? Kinds() : Kinds(Kind::of<TypeConcept>()); }
	virtual String						code(String const& _middle) const;
	virtual TypeConcept*				newClone() const { return new InvocableType(m_ellipsis, m_wild); }
	virtual bool						canStandAlone() const { return false; }
	virtual bool						defineSimilarityTo(TypeConcept const* _t, Castability _c) const;
	virtual List<Declaration*>			utilisedX() const;
	virtual void						setProperties(Hash<String, String> const& _ps) { Super::setProperties(_ps); m_ellipsis = _ps[L"ellipsis"].toBool(); m_wild = _ps[L"wild"].toBool(); }
};

class M_CLASS M_API_Types LambdaType: public_super InvocableType
{
	MARTTA_PROPER(InvocableType)

public:
	LambdaType(bool _ellipsis = false, bool _wild = false): InvocableType(_ellipsis, _wild) {}

protected:
	virtual TypeConcept*				newClone() const { return new LambdaType(m_ellipsis, m_wild); }
};

// Arguments children.
// child(Returned) is return type.
// child(n) is argument n
// For ... functions, m_ellipsis is set.
class M_CLASS M_API_Types FunctionType: public_super InvocableType
{
	MARTTA_PROPER(InvocableType)

public:
	FunctionType(bool _ellipsis = false, bool _wild = false): InvocableType(_ellipsis, _wild) {}

protected:
	virtual TypeConcept*				newClone() const { return new FunctionType(m_ellipsis, m_wild); }
};

}
