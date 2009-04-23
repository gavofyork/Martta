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

#include "Entity.h"

namespace Martta
{

class Type;
class SubAddressable;

/**
 * Class for anything individually referencable in the language.
 * Currently this includes only functions, variables, types and enumeration values.
 */
class DeclarationEntity: public Entity
{
	MARTTA_OBJECT_INTERFACE(Entity)

public:
	virtual ~DeclarationEntity();

	/// Returns the context if it is addressable. If not (e.g. Statement-derived context) it returns zero.
	virtual SubAddressable*				addressableContext() const;

	/// @returns the user-visible name used for this entity. (e.g. "foo", "bar", "my class")
	virtual QString						name() const;
	/// @returns the name used for this declaration in the CPP code. (e.g. "foo", "m_bar", "MyClass")
	virtual QString						codeName() const;
	/// @returns the program-wide reference used for this declaration in the CPP code (calls codeName()).
	/// (e.g. "::MyClass::m_foo", "::MyClass::bar", "::MyClass")
	virtual QString						reference() const;
	/// @returns the Martta-identity for this entity.
	/// (e.g. "m_foo", "void bar(int), "MyClass")
	virtual QString						identity() const { return codeName(); }
	/// @returns the program-wide Martta-reference for this entity.
	/// (e.g. ";;MyClass;;m_foo", ";;MyClass;;void bar(int)", ";;MyClass")
	virtual QString						key() const;

	virtual QString						interfaceCode() const { return QString(); }
	virtual QString						implementationCode() const { return QString(); }

	DeclarationEntity*					lookupChild(QString const& _key) const;
	int									registerAnonymous(DeclarationEntity const* _e) const { if (m_anonyma.contains(_e)) return m_anonyma.indexOf(_e); m_anonyma << _e; return m_anonyma.size() - 1; }
	void								registerAnonymous(DeclarationEntity const* _e, int _k) { while (m_anonyma.size() <= _k) m_anonyma << 0; m_anonyma[_k] = _e; }

	QList<DeclarationEntity*>			utilisedSiblings() const;
	virtual QList<DeclarationEntity*>	utilised() const;
	
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	
protected:
	virtual void						onLeaveScene(RootEntity* _new, RootEntity* _old);
	virtual int							minimumRequired() const { return 1; }
	virtual Kinds						allowedKinds(int _i) const;

private:
	mutable QList<DeclarationEntity const*>
										m_anonyma;
};

}
