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

#include <msTextStream.h>
using MarttaSupport::TextStream;

#include "TypeEntity.h"

namespace Martta
{

class TypeDefinition;
class ModifyingType;
class TypeEntity;

// We, for the general case, assume T is a TypeEntity const&
template<class T>
struct TypeConstructor
{
	static void construct(Type* _this, T _tree);
};

class Type
{
	friend class ModifyingType;
	friend class TypeEntity;
	template<class T> friend struct TypeConstructor;
	
public:
	enum Aadl { AdoptAndDeleteLater };

	inline Type() { m_top = TypeEntity::null->clone(this); }
	inline Type(Type const& _t): m_top(_t.m_top->clone(this)) {}
	inline Type(TypeEntity* _te, Aadl): m_top(_te) { m_top->setOwner(this); }
	template<class T> inline Type(T const& _t) { TypeConstructor<T const&>::construct(this, _t); };
	template<class T> inline Type(T* _t) { TypeConstructor<T*>::construct(this, _t); };
	inline ~Type() { m_top->killAndDelete(); }

	bool isNull() const { return m_top->isPlaceholder(); }
	bool isWellDefined() const { return m_top->isWellDefined(); }
	bool isUltimatelyNull() const;

	// Note this will change the class template types in order to remember what template substitution have happened.
	// If you don't want that to happen, take a copy first.
	bool isSimilarTo(Type const& _to, TypeEntity::Castability _c) const;
	inline bool valueSimilarity(Type const& _to) const { return m_top->isSimilarTo(_to.m_top, TypeEntity::Convertible); }
	Type strippedTo(Type const& _t) const;

	Type& operator=(Type const& _t);

	bool operator==(Type const& _t) const;
	bool operator!=(Type const& _t) const { return !operator==(_t); }

	inline void insertSilentCopy(Position _p) const { _p.insertSilent(m_top->clone(0)); }
	inline void placeCopy(Position _p) const { _p.place(m_top->clone(0)); }

	inline operator Types() const { return Types() << *this; }
	inline Types	operator,(Type const& _q) const { return Types() << *this << _q; }

	inline TypeEntity& operator*() const { return *m_top; }
	inline TypeEntity* operator->() const { return m_top; }
	
	/// Append as child of top. Goes onto the back of the top's list.
	inline Type& append(Type const& _t)
	{
		m_top->back().place(_t.m_top->clone(this));
		return *this;
	}
	inline Type& place(Type const& _t, int _name = TypeEntity::Default)
	{
		AssertNR(_name < 0);
		m_top->middle(_name).place(_t.m_top->clone(this));
		return *this;
	}
	/// As append() but doesn't change this type.
	inline Type& appendedWith(Type const& _t) const { return Type(*this).append(_t); }
	inline Type& placedWith(Type const& _t, int _name = TypeEntity::Default) const { return Type(*this).place(_t, _name); }
	
	/// Append to top. This is now the value of the new top item.
	inline Type& topWith(TypeEntity const& _newContext, int _at = TypeEntity::Default)
	{
		TypeEntity* t = _newContext.clone(this);
		t->middle(_at).place(m_top);
		m_top = t;
		return *this;
	}
	/// As topWith(), but doesn't change this type.
	inline Type toppedWith(TypeEntity const& _newContext, int _at = TypeEntity::Default) const { return Type(*this).topWith(_newContext, _at); }
	
	/// Append to bottom of tree.
	inline Type& tailWith(TypeEntity const& _newChild, int _at = TypeEntity::Default)
	{
		m_top->bottom()->middle(_at).place(_newChild.clone(this));
		return *this;
	}
	/// As tailWith(), but doesn't change this type.
	inline Type tailedWith(TypeEntity const& _newChild, int _at = TypeEntity::Default) const { return Type(*this).tailWith(_newChild, _at); }

private:
	void simpleConstruct(TypeEntity const& _tree) { m_top = ((&_tree) ? _tree : *TypeEntity::null).clone(this); }

	TypeEntity* m_top;
};

inline Types operator,(Types const& _qs, Type const& _t)
{
	return Types(_qs) << _t;
}

inline TextStream& operator<<(TextStream& _out, Martta::Type const& _item)
{
	return _out << _item->code();
}

template<class T>
inline void TypeConstructor<T>::construct(Type* _this, T _tree)
{
	_this->simpleConstruct(_tree);
}

}
