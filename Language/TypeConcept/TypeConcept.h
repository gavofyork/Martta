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

#include <msRgb.h>
using namespace MarttaSupport;

#include "WebViewable.h"
#include "TypedOwner.h"
#include "Concept.h"

#ifndef M_API_TypeConcept
#define M_API_TypeConcept M_OUTAPI
#endif

namespace Martta
{

class M_CLASS Declaration;
class M_CLASS ModifyingType;
class M_CLASS ValueDefiner;
class Type;
class M_CLASS TypeConcept;
template<class T> struct TypeConstructor;

class M_CLASS M_API_TypeConcept TypeConcept: public_super Concept, public_interface TypedOwner, public_interface WebViewable
{
	MARTTA_PLACEHOLDER(Concept)
	MARTTA_ALSO_INHERITS(TypedOwner, 0)
	MARTTA_ALSO_INHERITS(WebViewable, 1)

	friend class Type;
	template<class T> friend struct TypeConstructor;
	friend class ModifyingType;
	friend class TypeDefinition;

public:
	static int							s_typeCount;
	static bool							s_debugCastability;

	MARTTA_NAMED(Default)

	enum TNT { TheNullType };

	inline TypeConcept(): m_owner(0)
	{
#if defined(DEBUG)
		s_typeCount++;
#endif
		m_isUnchanging = true;
		m_isAsleep = true;
	}
	TypeConcept(TNT);
	inline ~TypeConcept()
	{
#if defined(DEBUG)
		s_typeCount--;
#endif
	}

	virtual String						typeName() const { return code(); }
	virtual String						code(String const& _middle = "") const { return L"???" + _middle; }
	virtual TypeConcept*					bottom() { return this; }
	virtual Rgb							idColour() const { return 0x777777; }
	virtual String						defineHtml() const;
	virtual bool						shouldNotify() const { return !m_owner && !m_isAsleep && !m_isUnchanging && isInModel(); }
	virtual bool						shouldBeNotified() const { return !m_owner && !m_isAsleep && !m_isUnchanging && isInModel(); }
	virtual bool						isInModel() const { return (parent() && !m_owner) ? parent()->isInModel() : true; }

	inline bool							isOwned() const { if (m_owner) return true; if (!parent()) return false; if (TypeConcept* t = tryParent<TypeConcept>()) return t->isOwned(); return true; }
	virtual bool						isNull() const { return isPlaceholder(); }
	virtual bool						isUltimatelyNull() const { return isNull(); }
	virtual bool						isWellDefined() const { return !isNull(); }

	typedef int Castability;
	enum { Unrelated = 0, Physical = 1, Convertible = 2, FairlyConvertible = 6, BasicallyConvertible = 14, VeryConvertible = 30, ConstPerfectConvertible = 62, MostConvertible = 62, Logical = Physical | Convertible };
	// Note: Logical can *only* be used for *giving* the _requirement parameter.
	//		Test the _requirement parameter for portions of Logicality with isLogicalAtMost.
	// Note: Unrelated is a dummy requirement and will always return true;
	// FairlyConvertible: float->int, int->float etc and
	// BasicallyConvertible: long->int, signed char->unsigned short etc and
	// VeryConvertible: int->const int etc and
	// ConstPerfectConvertible: int->int, const int->const int
	bool								isSimilarTo(TypeConcept const* _to, Castability _requirement) const;
	inline bool							isEquivalentTo(TypeConcept const* _t) const { return defineEquivalenceTo(_t) || _t->defineEquivalenceFrom(this); }
	virtual bool						contentsEquivalentTo(TypeConcept const*) const { return true; }
	virtual bool						canStandAlone() const { return true; }
	virtual bool						isCallable(Type* = 0, bool = false) const { return false; }

	// TODO: Make these protected again when we have a TypeForwarder notion (which will be a friend).
	// Classes may opt to reimplement one or both of these.
	// When determining the return type make sure you check the Castability _requirement first, before considering the
	// types. This helps prevent circular dependencies which will cause Martta to crash.
	virtual bool						defineSimilarityFrom(TypeConcept const* /*_from*/, Castability /*_requirement*/) const { return isNull(); }
	virtual bool						defineSimilarityTo(TypeConcept const* /*_to*/, Castability _requirement) const { return _requirement == Unrelated; }

	virtual bool						hasDefaultConstructor() const { return false; }
	/// Types that assignment operator may take on right hand side, assuming left hand side is a reference to this type.
	virtual Types						assignableTypes() const;
	virtual List<ValueDefiner*>			applicableMembers(Concept const* /*_s*/ = 0, bool /*_isConst*/ = false) const { return List<ValueDefiner*>(); }

	/// isType/asType: They ignore qualifiers and test for/identify the physical type stored.
	/// In terms of C++, Reference and Const are considered physically transparent (though
	/// can still be searched for explicitly). FunctionType, Memberify and AddressType are not.
	virtual bool						isType(Kind _typeKind) { return Concept::isKind(_typeKind); }
	template<class T> inline bool		isType() { return isType(Kind::of<T>()); }
	virtual TypeConcept*					asType(Kind _typeKind) { (void)(_typeKind); AssertNR(isType(_typeKind)); return this; }
	template<class T> inline T*			asType() { return asType(Kind::of<T>())->asKind<T>(); }
	inline TypeConcept*					tryType(Kind _typeKind) { return isType(_typeKind) ? asType(_typeKind) : 0; }
	template<class T> inline T*			tryType() { return isType<T>() ? asType<T>() : 0; }

	template<class T> inline TypeConcept*ignore() { return isKind<T>() ? childAs<TypeConcept>(Default) : this; }

	///
	template<class T> inline T*			knit() { T* ret = new T; knitIn(ret); return ret; }
	/// _t goes in our place, and we are a child of _t
	void								knitIn(TypeConcept* _t);

	static TypeConcept*					null;

	/// Just a tunnel into other TypeConcept's newClone methods.
	static TypeConcept*					cloneOf(TypeConcept const* _t, Type* _owner) { return _t->newClone(_owner); }

	virtual List<Declaration*>			utilised() const { return List<Declaration*>(); }

	virtual void						rejig() const {}

	String								typeHtml(String const& _middle) const;
	virtual String						fullHtml(String const& _middle) const { return tagOf(L"TypeConcept", typeHtml(_middle)); }

protected:
	/// This newClone is the simple one; it doesn't have to change the ownership or duplicate the children.
	virtual TypeConcept*					newClone() const { return new TypeConcept; }
	/// This newClone must provide the re-owned entity, complete with all necessary children.
	virtual TypeConcept*					newClone(Type* _owner) const;

	// You can just use == for testing whether an allowed castability fulfills a required castability, except in a couple of cases;
	// isLogicalAtMost will test true iff the required castability is equal to one or both of Physical, Convertible. In particular, it will
	// return false if the requirement includes FairlyConvertible or VeryConvertible.
	static inline bool					isLogicalAtMost(Castability _required) { return !(_required & ~Physical & ~Convertible); }
	// isAnyConvertible will test true iff the required castability is exactly equal to any of the Convertibles.
	// In particular, it will return false if the requirement includes Physical.
	static inline bool					isAnyConvertible(Castability _required) { return !(_required & ~MostConvertible); }
	// isAtMostFairlyConvertible will test true iff the required castability is exactly equal to any of the Convertibles at less restrictive or equals to FairlyConvertible.
	// i.e. It will return false if the requirement includes Physical, BasicallyConvertible, VeryConvertible etc.
	static inline bool					isFairlyConvertibleAtMost(Castability _required) { return !(_required & ~FairlyConvertible); }
	// isAtMostBasicallyConvertible will test true iff the required castability is exactly equal to any of the Convertibles at less restrictive or equals to FairlyConvertible.
	// i.e. It will return false if the requirement includes Physical, VeryConvertible etc.
	static inline bool					isBasicallyConvertibleAtMost(Castability _required) { return !(_required & ~BasicallyConvertible); }

	virtual bool						isSuperfluous() const { return parent()->allowedKinds(index()).commonBase() != kind() && isNull(); }
	virtual bool						isInValidState() const { return isWellDefined(); }

	virtual bool						defineEquivalenceFrom(TypeConcept const* /*_from*/) const { return false; }
	virtual bool						defineEquivalenceTo(TypeConcept const* /*_to*/) const;

	Type*								owner() const { return m_owner; }
	void								setOwner(Type* _o) { m_owner = _o; }

	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(int, Concept*) { changed(Logically); }

	// TypeConcept should not defer to Concept for [a-z] keypress. Instead it should attempting to insert a specific TypeConcept
	// (actually a BuiltType, but we're not to know). This might be changed in the future.
	virtual bool						keyPressed(KeyEvent const*_e);

	virtual void						parentAdded();
	virtual void						parentSwitched(Concept* _exParent);

private:
	void								setInert(bool _inert) { if (m_isUnchanging != _inert || m_isAsleep != _inert) { m_isUnchanging = m_isAsleep = _inert; foreach (TypeConcept* e, childrenOf<TypeConcept>()) e->setInert(_inert); apresLoad(); } }

	/// Returns an exact copy of this tree, except that the top is owned by _t.
	/// Unowned nodes are copied.
	inline TypeConcept*					clone(Type* _owner) const { return newClone(_owner); }

	Type*								m_owner;
};

}
