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

#include "TypedOwner.h"
#include "Entity.h"

namespace Martta
{

extern int g_typeCount;
extern bool g_debugCastability;

class Declaration;
class ModifyingType;
class ValueDefiner;

class TypeEntity: public Entity, public_interface TypedOwner
{
	MARTTA_PLACEHOLDER(Entity)
	MARTTA_INHERITS(TypedOwner, 0)

	friend class Type;
	friend class ModifyingType;

public:
	enum { Default = FirstNamed, EndOfNamed };

	TypeEntity(): m_owner(0) { g_typeCount++; }
	~TypeEntity() { g_typeCount--; }
	
	virtual QString						idColour() const { return "#777"; }
	virtual QString						code(QString const& _middle = "") const { return _middle; }
	virtual TypeEntity*					bottom() { return this; }
	
	inline bool							isOwned() const { if (m_owner) return true; if (!parent()) return false; if (TypeEntity* t = tryParent<TypeEntity>()) return t->isOwned(); return true; }
	virtual bool						isNull() const { return isPlaceholder(); }
	virtual bool						isUltimatelyNull() const { return isNull(); }
	virtual bool						isWellDefined() const { return !isNull(); }
	virtual bool						botherNotifying() const { return !isOwned(); }
	
	typedef int Castability;
	enum { Unrelated = 0, Physical = 1, Convertible = 2, FairlyConvertible = 6, BasicallyConvertible = 14, VeryConvertible = 30, ConstPerfectConvertible = 62, MostConvertible = 62, Logical = Physical | Convertible };
	// Note: Logical can *only* be used for *giving* the _requirement parameter.
	//		Test the _requirement parameter for portions of Logicality with isLogicalAtMost.
	// Note: Unrelated is a dummy requirement and will always return true;
	// FairlyConvertible: float->int, int->float etc and
	// BasicallyConvertible: long->int, signed char->unsigned short etc and
	// VeryConvertible: ? and
	// ConstPerfectConvertible: ?
	bool								isSimilarTo(TypeEntity const* _to, Castability _requirement) const;
	inline bool							isEquivalentTo(TypeEntity const* _t) const { return defineEquivalenceTo(_t) || _t->defineEquivalenceFrom(this); }
	virtual bool						contentsEquivalentTo(TypeEntity const*) const { return true; }
	virtual bool						canStandAlone() const { return true; }

	virtual bool						hasDefaultConstructor() const { return false; }
	/// Types that assignment operator may take on right hand side, assuming left hand side is a reference to this type.
	virtual Types						assignableTypes() const;
	virtual QList<ValueDefiner*>		applicableMembers(Entity* /*_s*/ = 0, bool /*_isConst*/ = false) const { return QList<ValueDefiner*>(); }
	
	/// isType/asType: They ignore qualifiers and test for/identify the physical type stored.
	/// In terms of C++, Reference and Const are considered physically transparent (though
	/// can still be searched for explicitly). FunctionType, Memberify and AddressType are not.
	virtual bool						isType(Kind _typeKind) { return Entity::isKind(_typeKind); }
	template<class T> inline bool		isType() { return isType(Kind::of<T>()); }
	virtual TypeEntity*					asType(Kind _typeKind) { M_ASSERT(isType(_typeKind)); return this; }
	template<class T> inline T*			asType() { return static_cast<T*>(asType(Kind::of<T>())); }
	
	template<class T> inline TypeEntity*ignore() { return isKind<T>() ? childAs<TypeEntity>(Default) : this; }

	/// 
	template<class T> inline T*			knit() { T* ret = new T; knitIn(ret); return ret; }
	/// _t goes in our place, and we are a child of _t
	void								knitIn(ModifyingType* _t);
	
	TypeEntity*							setOwned() { m_owner = (Type*)1; return this; }
	static TypeEntity*					null;
	
	/// Just a tunnel into other TypeEntity's newClone methods.
	static TypeEntity*					cloneOf(TypeEntity const* _t, Type* _owner) { return _t->newClone(_owner); }
	
	virtual QList<Declaration*>	utilised() const { return QList<Declaration*>(); }
	
protected:
	/// This newClone is the simple one; it doesn't have to change the ownership or duplicate the children.
	virtual TypeEntity*					newClone() const { return new TypeEntity; }
	/// This newClone must provide the re-owned entity, complete with all necessary children.
	virtual TypeEntity*					newClone(Type* _owner) const;
	
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

	
	// Classes may opt to reimplement one or both of these.
	// When determining the return type make sure you check the Castability _requirement first, before considering the
	// types. This helps p[revent circular dependencies which will cause Martta to crash.
	virtual bool						defineSimilarityFrom(TypeEntity const* /*_from*/, Castability /*_requirement*/) const { return isNull(); }
	virtual bool						defineSimilarityTo(TypeEntity const* /*_to*/, Castability _requirement) const { return _requirement == Unrelated; }
	
	virtual bool						defineEquivalenceFrom(TypeEntity const* /*_from*/) const { return false; }
	virtual bool						defineEquivalenceTo(TypeEntity const* /*_to*/) const;
	
	Type*								owner() const { return m_owner; }
	void								setOwner(Type* _o) { m_owner = _o; }
	
	QString								typeLayout() const { return QString("e;c;s%1;fb;").arg(idColour()); } 
	QString								instanceLayout() const { return QString("e;c;s%1;fb0;").arg(idColour()); } 
	
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(Entity*) { changed(); }
	
private:
	/// Returns an exact copy of this tree, except that the top is owned by _t.
	/// Unowned nodes are copied.
	inline TypeEntity*					clone(Type* _owner) const { return newClone(_owner); }

	Type*								m_owner;
};

}
