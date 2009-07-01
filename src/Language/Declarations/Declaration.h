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

#include "Identifiable.h"
#include "Entity.h"

namespace Martta
{

class ValueDefiner;

/**
 * Class for anything individually referencable in the language.
 * Currently this includes only functions, variables, types and enumeration values.
 */
class Declaration: public Entity, public_interface Identifiable
{
	MARTTA_PLACEHOLDER(Entity)
	MARTTA_INHERITS(Identifiable, 0)

public:
	virtual ~Declaration();

	/// From Identifiable (default implementations).
	virtual QString						key() const;
	virtual Identifiable*				lookupChild(QString const& _key) const;
	Identifiable*						findEntity(QString const& _key) const;	// Treats this entity as root scope and looks for (scoped) _key in it. 
	void								archivePtrs(bool _clear = false) const;
	void								restorePtrs() const;
	
	int									registerAnonymous(Identifiable const* _e) const { if (m_anonyma.contains(_e)) return m_anonyma.indexOf(_e); m_anonyma << _e; return m_anonyma.size() - 1; }
	void								registerAnonymous(Identifiable const* _e, int _k) { while (m_anonyma.size() <= _k) m_anonyma << 0; m_anonyma[_k] = _e; }

	QList<Declaration*>					utilisedSiblings() const;
	virtual QList<Declaration*>			utilised() const;

	QList<ValueDefiner*>				valuesKnown() const;
	virtual QList<ValueDefiner*>		valuesAdded() const { return QList<ValueDefiner*>(); }
	
	virtual QString						interfaceCode() const { return QString(); }
	virtual QString						implementationCode() const { return QString(); }

	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);
	
protected:
	virtual int							minRequired(int _i) const { return _i == Identity ? 1 : 0; }
	virtual Kinds						allowedKinds(int _i) const;

private:
	mutable QList<Identifiable const*>	m_anonyma;
};

}
