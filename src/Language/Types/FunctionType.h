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
#include "TypeEntity.h"

namespace Martta
{

// Arguments+1 children.
// entity(0) is return type.
// entity(n+1) is argument n
// For ... functions, m_ellipsis is set.
class FunctionType: public TypeEntity
{
	MARTTA_OBJECT(TypeEntity)

public:
	FunctionType(bool _ellipsis = false, bool _wild = false): m_ellipsis(_ellipsis), m_wild(_wild) {}

	bool								ellipsis() const { return m_ellipsis; }
	virtual bool						isUltimatelyNull() const { return m_wild; }
	Type								returnType() const { return m_wild ? Type() : *entityAs<TypeEntity>(0); }
	Type								argumentType(int _i) const { return !m_wild && entityIs<TypeEntity>(_i + 1) ? *entityAs<TypeEntity>(_i + 1) : Type(); }
	int									minimumArgCount() const { return m_wild ? 0 : (entities().size() - 1); }
	bool								hasArgumentAt(int _i) const { return m_wild || m_ellipsis || _i < entities().size() - 1; }

	void								setEllipsis(bool _on = true) { m_ellipsis = _on; }

	virtual QString						idColour() const { return "#b7f"; }
	
private:
	virtual Types						assignableTypes() const;
	virtual int							minimumRequired() const { return m_wild ? 0 : 1; }
	virtual Kinds						allowedKinds(int) const { return m_wild ? Kinds() : Kinds(Kind::of<TypeEntity>()); }
	virtual QString						code(QString const& _middle) const;
	virtual TypeEntity*					newClone() const { return new FunctionType(m_ellipsis, m_wild); }
	virtual bool						canStandAlone() const { return false; }
	virtual bool						defineSimilarityTo(TypeEntity const* _t, Castability _c) const;

	bool m_ellipsis;
	bool m_wild;
};

}
