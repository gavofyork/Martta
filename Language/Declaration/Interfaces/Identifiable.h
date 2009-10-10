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

#include "Concept.h"
#include "Named.h"

#ifndef M_API_Declaration
#define M_API_Declaration M_OUTAPI
#endif

namespace Martta
{

// Don't forget to use import/export functions!
class M_API_Declaration Identifiable: public_super Concept, public Named
{
	MARTTA_INTERFACE

public:
	MARTTA_NAMED(Identity)

	/// @returns the user-visible name used for this entity. (e.g. "foo", "bar", "my class")
	virtual String						name() const;
	/// @returns the name used for this declaration in the CPP code. (e.g. "foo", "m_bar", "MyClass")
	virtual String						codeName() const;
	/// @returns the program-wide reference used for this declaration in the CPP code (calls codeName()).
	/// (e.g. "::MyClass::m_foo", "::MyClass::bar", "::MyClass")
	virtual String						reference() const { return addressableContext() ? addressableContext()->reference() + "::" + codeName() : codeName(); }
	/// @returns the program-wide non-specific reference (i.e. for free-scoping calls that may be answered by
	/// a virtual overload). (e.g "m_foo", "::Namespace::Class")
	virtual String						nonSpecificReference() const { return reference(); }
	/// @returns the Martta-identity for this entity. Defaults to the codeName().
	/// (e.g. "m_foo", "void bar(int), "MyClass")
	virtual String						identity() const { return codeName(); }
	/// @returns the program-wide Martta-reference for this entity. This must deliver a unique and locatable
	/// string even when the entity is within a statement hierarchy or is anonymous (empty name).
	/// (e.g. ";;MyClass;;m_foo", ";;MyClass;;void bar(int)", ";;MyClass")
	virtual String						key() const;

	virtual Concept const*				associate() const { return self(); }

	/// @returns the entity from which this may be addressed. Typically it is the parent, though some entities
	/// (e.g. EnumValue) skip this immediate parent and use their parent's addressableContext.
	/// Default value to returns the parent is addressable or zero if not (e.g. Statement-derived parent).
	virtual Identifiable*				addressableContext() const;

	virtual int							registerAnonymous(Identifiable const* _e, Identifiable const** _registrar = 0) const;
	virtual void						registerAnonymous(Identifiable const* _e, int _k);

	/// @returns true if the entity cannot be explicitly referenced in the CPP code. This is the case in code
	/// like class {} x; or typedef struct {} y; or enum { z };
	virtual bool						isHidden() const { return codeName().startsWith(".") || name().isEmpty(); }				///< true for anonymous enums.

	virtual Identifiable*				lookupChild(String const& _subkey) const;
	Identifiable*						find(String const& _key) const;	// Treats this entity as root scope and looks for (scoped) _key in it.

protected:
	// These *MUST* be used by any final class in its property functions.
	void								properties(Hash<String, String>& _p) const;
	void								setProperties(Hash<String, String> const& _p);

	virtual ~Identifiable();
};

}
