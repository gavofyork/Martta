/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "Concept.h"
#include "Position.h"

namespace Martta
{

bool Position::exists() const
{
	return isValid() && m_index != UndefinedIndex && m_parent->child(m_index);
}

bool Position::isFixed() const
{
	return m_parent && (m_parent->isFixed() || m_parent->defineChildFixed(m_index == UndefinedIndex ? m_parent->cardinalChildCount() : m_index));
}

Concept* Position::concept() const
{
	AssertNR(exists());
	return m_parent->child(m_index);
}

Concept* Position::spawnPreparedSilent() const
{
	Concept* ret = Concept::spawn(allowedKinds().commonBase().name());
	ret->prepareChildren();
	insertSilent(ret);
	return ret;
}

Concept* Position::spawnPrepared() const
{
	Concept* ret = Concept::spawn(allowedKinds().commonBase().name());
	ret->prepareChildren();
	insert(ret);
	return ret;
}

Concept* Position::place(Concept* _e) const
{
	AssertNR(_e);
	_e->put(*this);
	return _e;
}

Concept* Position::insert(Concept* _e) const
{
	AssertNR(_e);
	_e->move(*this);
	return _e;
}

void Position::insertSilent(Concept* _e) const
{
	AssertNR(_e);
	_e->silentMove(*this);
}

bool Position::allowedToBeKind(Kind _k) const
{
	return m_parent ? m_parent->isAllowed((m_index == UndefinedIndex) ? m_parent->cardinalChildCount() : m_index, _k) : true;
}

bool Position::isRequired() const
{
	return m_index < 0 ? (m_parent->childCount(m_index) < m_parent->minRequired(m_index)) : ((m_index == UndefinedIndex ? m_parent->cardinalChildCount() : m_index) < m_parent->minRequired());
}

Kinds Position::allowedKinds() const
{
	return m_parent->allowedKinds(m_index == UndefinedIndex ? m_parent->cardinalChildCount() : m_index);
}

Kinds Position::deniedKinds() const
{
	return m_parent->deniedKinds(m_index == UndefinedIndex ? m_parent->cardinalChildCount() : m_index);
}

Concept* Position::nearestConcept() const
{
	if (m_parent->child(m_index))
		return m_parent->child(m_index);
	else if (m_parent->cardinalChildCount())
		return m_parent->cardinalChildren().last();
	else
		return m_parent;
}

TextStream& operator<<(TextStream& _out, Position const& _item)
{
	_out << _item.m_index << "@" << &*_item.m_parent;
	if (_item.exists())
		_out << "[" << _item.concept() << "]";
	return _out;
}

}
