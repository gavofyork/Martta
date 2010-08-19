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
using namespace MarttaSupport;

#include "TypeConcept.h"

#ifndef M_API_TypeConcept
#define M_API_TypeConcept M_OUTAPI
#endif

namespace Martta
{

class M_CLASS TypeDefinition;
class M_CLASS ModifyingType;

// We, for the general case, assume T is a TypeConcept const&
template<class T>
struct TypeConstructor
{
	static void construct(Type* _this, T _tree);
};

class M_API_TypeConcept Type
{
	friend class ModifyingType;
	friend class TypeConcept;
	template<class T> friend struct TypeConstructor;

public:
	enum Aadl { AdoptAndDeleteLater };
	enum Tnto { TheNullTypeOwner };

	inline Type(Tnto) { m_top = 0; }
	inline Type() { m_top = TypeConcept::null->clone(this); }
	inline Type(Type const& _t): m_top(_t.m_top->clone(this)) {}
	inline Type(TypeConcept* _te, Aadl): m_top(_te) { m_top->setOwner(this); }
	template<class T> inline Type(T const& _t) { TypeConstructor<T const&>::construct(this, _t); };
	template<class T> inline Type(T* _t) { TypeConstructor<T*>::construct(this, _t); };
	inline ~Type() { m_top->killAndDelete(); }

	bool isNull() const { return m_top->isPlaceholder(); }
	bool isWellDefined() const { return m_top->isWellDefined(); }
	bool isUltimatelyNull() const;

	// Note this will change the class template types in order to remember what template substitution have happened.
	// If you don't want that to happen, take a copy first.
	bool isSimilarTo(Type const& _to, TypeConcept::Castability _c) const;
	inline bool valueSimilarity(Type const& _to) const { return m_top->isSimilarTo(_to.m_top, TypeConcept::Convertible); }
	Type strippedTo(Type const& _t) const;

	Type& operator=(Type const& _t);

	bool operator==(Type const& _t) const;
	bool operator!=(Type const& _t) const { return !operator==(_t); }

	inline void insertSilentCopy(Position _p) const { _p.insertSilent(m_top->clone(0)); }
	inline void placeCopy(Position _p) const { _p.place(m_top->clone(0)); }

	inline operator Types() const { return Types() << *this; }
	inline Types	operator,(Type const& _q) const { return Types() << *this << _q; }

	operator bool() const { return !isNull(); }
	inline TypeConcept& operator*() const { return *m_top; }
	inline TypeConcept* operator->() const { return m_top; }

	/// Append as child of top. Goes onto the back of the top's list.
	inline Type& append(Type const& _t)
	{
		m_top->back().place(_t.m_top->clone(this));
		return *this;
	}
	inline Type& place(Type const& _t, int _name = TypeConcept::Default)
	{
		AssertNR(_name < 0);
		m_top->middle(_name).place(_t.m_top->clone(this));
		return *this;
	}
	/// As append() but doesn't change this type.
	inline Type& appendedWith(Type const& _t) const { return Type(*this).append(_t); }
	inline Type& placedWith(Type const& _t, int _name = TypeConcept::Default) const { return Type(*this).place(_t, _name); }

	/// Append to top. This is now the value of the new top item.
	inline Type& topWith(TypeConcept const& _newContext, int _at = TypeConcept::Default)
	{
		TypeConcept* t = _newContext.clone(this);
		t->middle(_at).place(m_top);
		m_top = t;
		return *this;
	}
	/// As topWith(), but doesn't change this type.
	inline Type toppedWith(TypeConcept const& _newContext, int _at = TypeConcept::Default) const { return Type(*this).topWith(_newContext, _at); }

	/// Append to bottom of tree.
	inline Type& tailWith(TypeConcept const& _newChild, int _at = TypeConcept::Default)
	{
		m_top->bottom()->middle(_at).place(_newChild.clone(this));
		return *this;
	}
	/// As tailWith(), but doesn't change this type.
	inline Type tailedWith(TypeConcept const& _newChild, int _at = TypeConcept::Default) const { return Type(*this).tailWith(_newChild, _at); }

private:
	void simpleConstruct(TypeConcept const& _tree) { m_top = ((&_tree) ? _tree : *TypeConcept::null).clone(this); }

	TypeConcept* m_top;
};

inline String compileTypes(Types const& _t)
{
	String ret;
	foreach (Type t, _t)
		ret += t->typeName() + L", ";
	if (ret.endsWith(L", "))
		ret.chop(2);
	return ret;
}

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
	_this->simpleConstruct(*_tree.template asKind<class TypeConcept>());
}

}
