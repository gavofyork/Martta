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

#include <QString>

#include "SceneLeaver.h"
#include "Meta.h"

class QDomElement;

namespace Martta
{

// Don't forget to use import/export functions!
class Identifiable: public_interface SceneLeaver
{
	MARTTA_INTERFACE
	MARTTA_INHERITS(SceneLeaver, 0)
	MARTTA_NAMED(Identity)
	
public:	
	/// @returns the user-visible name used for this entity. (e.g. "foo", "bar", "my class")
	virtual QString						name() const;
	/// @returns the name used for this declaration in the CPP code. (e.g. "foo", "m_bar", "MyClass")
	virtual QString						codeName() const;
	/// @returns the program-wide reference used for this declaration in the CPP code (calls codeName()).
	/// (e.g. "::MyClass::m_foo", "::MyClass::bar", "::MyClass")
	virtual QString						reference() const { return addressableContext() ? addressableContext()->reference() + "::" + codeName() : codeName(); }
	/// @returns the Martta-identity for this entity. Defaults to the codeName().
	/// (e.g. "m_foo", "void bar(int), "MyClass")
	virtual QString						identity() const { return codeName(); }
	/// @returns the program-wide Martta-reference for this entity. This must deliver a unique and locatable
	/// string even when the entity is within a statement hierarchy or is anonymous (empty name).
	/// (e.g. ";;MyClass;;m_foo", ";;MyClass;;void bar(int)", ";;MyClass")
	virtual QString						key() const;
	
	/// @returns the entity from which this may be addressed. Typically it is the parent, though some entities
	/// (e.g. EnumValue) skip this immediate parent and use their parent's addressableContext. 
	/// Default value to returns the context is addressable or zero if not (e.g. Statement-derived context).
	virtual Identifiable*				addressableContext() const;

	/// @returns true if the entity cannot be explicitly referenced in the CPP code. This is the case in code
	/// like class {} x; or typedef struct {} y; or enum { z };
	virtual bool						isHidden() const { return codeName().startsWith(".") || name().isEmpty(); }				///< true for anonymous enums.

	virtual Identifiable*				lookupChild(QString const& _key) const;
	
	virtual void						onLeaveScene(RootEntity* _new, RootEntity* _old);
	
protected:
	// These *MUST* be used by any final class in its export/import functions.
	void								exportDom(QDomElement& _element) const;
	void								importDom(QDomElement const& _element);
	
	virtual ~Identifiable() {}
};

}
