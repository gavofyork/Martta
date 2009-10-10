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

#include "ConceptSupport.h"
#include "ChildValidifier.h"
#include "Depender.h"
#include "Dependee.h"
#include "KeyEvent.h"
#include "Dier.h"
#include "Meta.h"
#include "Position.h"
#include "CodeScene.h"
#include "SafePointer.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

class EditDelegateFace;

class M_API_Concept Concept: public_super Nothing, public SafePointerTarget, virtual public Dier, public ChildValidifier, public Depender, public Dependee
{
	MARTTA_COMMON(Nothing)

	friend class EditDelegateFace;
	friend class KeyEvent;

protected:
	enum { Cardinals = 0 };

public:
#if defined(DEBUG)
	void*								operator new(size_t _size);
	void								operator delete(void* p);
#endif

	/// Copy constructor which doesn't do anything. Have to have it so a derived class can use it.
	inline Concept(): Dier(), SafePointerTarget(), ChildValidifier(), Depender(), Dependee(), m_parent(0), m_index(UndefinedIndex) {}
	inline Concept(Concept const&): Dier(), SafePointerTarget(), ChildValidifier(), Depender(), Dependee() { AssertNR(false); }

	static inline void					initialiseClass() {}
	static inline void					finaliseClass() {}

	void								silentMove(Position const& _to);
	inline void							silentRemove() { silentMove(Nowhere); }
	void								move(Position const& _newPosition);
	/// Same as move() except that if the destination is a placeholder we replace it. Favour this over move() generally.
	void								put(Position const& _newPosition);
	/// Same as put() except that if the destination index is named any entity is replaced.
	void								shove(Position const& _newPosition);

	inline int							index() const { return m_index; }
	inline Concept*						parent() const { return m_parent; }
	template<class T> inline bool		parentIs() const { return m_parent ? m_parent->isKind<T>() : false; }
	template<class T> inline T*			parentAs() const { AssertNR(m_parent); return m_parent->asKind<T>(); }
	template<class T> inline T*			tryParent() const { return m_parent ? m_parent->tryKind<T>() : 0; }

	inline int							childCount() const { return m_namedChildren.size() + m_cardinalChildren.size(); }
	inline List<Concept*>				children() const { return m_namedChildren.values() + m_cardinalChildren; }
	inline Concept*						child(int _i) const { if (_i >= 0 && _i < m_cardinalChildren.size()) { AssertNR(m_cardinalChildren[_i]->m_parent == this); return m_cardinalChildren[_i]; } else return _i < 0 ? m_namedChildren.value(_i) : 0; }
	inline int							childCount(int _i) const { if (_i >= 0) return _i < m_cardinalChildren.size() ? 1 : 0; int r = 0; Hash<int, Concept*>::ConstIterator i = m_namedChildren.find(_i); while (i != m_namedChildren.end() && i.key() == _i) ++r, ++i; return r; }
	inline List<Concept*>				children(int _i) const { if(_i < 0) return m_namedChildren.values(_i); if (_i < m_cardinalChildren.size()) return List<Concept*>() << child(_i); return List<Concept*>(); }
	template<class T> inline List<T*>	childrenOf() const { return filterEntities<T>(children()); }
	template<class T> inline List<T*>	childrenAs() const { return castEntities<T>(children()); }
	template<class T> inline List<T*>	childrenOf(int _i) const { return filterEntities<T>(children(_i)); }
	template<class T> inline List<T*>	childrenAs(int _i) const { return castEntities<T>(children(_i)); }
	template<class T> inline int		childIndexOf() const { foreach (Concept* e, children()) if (e->isKind<T>()) return e->m_index; return UndefinedIndex; }
	template<class T> inline int		childCountOf() const { int r = 0; foreach (Concept* i, children()) if (i->isKind<T>()) r++; return r; }
	template<class T> inline int		childCountOf(int _i) const { int r = 0; foreach (Concept* i, children(_i)) if (i->isKind<T>()) r++; return r; }
	template<class T> inline T*			childOf() const { AssertNR(childCountOf<T>()); return childAs<T>(childIndexOf<T>()); }
	template<class T> inline T*			childOf(int _i) const { AssertNR(childCountOf<T>(_i)); foreach (Concept* i, children(_i)) if (T* r = i->tryKind<T>()) return r; return 0; }
	template<class T> inline bool		childIs(int _i) const { if (Concept* r = child(_i)) return r->isKind<T>(); return false; }
	template<class T> inline T*			childAs(int _i) const { Concept* e = child(_i); AssertNR(e); return e->asKind<T>(); }
	template<class T> inline T*			tryChild(int _i) const { if (Concept* e = child(_i)) return e->tryKind<T>(); return 0; }

	inline List<Concept*> const&			cardinalChildren() const { return m_cardinalChildren; }
	inline int							cardinalChildCount() const { return m_cardinalChildren.size(); }
	template<class T> inline List<T*>	cardinalChildrenOf() const { return filterEntities<T>(m_cardinalChildren); }
	template<class T> inline List<T*>	cardinalChildrenAs() const { return castEntities<T>(m_cardinalChildren); }
	template<class T> inline T*			cardinalChildOf() const { AssertNR(cardinalChildCountOf<T>()); return childAs<T>(cardinalChildIndexOf<T>()); }
	template<class T> inline int		cardinalChildIndexOf() const { for (int r = 0; r < m_cardinalChildren.size(); ++r) if (childIs<T>(r)) return r; return UndefinedIndex; }
	template<class T> inline int		cardinalChildCountOf() const { int r = 0; foreach (Concept* i, m_cardinalChildren) if (i->isKind<T>()) r++; return r; }

	inline Concept*						sibling(int _i) const { return m_parent ? m_parent->child(_i) : 0; }
	template<class T> inline bool		siblingIs(int _i) const { return m_parent ? m_parent->childIs<T>(_i) : false; }
	template<class T> inline T*			siblingAs(int _i) const { AssertNR(m_parent); return m_parent->childAs<T>(_i); }
	template<class T> inline T*			trySibling(int _i) const { return m_parent ? m_parent->tryChild<T>(_i) : 0; }
	inline List<Concept*>				siblings() const { if (!m_parent) return List<Concept*>(); List<Concept*> ret; foreach (Concept* e, m_parent->children()) if (e != this) ret += e; return ret; }
	template<class T> inline List<T*>	siblingsOf() const { return filterEntities<T>(siblings()); }
	inline int							siblingCount() const { return m_parent ? m_parent->childCount() - 1 : 0; }
	inline List<Concept*>				cardinalSiblings() const { if (!m_parent) return List<Concept*>(); List<Concept*> ret; foreach (Concept* e, m_parent->m_cardinalChildren) if (e != this) ret += e; return ret; }
	template<class T> inline List<T*>	cardinalSiblingsOf() const { return filterEntities<T>(cardinalSiblings());  }
	inline int							cardinalSiblingCount() const { return m_parent ? m_parent->cardinalChildCount() - (m_index < 0 ? 0 : 1) : 0; }
	inline List<Concept*>				parentsChildren() const { return m_parent ? m_parent->children() : List<Concept*>(); }
	template<class T> inline List<T*>	parentsChildrenOf() const { return m_parent ? m_parent->childrenOf<T>() : List<T*>(); }
	inline int							parentsChildCount() const { return m_parent ? m_parent->childCount() : 0; }
	inline List<Concept*>				parentsCardinalChildren() const { return m_parent ? m_parent->cardinalChildren() : List<Concept*>(); }
	template<class T> inline List<T*>	parentsCardinalChildrenOf() const { return m_parent ? m_parent->cardinalChildrenOf<T>() : List<T*>(); }
	inline int							parentsCardinalChildCount() const { AssertNR(m_parent); return m_parent->m_cardinalChildren.size(); }

	template<class T> bool				hasAncestor() const { return hasAncestor(T::staticKind); }
	bool								hasAncestor(Kind _k) const;
	bool								hasAncestor(Concept const* _a) const { return ancestorIndex(_a) != UndefinedIndex; }
	template<class T> T*				ancestor() const { Concept* r = ancestor(T::staticKind); return r ? r->asKind<T>() : 0; }
	Concept*								ancestor(Kind _k) const;
	template<class T> int				ancestorIndex() const { return ancestorIndex(T::staticKind); }
	int									ancestorIndex(Kind _k) const;
	int									ancestorIndex(Concept const* _a) const;

	Concept*							self() { return this; }
	Concept const*						self() const { return this; }

	template<class T> bool				hasSelfAncestor() const { return hasSelfAncestor(T::staticKind); }
	bool								hasSelfAncestor(Kind _k) const;
	bool								hasSelfAncestor(Concept const* _a) const;
	template<class T> T*				selfAncestor() const { Concept* r = selfAncestor(T::staticKind); return r ? r->asKind<T>() : 0; }
	Concept*								selfAncestor(Kind _k) const;
	inline Concept*						root() const { Concept* e; for (e = const_cast<Concept*>(this); e->m_parent; e = e->m_parent) {} return e; }
	template<class T> T*				rootOf() const { T* r = tryKind<T>(); for (Concept* e = const_cast<Concept*>(this); e; e = e->m_parent) if (T* t = e->tryKind<T>()) r = t; return r; }

	template<class T> List<T*>			selfAndAncestorsChildrenOf() const { List<T*> ret = childrenOf<T>(); return parent() ? ret + parent()->selfAndAncestorsChildrenOf<T>() : ret; }
	template<class T> List<T*>			superChildrenOf() const { List<T*> ret; ret << childrenOf<T>(); foreach (Concept* e, children()) ret << e->superChildrenOf<T>(); return ret; }

	inline int							properCount() const { int ret = 0; foreach (Concept* i, m_cardinalChildren) if (!i->isPlaceholder()) ret++; return ret; }
	inline List<Concept*>				properts() const { List<Concept*> ret; foreach (Concept* i, m_cardinalChildren) if (!i->isPlaceholder()) ret += i; return ret; }
	inline Concept*						proper(int _i) const { int c = 0; foreach (Concept* i, m_cardinalChildren) if (c++ == _i) return i; AssertNR(false); return 0; }

	virtual bool						usurpsChild(Concept const*) const { return false; }
	bool								isUsurped() const { return m_parent->usurpsChild(this); }

	void								changed(int _aspects = AllAspects);

	/// Clears all children. This does *not* notify anything of any familial changes.
	void								clearEntities();
	/// Brute-force makes the children valid. Deletes invalids, and reprepares the list.
	/// @returns true if any changes had to be made.
	/// @note changed() called as appropriate.
	bool								validifyChildren();
	/// Removes any invalid children replacing them with valid entities (in a valid state) if necessary.
	/// @note changed() called as appropriate.
	bool								removeInvalidChildren();
	/// Appends any children if they are necessary. They themselves will be in a valid state.
	/// @note changed() called as appropriate.
	Concept*								prepareChildren();

	virtual String						finalCode() const { return String::null; }
	virtual String						summary() const { return kind().name(); }

	/**
	 * Cull-check an object. If it is determined the object is superfluous (with the accoridng method), it will
	 * be killed.
	 */
	bool								cull();

	/**
	 * Put it on a cull-checking list. It won't get deleted until it is safe.
	 */
	void								checkForCullingLater();

	/**
	 * Create a new entity.
	 */
	inline static Concept*				spawn(String const& _kind) { AuxilliaryFace const* f = AuxilliaryRegistrar::get()->auxilliary(_kind); AssertNR(f); return f->create(); }
	template<class T> inline static T*	evaluateAs(String const& _exp, int* _readUpto = 0) { return evaluate(_exp, _readUpto)->asKind<T>(); }
	static Concept*						evaluate(String const& _exp, int* _readUpto = 0);

	/**
	 * Destructs the object. Use this before delete. It sets the parent to zero while the object is still
	 * properly defined, and issues a few bits of pre-delete cleanup.
	 *
	 * Obviously, after this is called you should no longer use the object, since it'll be dead.
	 *
	 * Following this, but before the object is deleted, you might want to notify any other objects of there
	 * being a death of a dependent.
	 *
	 * @note Most of the time one of the other object-heirarchy-modifying methods will be a better choice,
	 * since they handle all of the notifications for you.
	 *
	 * @param _substitute If given then all SafePointers that target this object will be set to _substitute
	 * rather than zero.
	 *
	 * @see usurp() replace() deleteAndRefill()
	 */
	void								kill(Concept* _substitute = 0);

	/**
	 * kill()s and then deletes the object. Rewires any rewirable safe pointers to the substitute if non-zero.
	 * Does not inform any dependent objects that the switch/removal has occured---this is up to the caller.
	 */
	inline void							killAndDelete(Concept* _substitute = 0) { kill(_substitute); delete this; }

	/**
	 * kill()s the deletes the object but informs any dependent it is dieing first. If you just want to kill an
	 * entity in the tree, use this rather than killAndDelete(). This will not replace the position with another
	 * entity, even if it leaves the tree in an invalid state. To keep everything valid, just use deleteAndRefill().
	 */
	inline void							killAndDeleteWithNotification() { move(Nowhere); inLimbo(); killAndDelete(); }

	/**
	 * Kills this object and replaces it with _u.
	 * All of the children are transplanted to _u.
	 * _u's allowedKinds/deniedKinds must agree with the current litter.
	 */
	Concept*								usurp(Concept* _u);
	template<class T> inline T*			usurp() { T* r = new T; usurp(r); return r; }

	/**
	 * Kills this object and replaces it with _r.
	 * All children are killed.
	 */
	Concept*								replace(Concept* _r);
	template<class T> inline T*			replace() { T* r = new T; replace(r); return r; }

	enum { NonePrefered = INT_MAX - 1 };
	/**
	 * Moves this to become the child of @a _e (at the first allowable place), and puts _e into our place.
	 * Will assert against failure.
	 * Order of preference according to firstFor().
	 * Our children are preserved.
	 *
	 * If an entity already exists in our future, non-cardinal, position (as a child of @a _e ), then it
	 * will be replaced (silently) by us. To be safe, guarantee that there is nothing in the destination
	 * index prior to this operation.
	 */
	Concept*								insert(Concept* _e, int _preferedIndex = NonePrefered);
	template<class T> inline T*			insert(int _preferedIndex = NonePrefered) { T* r = new T; insert(r, _preferedIndex); return r; }

	/**
	 * Adopts a new child entity. Puts it in the best position according to firstFor(). Will replace any
	 * entity is the best position happens to be named.
	 *
	 * To check if the child has been inserted, compare the child's parent() after this call.
	 */
	inline void							adopt(Concept* _ch) { _ch->put(firstFor(_ch->kind())); }
	/**
	 * Same as adopt() except that shove() semantics are used rather than put().
	 */
	inline void							adoptWithShove(Concept* _ch) { _ch->shove(firstFor(_ch->kind())); }

	/**
	 * Attempts to make this the child of @a _e (at the first allowable place).
	 * If no such position is found, it is removed from the model but not deleted.
	 * @returns true iff we are still in the model (as a child of @a _e).
	 * Our children are preserved.
	 */
	bool								tryInsert(Concept* _e, int _preferedIndex = NonePrefered);

	/**
	 * Safe, model-aware deletion operation. It keeps everything in order according to allowedKinds().
	 */
	void								deleteAndRefill(Concept* _e = 0, bool _moveToGrave = false);

	virtual void const*					tryKindBlind(Kind _k) const { return _k == staticKind ? reinterpret_cast<void const*>(this) : _k.isInterface() ? toInterface(_k) : 0; }

	template<class T> inline bool		isKind() const { return this && kind().isKind(T::staticKind); }
	inline bool							isKind(Kind _k) const { return this && kind().isKind(_k); }
	inline bool							isKind(Kinds _k) const { return this && kind().isKind(_k); }

	template<class T> inline T*			asInterface() { return const_cast<T*>(const_cast<Concept const*>(this)->asInterface<T>()); }
	template<class T> inline T const*	asInterface() const { AssertNR(isKind<T>()); return reinterpret_cast<T const*>(toInterface(T::staticKind)); }
	template<class T> inline T*			asKind() { AssertNR(this); AssertNR(isKind<T>()); return /*T::staticKind.isInterface() ?*/ asInterface<T>()/* : tryCast<T*>(this)*/; }
	template<class T> inline T const*	asKind() const { AssertNR(this); AssertNR(isKind<T>()); return /*T::staticKind.isInterface() ?*/ asInterface<T>()/* : tryCast<T const*>(this)*/; }
	template<class T> inline T*			tryKind() { if (this && isKind<T>()) return asKind<T>(); return 0; }
	template<class T> inline T const*	tryKind() const { if (this && isKind<T>()) return asKind<T>(); return 0; }

	// The following are information for checking and selection mechanisms.
	// These values could change depending upon elements already in place (e.g. dereference operators), so should be rechecked whenever anything changes.

	/// @note Do not call this directly; use isAllowed() on the child entity instead, since that is a more complete test.
	/// @returns the minimum number of entities than can be allowed in a entity slot, if @a _i does not equal Cardinals,
	/// or the required minimum number of cardinals, if @a _i equals Cardinals.
	/// A greater amount than these may be allowed, but never fewer.
	/// Default returns zero (i.e. no entity strictly needed).
	/// @note Do not call this directly; use isAllowed() on the child entity instead, since that is a more complete test.
	virtual int							minRequired(int = Cardinals) const { return 0; }
	/// @returns the kind of entities allowed in the given child position.
	/// Reimplement to allow particular entity kinds.
	/// Default returns the empty list (i.e. entity may not have any children).
	/// @note Do not call this directly; use isAllowed() on the child entity instead, since that is a more complete test.
	virtual Kinds						allowedKinds(int) const { return Kinds(); }
	/// @returns the kind of entities explicitly denied in the given child position over those listed in allowedKinds().
	/// Reimplement to restrict particular entity kinds.
	/// Default returns the empty list (i.e. no special restrictions).
	/// @note Do not call this directly; use isAllowed() on the child entity instead, since that is a more complete test.
	virtual Kinds						deniedKinds(int) const { return Kinds(); }
	/// @returns true if we have an allowable state (according to us).
	/// @note Derivations should honour the decision of their Super should it be true.
	/// @note Do not call this directly; use isValid() on the child entity instead, since that is a more complete test.
	virtual bool						isInValidState() const { return true; }
	/// @returns true if parentheses are need around the expression.
	// TODO: figure out if parentheses are needed
	// TODO: Consider moving to somewhere more specialised.
	virtual bool						doINeedParenthesising(Concept const* _child) const { (void)_child; return false; }
	/// @returns true if this object isn't actually a proper language entity.
	/// Overrides automatically handled in derivations by MARTTA_OBJECT(_INTERFACE) macros. Do not reimplement.
	virtual bool						isPlaceholder() const { return kind().isPlaceholder(); }
	/// Checked after change; if true is returned, this may be deleted.
	virtual bool						isSuperfluous() const;
	bool								isFixed() const { return over().isFixed(); }
	virtual bool						defineChildFixed(int) const { return false; }

	// Tests; these use the above information in order to determine the answer.

	/// @returns true if kind _o is allowed at entity position _i.
	/// Takes into account parent's allowedKinds(int) and this object's allowedAncestor.
	bool								isAllowed(int _i, Kind _o) const;
	/// Convenience version of above.
	/// Takes into account parent's allowedKinds(int) and this object's allowedAncestor.
	template<class T> inline bool		isAllowed(int _i) const { return isAllowed(_i, Kind::of<T>()); }

	/// Takes into account parent's allowedKinds(int) and this object's allowedAncestor.
	inline bool							isAllowed() const { return over().allowedToBeKind(kind()); }

	/// @returns true if this entity cannot be killed without costing the validity of its parent's children set.
	bool								isNecessary() const;
	/// @returns true if all entity positions are filled validly, including all those required.
	/// @warning This uses allowedKinds() and minRequired, and so is not safe to be called from your reimplementation of
	/// these or anything that they (indirectly) use.
	bool								isComplete() const;
	/// @returns true if object is current in some sort of legitimate (change-tracking) heirarchy.
	virtual bool						isInModel() const { return true; }
	/// @returns true if everything is absolutely, 100%, fine.
	bool								isValid() const;

	// Insertion points
	inline Position						over() const { return Position(parent(), index()); }
	inline Position						front() { return Position(this, 0); }
	inline Position						back() { return Position(this, UndefinedIndex); }
	inline Position						middle(int _at) { return Position(this, _at); }
	Position							named(String const& _name);

	/**
	 * Order of preference:
	 * - A named position that requires us to be there.
	 * - A placeholder of a cardinal position.
	 * - The back of the cardinals that requires us to be there.
	 * - A named position that allows us being there.
	 * - The back of the cardinals if it allows us being there.
	 * - Nowhere
	 */
	Position							firstFor(Kind const& _k);
	bool								attemptAppend(KeyEvent const* _e);
	bool								attemptInsert(KeyEvent const* _e);

	virtual int							definePreferedFor(Kind const&) const { return NonePrefered; }

	// Save/load
	virtual List<Concept*>				savedChildren() const;
	virtual void						properties(Hash<String, String>&) const {}
	virtual void						setProperties(Hash<String, String> const&) {}
	virtual void						apresLoad() {}	// Called following a load after the model has been loaded.
	virtual void						archive() { Hash<String, String> h; properties(h); setProperties(h); }	// Should (at least) force all ModelPtrs to revert to textual form.

	// We've been double-clicked.
	virtual bool						keyPressed(KeyEvent const*);
	static bool							keyPressedOnPosition(Position const&, KeyEvent const*) { return false; }

	String								indexName(int _i) const;
	String								indexName() const { return m_parent ? m_parent->indexName(m_index) : "[Orphan]"; }
	String								namedIndexId() const;		// number for static named, alpha for dynamic named, null for cardinal.
	List<int>							knownNames() const;

	/// These apply to all CodeScenes in operation.
	void								setCurrent();
	void								dropCursor();
	bool								isCurrent() const;
	bool								isCurrentOrAncestor() const;
	bool								isEditing() const;
	void								clearEditing();

	virtual EditDelegateFace*			newDelegate(CodeScene*) { return 0; }

	static void							keyPressEventStarter(KeyEvent* _e, bool _abortive = false);

	/// Static helpers.
	inline static bool					isTemporary(Concept* _e) { SafePointer<Concept> p(_e); p->clearEditing(); return !p; }
	static bool							isValidName(String const& _n);
	template<class T>
	inline static bool					simplePlaceholderKeyPressHandler(Position const& _p, KeyEvent const* _e, String const& _t);
	template<class T>
	inline static bool					simplePositionKeyPressHandler(Position const& _p, KeyEvent const* _e, String const& _t, bool _ontoNew = true);

	void								debugTree() const;
	void								debugTree(String const& _i) const;

protected:
	virtual ~Concept();

	/// Convenience methods.
	/// Notes parentSwitched to child, and child removal to old parent.
	/// @important This does *not* notify the new parent of the child addition; you must do that yourself!
	/// This assumes _old is still valid!
	inline void							parentSwitchedWithChildRemoved(Position const& _old)
	{
		if (m_parent && _old.parent())
			parentSwitched(_old.parent());
		else if (_old.parent())
			parentRemoved(_old.parent());
		else if (m_parent)
			parentAdded();
		if (_old.parent())
			_old.parent()->childRemoved(this, _old.index());
	}
	/// Given an Concept/Interface-pointer-style variable and a value, set the variable and call add, change and remove
	/// dependency as necessary.
	template<class T, class U> void		setDependency(T& _dependencyVariable, U const& _dependency);

private:
	/** This will insert an entity @a _child into our brood at index @a _childsIndex.
	 * It will make sure all children have the correct index but nothing more. In particular
	 * the new child's parent and rootConcept will not be updated.
	 *
	 * This handles the case of negative and positive child indices, and the index may be
	 * UndefinedIndex, in which case the child is appended.
	 */
	void								insertIntoBrood(int _childsIndex, Concept* _child);
	/** This removes a previously inserted @a _child from our brood.
	 * It makes sure all children have the correct index but nothing more. In particular
	 * the old child's index, parent and rootConcept are not updated.
	 *
	 * This handles the case of negative and position child indices.
	 */
	void								removeFromBrood(int _childsIndex, Concept* _child);
	/** Removes all children at index @a _childsIndex from the brood of children.
	 *
	 * It makes sure all children have the correct index but nothing more. In particular
	 * the old children's index, parent and rootConcept are not updated.
	 *
	 * This handles the case of negative and position child indices.
	 */
	void								removeAllFromBrood(int _childsIndex);
	/**
	 * Just moves the child of index @a _old in the brood to have index @a _new, still within the brood.
	 *
	 * It makes sure all children have the correct index but nothing more. In particular
	 * the old children's index, parent and rootConcept are not updated.
	 *
	 * This handles the case of negative and position child indices.
	 */
	void								moveWithinBrood(int _old, int _new);

	/// Helper function for validifyChildren()
	bool								validifyChild(int _i, int* _added);

	void								notifyOfStrobe(Concept* _strobeCreation);

	void								inLimbo(Dependee* _replacement = 0);

	Concept*								m_parent;
	int									m_index;
	List<Concept*>						m_cardinalChildren;
	Hash<int, Concept*>					m_namedChildren;
};

M_API_Concept extern int s_news;
M_API_Concept extern int s_deletes;

inline MarttaSupport::TextStream& operator<<(MarttaSupport::TextStream& _out, const Kind& _item)
{
	return _out << _item.name();
}

inline MarttaSupport::TextStream& operator<<(MarttaSupport::TextStream& _out, const Concept& _item)
{
	return _out << _item.kind() << "(*" << (&_item) << ")";
}

inline MarttaSupport::TextStream& operator<<(MarttaSupport::TextStream& _out, const Concept* _item)
{
	if (_item)
		return _out << _item->kind() << "*(" << ((void*)_item) << ")";
	else
		return _out << "Concept*( 0 )";
}

}

/*
inline QDebug operator<<(QDebug _out, const Martta::Kind& _item)
{
	return _out << _item.name();
}

inline QDebug operator<<(QDebug _out, const Martta::Concept& _item)
{
	return _out << _item.kind() << "(*" << (&_item) << ")";
}

inline QDebug operator<<(QDebug _out, const Martta::Concept* _item)
{
	if (_item)
		return _out << _item->kind() << "*(" << ((void*)_item) << ")";
	else
		return _out << "Concept *( 0 )";
}
*/
template<class T, class U>
void Martta::Concept::setDependency(T& _dependencyVariable, U const& _dependency)
{
	Concept* old = _dependencyVariable->tryKind<Concept>();
//	AssertNR(!_dependency || _dependency->asKind<Concept>()->isInModel());	// Can also be a SimpleConcept - probably best to virtualise.
	AssertNR(!old || old->isInModel());
	if (old)
		removeDependency(old);
	_dependencyVariable = _dependency;
	if (_dependency)
		addDependency(_dependency->asKind<Concept>());
	if (old != _dependency->tryKind<Concept>())
		changed();
}

template<class T>
bool Martta::Concept::simplePlaceholderKeyPressHandler(Position const& _p, KeyEvent const* _e, String const& _t)
{
	if (_e->text() == _t && _p.allowedToBeKind<T>() && _p.exists() && _p->isPlaceholder())
	{
		Concept* e = Kind::of<T>().spawnPrepared();
		_p.place(e);
		_e->codeScene()->navigateInto(e);
		return true;
	}
	return false;
}

template<class T>
bool Martta::Concept::simplePositionKeyPressHandler(Position const& _p, KeyEvent const* _e, String const& _t, bool _ontoNew)
{
	if (_e->text() == _t && _p.allowedToBeKind<T>())
	{
		T* n = new T;
		if (_p.exists() && !_p->isPlaceholder() && n->firstFor(_p->kind()) != Nowhere)
		{
			// insert
			// when pressed on _p refers to a, changes from x->(a->(d, e), b, c) to x->(N->(a->(d, e)), b, c)
			Concept* e = _p.entity();
			mDebug() << e;
			mDebug() << n;
			_e->noteStrobeCreation(e, n);
			e->insert(n);
			n->prepareChildren();
			if (_ontoNew)
				_e->codeScene()->navigateToNew(n);
			else
				n->setCurrent();
		}
		else if (!_p.exists() || _p->isPlaceholder() || _p.index() >= 0)	// if it's not cardinal we'll end up pushing the new thing behind the current.
		{
			// when pressed on _p which already exists, changes from x->(a->(d, e), b, c) to x->(N, a->(d, e), b, c)
			// when pressed on _p which doesn't exist yet or is a placeholder (a), changes from x->([a,] b, c) to x->(N, b, c)
			n->prepareChildren();
			_p.place(n);
			_e->codeScene()->navigateInto(n);
		}
		else
			delete n;
	}
	else
		return false;
	return true;
}
