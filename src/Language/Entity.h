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

#include <QDebug>			// Move to mDebug & link in.
#include <QRgb>
#include <qglobal.h>

#include <msList.h>
#include <msHash.h>
#include <msString.h>
using MarttaSupport::Hash;
using MarttaSupport::MultiHash;
using MarttaSupport::List;
using MarttaSupport::String;

// Same here - move to interface?
#include "EntityKeyEvent.h"	// Depends on QKeyEvent

#include "ChildValidifier.h"
#include "Depender.h"
#include "Dependee.h"
#include "Serialisable.h"

#include "Dier.h"
#include "Meta.h"
#include "Position.h"
#include "SafePointer.h"

class QTextStream;
class QDomElement;

namespace Martta
{

class EntityStylist;

class EditDelegateFace;
class CodeScene;
class DecorationContext;

/**
 * Note regarding rootEntity/null-Context: You can never depend on something which does not share the
 * same Root object as you. Objects yet to be inserted into the program have a nullContext and thus
 * no Root. If you make objects within the scene children of such objects they will be moved
 * out of scene, and thus can have no interactions (i.e. dependencies/references) with objects within the
 * scene. This applies even if the situation is temporary, since the check/changes happen at move
 * time.
 */
class Entity: public Nothing, public SafePointerTarget, virtual public Dier, public_interface ChildValidifier, public_interface Depender, public_interface Dependee, public_interface Serialisable
{
	MARTTA_COMMON(Nothing)
	MARTTA_INHERITS(ChildValidifier, 0)
	MARTTA_INHERITS(Dependee, 1)
	MARTTA_INHERITS(Depender, 2)
	MARTTA_INHERITS(Serialisable, 3)
	
	friend class EntityStylist;
	friend class EditDelegateFace;
	friend class CodeScene;

protected:
	enum { EndOfNamed = INT_MIN, Cardinals = 0 };
	
public:
	static const bool					IsInterface = false;
	static const bool					IsPlaceholder = true;
	static const bool					IsObject = false;
	
	inline void*						operator new(size_t _size);
	inline void							operator delete(void* p);
	
	/// Copy constructor which doesn't do anything. Have to have it so a derived class can use it.
	inline Entity(): Dier(), ChildValidifier(), Depender(), Dependee(), Serialisable(), SafePointerTarget(), m_parent(0), m_index(UndefinedIndex) {}
	inline Entity(Entity const&): Dier(), ChildValidifier(), Familial(), Depender(), Dependee(), Serialisable(), SafePointerTarget() { M_ASSERT(false); }
	
	static void							initialiseClass() {}
	static void							finaliseClass() {}
	
	void								prepareMove(Position const& _newPosition);
	void								commitMove(Position const& _oldPosition);
	void								silentMove(Position const& _to) { Position from = over(); prepareMove(_to); commitMove(from); }
	void								silentRemove() { silentMove(Nowhere); }
	void								move(Position const& _newPosition);
	/// Same as move() except that if the destination is a placeholder we replace it, 
	void								put(Position const& _newPosition);

	inline int							index() const { return m_index; }
	inline Entity*						parent() const { return m_parent; }
	template<class T> inline bool		parentIs() const { return m_parent ? m_parent->isKind<T>() : false; }
	template<class T> inline T*			parentAs() const { M_ASSERT(m_parent); return m_parent->asKind<T>(); }
	template<class T> inline T*			tryParent() const { return m_parent ? m_parent->tryKind<T>() : 0; }
	
	inline int							childCount() const { return m_namedChildren.size() + m_cardinalChildren.size(); }
	inline List<Entity*>				children() const { return m_namedChildren.values() + m_cardinalChildren; }
	inline Entity*						child(int _i) const { if (_i >= 0 && _i < m_cardinalChildren.size()) { M_ASSERT(m_cardinalChildren[_i]->m_parent == this); return m_cardinalChildren[_i]; } else return _i < 0 ? m_namedChildren.value(_i) : 0; }
	inline int							childCount(int _i) const { if (_i >= 0) return _i < m_cardinalChildren.size() ? 1 : 0; int r = 0; Hash<int, Entity*>::ConstIterator i = m_namedChildren.find(_i); while (i != m_namedChildren.end() && i.key() == _i) ++r, ++i; return r; }
	inline List<Entity*>				children(int _i) const { if(_i < 0) return m_namedChildren.values(_i); if (_i < m_cardinalChildren.size()) return List<Entity*>() << child(_i); return List<Entity*>(); }
	template<class T> inline List<T*>	childrenOf() const { return filterEntities<T>(children()); }
	template<class T> inline List<T*>	childrenAs() const { return castEntities<T>(children()); }
	template<class T> inline List<T*>	childrenOf(int _i) const { return filterEntities<T>(children(_i)); }
	template<class T> inline List<T*>	childrenAs(int _i) const { return castEntities<T>(children(_i)); }
	template<class T> inline int		childIndexOf() const { foreach (Entity* e, children()) if (e->isKind<T>()) return e->m_index; return UndefinedIndex; }
	template<class T> inline int		childCountOf() const { int r = 0; foreach (Entity* i, children()) if (i->isKind<T>()) r++; return r; }
	template<class T> inline int		childCountOf(int _i) const { int r = 0; foreach (Entity* i, children(_i)) if (i->isKind<T>()) r++; return r; }
	template<class T> inline T*			childOf() const { M_ASSERT(childCountOf<T>()); return childAs<T>(childIndexOf<T>()); }
	template<class T> inline T*			childOf(int _i) const { M_ASSERT(childCountOf<T>(_i)); foreach (Entity* i, children(_i)) if (T* r = i->tryKind<T>()) return r; return 0; } 
	template<class T> inline bool		childIs(int _i) const { if (Entity* r = child(_i)) return r->isKind<T>(); return false; }
	template<class T> inline T*			childAs(int _i) const { Entity* e = child(_i); M_ASSERT(e); return e->asKind<T>(); }
	template<class T> inline T*			tryChild(int _i) const { if (Entity* e = child(_i)) return e->tryKind<T>(); return 0; }
	
	inline List<Entity*> const&		cardinalChildren() const { return m_cardinalChildren; }
	inline int							cardinalChildCount() const { return m_cardinalChildren.size(); }
	template<class T> inline List<T*>	cardinalChildrenOf() const { return filterEntities<T>(m_cardinalChildren); }
	template<class T> inline List<T*>	cardinalChildrenAs() const { return castEntities<T>(m_cardinalChildren); }
	template<class T> inline T*			cardinalChildOf() const { M_ASSERT(cardinalChildCountOf<T>()); return childAs<T>(cardinalChildIndexOf<T>()); }
	template<class T> inline int		cardinalChildIndexOf() const { for (int r = 0; r < m_cardinalChildren.size(); ++r) if (childIs<T>(r)) return r; return UndefinedIndex; }
	template<class T> inline int		cardinalChildCountOf() const { int r = 0; foreach (Entity* i, m_cardinalChildren) if (i->isKind<T>()) r++; return r; }

	inline Entity*						sibling(int _i) const { return m_parent ? m_parent->child(_i) : 0; }
	template<class T> inline bool		siblingIs(int _i) const { return m_parent ? m_parent->childIs<T>(_i) : false; }
	template<class T> inline T*			siblingAs(int _i) const { M_ASSERT(m_parent); return m_parent->childAs<T>(_i); }
	template<class T> inline T*			trySibling(int _i) const { return m_parent ? m_parent->tryChild<T>(_i) : 0; }
	inline List<Entity*>				siblings() const { if (!m_parent) return List<Entity*>(); List<Entity*> ret; foreach (Entity* e, m_parent->children()) if (e != this) ret += e; return ret; }
	template<class T> inline List<T*>	siblingsOf() const { return filterEntities<T>(siblings()); }
	inline int							siblingCount() const { return m_parent ? m_parent->childCount() - 1 : 0; }
	inline List<Entity*>				cardinalSiblings() const { if (!m_parent) return List<Entity*>(); List<Entity*> ret; foreach (Entity* e, m_parent->m_cardinalChildren) if (e != this) ret += e; return ret; }
	template<class T> inline List<T*>	cardinalSiblingsOf() const { return filterEntities<T>(cardinalSiblings());  }
	inline int							cardinalSiblingCount() const { return m_parent ? m_parent->cardinalChildCount() - (m_index < 0 ? 0 : 1) : 0; }
	inline List<Entity*>				parentsChildren() const { return m_parent ? m_parent->children() : List<Entity*>(); }
	template<class T> inline List<T*>	parentsChildrenOf() const { return m_parent ? m_parent->childrenOf<T>() : List<T*>(); }
	inline int							parentsChildCount() const { return m_parent ? m_parent->childCount() : 0; }
	inline List<Entity*>				parentsCardinalChildren() const { return m_parent ? m_parent->cardinalChildren() : List<Entity*>(); }
	template<class T> inline List<T*>	parentsCardinalChildrenOf() const { return m_parent ? m_parent->cardinalChildrenOf<T>() : List<T*>(); }
	inline int							parentsCardinalChildCount() const { M_ASSERT(m_parent); return m_parent->m_cardinalChildren.size(); }

	template<class T> bool				hasAncestor() const { return hasAncestor(T::staticKind); }
	bool								hasAncestor(Kind _k) const;
	bool								hasAncestor(Entity const* _a) const { return ancestorIndex(_a) != UndefinedIndex; }
	template<class T> T*				ancestor() const { Entity* r = ancestor(T::staticKind); return r ? r->asKind<T>() : 0; }
	Entity*								ancestor(Kind _k) const;
	template<class T> int				ancestorIndex() const { return ancestorIndex(T::staticKind); }
	int									ancestorIndex(Kind _k) const;
	int									ancestorIndex(Entity const* _a) const;
	
	template<class T> bool				hasSelfAncestor() const { return hasSelfAncestor(T::staticKind); }
	bool								hasSelfAncestor(Kind _k) const;
	bool								hasSelfAncestor(Entity const* _a) const;
	template<class T> T*				selfAncestor() const { Entity* r = selfAncestor(T::staticKind); return r ? r->asKind<T>() : 0; }
	Entity*								selfAncestor(Kind _k) const;
	inline Entity*						root() const { Entity* e; for (e = const_cast<Entity*>(this); e->m_parent; e = e->m_parent) {} return e; }
	template<class T> T*				rootOf() const { T* r = tryKind<T>(); for (Entity* e = const_cast<Entity*>(this); e; e = e->m_parent) if (T* t = e->tryKind<T>()) r = t; return r; }
	
	template<class T> List<T*>			selfAndAncestorsChildrenOf() const { List<T*> ret = childrenOf<T>(); return parent() ? ret + parent()->selfAndAncestorsChildrenOf<T>() : ret; }
	
	inline int							nonPlaceholderCount() const { int ret = 0; foreach (Entity* i, m_cardinalChildren) if (!i->isPlaceholder()) ret++; return ret; }
	inline List<Entity*>				nonPlaceholders() const { List<Entity*> ret; foreach (Entity* i, m_cardinalChildren) if (!i->isPlaceholder()) ret += i; return ret; }
	inline Entity*						nonPlaceholder(int _i) const { int c = 0; foreach (Entity* i, m_cardinalChildren) if (c++ == _i) return i; M_ASSERT(false); return 0; }

	virtual bool						usurpsChild(Entity const*) const { return false; }
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
	Entity*								prepareChildren();
	
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
	inline static Entity*				spawn(String const& _kind) { AuxilliaryFace const* f = AuxilliaryRegistrar::get()->auxilliary(_kind); M_ASSERT(f); return f->create(); }

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
	void								kill(Entity* _substitute = 0);
	
	/**
	 * kill()s and deletes the object.
	 * 
	 * Convenience function for "kill(); delete this;".
	 */
	inline void							killAndDelete(Entity* _substitute = 0) { kill(_substitute); delete this; }
	/**
	 * kill()s the deletes the object but informs any dependent it is dieing first.
	 */
	inline void							killAndDeleteWithNotification() { move(Nowhere); oneFootInTheGrave(); killAndDelete(); }

	/**
	 * Kills this object and replaces it with _u.
	 * All of the children are transplanted to _u.
	 * _u's allowedKinds/deniedKinds must agree with the current litter.
	 */
	Entity*								usurp(Entity* _u);
	template<class T> inline T*			usurp() { T* r = new T; usurp(r); return r; }
		
	/**
	 * Kills this object and replaces it with _r.
	 * All children are killed.
	 */
	Entity*								replace(Entity* _r);
	template<class T> inline T*			replace() { T* r = new T; replace(r); return r; }
	
	enum { NonePrefered = INT_MAX - 1 };
	/**
	 * Moves this to become the child of @a _e (at the first allowable place), and puts _e into our place.
	 * Will assert against failure.
	 * Order of preference according to firstFor().
	 * Our children are preserved.
	 */
	Entity*								insert(Entity* _e, int _preferedIndex = NonePrefered);
	template<class T> inline T*			insert(int _preferedIndex = NonePrefered) { T* r = new T; insert(r, _preferedIndex); return r; }
	
	/**
	 * Attempts to make this the child of @a _e (at the first allowable place).
	 * If no such position is found, it is removed from the model but not deleted.
	 * @returns true iff we are still in the model (as a child of @a _e).
	 * Our children are preserved.
	 */
	bool								tryInsert(Entity* _e, int _preferedIndex = NonePrefered);
	
	/**
	 * Safe, model-aware deletion operation. It keeps everything in order according to allowedKinds().
	 */
	void								deleteAndRefill(Entity* _e = 0, bool _moveToGrave = false);

	template<class T> inline bool		isKind() const { return this && kind().isKind(T::staticKind); }
	inline bool							isKind(Kind _k) const { return this && kind().isKind(_k); }
	inline bool							isKind(Kinds _k) const { return this && kind().isKind(_k); }
	
	template<class T> inline T*			asInterface() { return const_cast<T*>(const_cast<Entity const*>(this)->asInterface<T>()); }
	template<class T> inline T const*	asInterface() const { M_ASSERT(isKind<T>()); return reinterpret_cast<T const*>(toInterface(T::staticKind)); }
	template<class T> inline T*			asKind() { M_ASSERT(this); M_ASSERT(isKind<T>()); return T::IsInterface ? asInterface<T>() : tryCast<T*>(this); }
	template<class T> inline T const*	asKind() const { M_ASSERT(this); M_ASSERT(isKind<T>()); return T::IsInterface ? asInterface<T>() : tryCast<T const*>(this); }
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
	virtual bool						doINeedParenthesising(Entity const* _child) const { (void)_child; return false; }
	/// @returns true if this object isn't actually a real language entity.
	/// Overrides automatically handled in derivations by MARTTA_OBJECT(_INTERFACE) macros. Do not reimplement.
	virtual bool						isPlaceholder() const { return true; }
	/// Checked after change; if true is returned, this may be deleted.
	virtual bool						isSuperfluous() const;
	
	// Tests; these use the above information in order to determine the answer.
	
	/// @returns true if kind _o is allowed at entity position _i.
	/// Takes into account parent's allowedKinds(int) and this object's allowedAncestor.
	bool								isAllowed(int _i, Kind _o) const;
	/// Convenience version of above.
	/// Takes into account parent's allowedKinds(int) and this object's allowedAncestor.
	template<class T> inline bool		isAllowed(int _i) const { return isAllowed(_i, Kind::of<T>()); }
	/// @returns true if this entity would be valid if it were an object of a particular type.
	/// Takes into account parent's allowedKinds(int) and this object's allowedAncestor.
	inline bool							isAllowed(Kind _o) const { return parent() ? parent()->isAllowed(index(), _o) : true; }
	/// Convenience version of above.
	/// Takes into account parent's allowedKinds(int) and this object's allowedAncestor.
	template<class T> inline bool		isAllowed() const { return isAllowed(Kind::of<T>()); }
	/// @returns true if this entity, in its current position, is valid.
	/// Takes into account parent's allowedKinds(int) and this object's allowedAncestor.
	inline bool							isAllowed() const { return isAllowed(kind()); }
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
	bool								attemptAppend(EntityKeyEvent const* _e);
	bool								attemptInsert(EntityKeyEvent const* _e);
	
	virtual int							definePreferedFor(Kind const&) const { return NonePrefered; }

	// Save/load
	virtual void						exportDom(QDomElement& _element) const;
	virtual void						importDom(QDomElement const& _element);

	// UI
	/// 
	virtual void						decorate(DecorationContext const&) const;
	virtual EditDelegateFace*			newDelegate(CodeScene*) { return 0; }
	// We've been double-clicked.
	bool								activated(CodeScene* _s);
	virtual bool						onActivated(CodeScene*) { return false; }
	virtual bool						keyPressed(EntityKeyEvent const*);
	static bool							keyPressedOnPosition(Position const&, EntityKeyEvent const*) { return false; }
	
	String								indexName(int _i) const;
	String								indexName() const { return m_parent ? m_parent->indexName(m_index) : "[Orphan]"; }
	List<int>							knownNames() const;
	
	virtual String						defineLayout(ViewKeys const&) const;
	virtual Entity*						isExpander() const { return 0; }
	
	/// We become current in all code scenes.
	void								setCurrent();
	void								setEditing(CodeScene* _s);
	bool								isCurrent() const;
	bool								isCurrent(CodeScene* _s) const;
	bool								isCurrentOrAncestor() const;
	bool								isCurrentOrAncestor(CodeScene* _s) const;
	bool								isEditing() const;
	bool								isEditing(CodeScene* _s) const;
	EditDelegateFace*					editDelegate(CodeScene* _s);
	void								clearEditing();
	void								navigateInto(CodeScene* _s);
	void								navigateOnto(CodeScene* _s);
	void								navigateToNew(CodeScene* _s);
	void								dropCursor();
	/// Redraw ourself in all views.
	void								repaint();
	/// Re-layout ourself now in all views.
	void								relayout();
	/// Re-layout ourself sometime before the next paint cycle in all views.
	void								relayoutLater();
	/// Redraw ourself in one view.
	void								repaint(CodeScene* _s);
	/// Re-layout ourself now in one view.
	void								relayout(CodeScene* _s);
	/// Re-layout ourself sometime before the next paint cycle in one view.
	void								relayoutLater(CodeScene* _s);
	/// Reset-layout cache.
	void								resetLayoutCache();
	
	static void							keyPressEventStarter(EntityKeyEvent* _e, bool _abortive = false);
	void								keyPressEvent(EntityKeyEvent* _e);
	void								activateEvent(CodeScene* _s);
	
	/// Static helpers.
	inline static bool					isTemporary(Entity* _e) { SafePointer<Entity> p(_e); p->clearEditing(); return !p; }
	static bool							isValidName(String const& _n);
	template<class T>
	inline static bool					simplePlaceholderKeyPressHandler(Position const& _p, EntityKeyEvent const* _e, String const& _t);
	template<class T>
	inline static bool					simplePositionKeyPressHandler(Position const& _p, EntityKeyEvent const* _e, String const& _t, bool _ontoNew = true);	

	void								debugTree() const;
	void								debugTree(String const& _i) const;
	
	/// Called directly following a load.
	virtual void						apresLoad() {}
	
protected:
	virtual ~Entity();

	/// Convenience methods.
	/// Notes parentSwitched to child, and child removal to old parent.
	/// @important This does *not* notify the new parent of the child addition; you must do that yourself! 
	/// This assumes _old is still valid!
	inline void							parentSwitchedWithChildRemoved(Position const& _old)
	{
		if (m_parent && _old.parent())
		{
			parentSwitched(_old.parent());
			m_parent->resetLayoutCache();
		}
		else if (_old.parent())
			parentRemoved(_old.parent());
		else if (m_parent)
		{
			parentAdded();
			m_parent->resetLayoutCache();
		}
		if (_old.parent())
		{
			_old.parent()->childRemoved(this, _old.index());
			_old.parent()->resetLayoutCache();
		}
	}
	/// Given an Entity/Interface-pointer-style variable and a value, set the variable and call add, change and remove
	/// dependency as necessary.
	template<class T, class U> void		setDependency(T& _dependencyVariable, U const& _dependency);
	
private:
	/** This will insert an entity @a _child into our brood at index @a _childsIndex.
	 * It will make sure all children have the correct index but nothing more. In particular
	 * the new child's parent and rootEntity will not be updated.
	 * 
	 * This handles the case of negative and positive child indices, and the index may be
	 * UndefinedIndex, in which case the child is appended.
	 */
	void								insertIntoBrood(int _childsIndex, Entity* _child);
	/** This removes a previously inserted @a _child from our brood.
	 * It makes sure all children have the correct index but nothing more. In particular
	 * the old child's index, parent and rootEntity are not updated.
	 * 
	 * This handles the case of negative and position child indices.
	 */
	void								removeFromBrood(int _childsIndex, Entity* _child);
	/** Removes all children at index @a _childsIndex from the brood of children.
	 * 
	 * It makes sure all children have the correct index but nothing more. In particular
	 * the old children's index, parent and rootEntity are not updated.
	 * 
	 * This handles the case of negative and position child indices.
	 */
	void								removeAllFromBrood(int _childsIndex);
	/**
	 * Just moves the child of index @a _old in the brood to have index @a _new, still within the brood.
	 * 
	 * It makes sure all children have the correct index but nothing more. In particular
	 * the old children's index, parent and rootEntity are not updated.
	 * 
	 * This handles the case of negative and position child indices.
	 */
	void								moveWithinBrood(int _old, int _new);
	
	/// Helper function for validifyChildren()
	bool								validifyChild(int _i, int* _added);
	
	void								notifyOfStrobe(Entity* _strobeCreation);

	Entity*								m_parent;
	int									m_index;
	List<Entity*>						m_cardinalChildren;
	Hash<int, Entity*>					m_namedChildren;
};

class EntityStylist
{
public:
	virtual ~EntityStylist() {}
	
	virtual String defineLayout(Entity* _e, ViewKeys const& _k)
	{
		return _e->defineLayout(_k);
	}
};

extern int s_news;
extern int s_deletes;

}

void* Martta::Entity::operator new(size_t _size)
{
	s_news++;
	void* ret = malloc(_size);
	return ret;
}

void Martta::Entity::operator delete(void* p)
{
	s_deletes++;
	free(p);
}

inline QDebug operator<<(QDebug _out, const Martta::Kind& _item)
{
	return _out << _item.name();
}

inline QDebug operator<<(QDebug _out, const Martta::Entity& _item)
{
	return _out << _item.kind() << "(*" << (&_item) << ")";
}

inline QDebug operator<<(QDebug _out, const Martta::Entity* _item)
{
	if (_item)
		return _out << _item->kind() << "*(" << ((void*)_item) << ")";
	else
		return _out << "Entity *( 0 )";
}

template<class T, class U>
void Martta::Entity::setDependency(T& _dependencyVariable, U const& _dependency)
{
	if (_dependencyVariable != _dependency)
	{
		Entity* old = _dependencyVariable->asKind<Entity>();
//		M_ASSERT(!_dependency || _dependency->asKind<Entity>()->isInModel());	// Can also be a SimpleEntity - probably best to virtualise.
		M_ASSERT(!old || old->asKind<Entity>()->isInModel());
		if (old)
			removeDependency(old);
		_dependencyVariable = _dependency;
		if (_dependency)
		{
			addDependency(_dependency->asKind<Entity>());
			if (old)
				dependencySwitched(_dependency->asKind<Entity>(), old);
		}
		changed();
	}
}

template<class T>
bool Martta::Entity::simplePlaceholderKeyPressHandler(Position const& _p, EntityKeyEvent const* _e, String const& _t)
{
	if (_e->text() == _t && _p.allowedToBeKind<T>() && _p.exists() && _p->isPlaceholder())
	{
		Entity* e = Kind::of<T>().spawnPrepared();
		_p.place(e);
		e->navigateInto(_e->codeScene());
		return true;
	}
	return false;
}

template<class T>
bool Martta::Entity::simplePositionKeyPressHandler(Position const& _p, EntityKeyEvent const* _e, String const& _t, bool _ontoNew)
{
	if (_e->text() == _t && _p.allowedToBeKind<T>())
	{
		T* n = new T;
		if (_p.exists() && !_p->isPlaceholder() && n->firstFor(_p->kind()) != Nowhere)
		{
			// insert
			// when pressed on _p refers to a, changes from x->(a->(d, e), b, c) to x->(N->(a->(d, e)), b, c)
			Entity* e = _p.entity();
			_e->noteStrobeCreation(e, n);
			e->insert(n);
			n->prepareChildren();
			if (_ontoNew)
				n->navigateToNew(_e->codeScene());
			else
				n->setCurrent();
		}
		else
		{
			// when pressed on _p which already exists, changes from x->(a->(d, e), b, c) to x->(N, a->(d, e), b, c)
			// when pressed on _p which doesn't exist yet or is a placeholder (a), changes from x->([a,] b, c) to x->(N, b, c)
			n->prepareChildren();
			_p.place(n);
			n->navigateInto(_e->codeScene());
		}
	}
	else
		return false;
	return true;
}
