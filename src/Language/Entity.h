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

#include <QDebug>
#include <QList>
#include <QHash>

// Same here - move to interface?
#include "EntityKeyEvent.h"

#include "SceneLeaver.h"
#include "ChildValidifier.h"
#include "Familial.h"

#include "Meta.h"
#include "InsertionPoint.h"
#include "SafePointer.h"

class QTextStream;
class QDomElement;

namespace Martta
{

class EntityStylist;
class EditDelegateFace;

class CodeScene;
class DecorationContext;

// CHANGE: Moves to ChangeMan
enum ChangeOperation
{
	EntityChanged = 0,
	EntityChildrenInitialised,
	DependencyAdded,
	DependencyRemoved,
	DependencyChanged,
	DependencySwitched,
	ChildMoved,
	ContextIndexChanged
};
class ChangeEntry
{
public:
	ChangeEntry(Entity* _s, ChangeOperation _op, Entity* _o = 0): m_subject(_s), m_object(_o), m_op(_op) {}
	Entity* m_subject;
	Entity* m_object;
	ChangeOperation m_op;
};
extern QList<ChangeEntry> s_changes;
void change(Entity* _s, ChangeOperation _op, Entity* _o = 0);
inline QList<ChangeEntry> const& changes() { return s_changes; }
inline void clearChanges() { s_changes.clear(); }

class BasicRoot;

/**
 * Note regarding rootEntity/null-Context: You can never depend on something which does not share the
 * same BasicRoot object as you. Objects yet to be inserted into the program have a nullContext and thus
 * no BasicRoot. If you make objects within the scene children of such objects they will be moved
 * out of scene, and thus can have no interactions (i.e. dependencies/references) with objects within the
 * scene. This applies even if the situation is temporary, since the check/changes happen at move
 * time.
 */
class Entity: public Nothing, public SafePointerTarget, public_interface SceneLeaver, public_interface ChildValidifier, public_interface Familial
{
	MARTTA_COMMON(Nothing)
	MARTTA_INHERITS(SceneLeaver, 0)
	MARTTA_INHERITS(ChildValidifier, 1)
	MARTTA_INHERITS(Familial, 2)
//	MARTTA_INHERITS(Dependee, 2)
//	MARTTA_INHERITS(Depender, 3)
	
	friend class EntityStylist;
	friend class EditDelegateFace;

protected:
	enum { EndOfNamed = INT_MIN, Cardinals = 0 };
	
public:
	static const bool					IsInterface = false;
	static const bool					IsPlaceholder = true;
	static const bool					IsObject = false;
	
	inline void*						operator new(size_t _size);
	inline void							operator delete(void* p);
	
	/// Copy constructor which doesn't do anything. Have to have it so a derived class can use it.
	inline Entity(): SceneLeaver(), ChildValidifier(), Familial(), SafePointerTarget(), m_rootEntity(0), m_parent(0), m_index(UndefinedIndex), m_notifiedOfChange(0) {}
	inline Entity(Entity const&): SceneLeaver(), ChildValidifier(), Familial(), SafePointerTarget() { M_ASSERT(false); }
	
	static void							initialiseClass() {}
	static void							finaliseClass() {}
	
	void								prepareMove(InsertionPoint const& _newPosition);
	void								commitMove(InsertionPoint const& _oldPosition);
	void								silentMove(InsertionPoint const& _to) { InsertionPoint from = over(); prepareMove(_to); commitMove(from); }
	void								silentRemove() { silentMove(Nowhere); }
	void								move(InsertionPoint const& _newPosition);

	inline int							index() const { return m_index; }
	inline Entity*						parent() const { return m_parent; }
	template<class T> inline bool		parentIs() const { return m_parent ? m_parent->isKind<T>() : false; }
	template<class T> inline T*			parentAs() const { M_ASSERT(m_parent); return m_parent->asKind<T>(); }
	template<class T> inline T*			tryParentAs() const { M_ASSERT(m_parent); return m_parent ? m_parent->tryKind<T>() : 0; }
	
	inline int							childCount() const { return m_namedChildren.size() + m_cardinalChildren.size(); }
	inline QList<Entity*>				children() const { return m_namedChildren.values() + m_cardinalChildren; }
	inline Entity*						child(int _i) const { if (_i >= 0 && _i < m_cardinalChildren.size()) { M_ASSERT(m_cardinalChildren[_i]->m_parent == this); return m_cardinalChildren[_i]; } else return _i < 0 ? m_namedChildren.value(_i) : 0; }
	inline int							childCountAt(int _i) const { if (_i >= 0) return _i < m_cardinalChildren.size() ? 1 : 0; int r = 0; QHash<int, Entity*>::ConstIterator i = m_namedChildren.find(_i); while (i != m_namedChildren.end() && i.key() == _i) ++r, ++i; return r; }
	inline QList<Entity*>				childrenAt(int _i) const { if(_i < 0) return m_namedChildren.values(_i); if (_i < m_cardinalChildren.size()) return QList<Entity*>() << child(_i); return QList<Entity*>(); }
	template<class T> inline QList<T*>	childrenOf() const { return filterEntities<T>(children()); }
	template<class T> inline int		childIndexOf() const { foreach (Entity* e, children()) if (e->isKind<T>()) return e->m_index; return UndefinedIndex; }
	template<class T> inline int		childCountOf() const { int r = 0; foreach (Entity* i, children()) if (i->isKind<T>()) r++; return r; }
	template<class T> inline T*			childOf() const { M_ASSERT(childCountOf<T>()); return childAs<T>(childIndexOf<T>()); }
	template<class T> inline bool		childIs(int _i) const { if (Entity* r = child(_i)) return r->isKind<T>(); return false; }
	template<class T> inline T*			childAs(int _i) const { Entity* e = child(_i); M_ASSERT(e); return e->asKind<T>(); }
	template<class T> inline T*			tryChild(int _i) const { if (Entity* e = child(_i)) return e->tryKind<T>(); return 0; }
	
	inline QList<Entity*> const&		cardinalChildren() const { return m_cardinalChildren; }
	inline int							cardinalChildCount() const { return m_cardinalChildren.size(); }
	template<class T> inline QList<T*>	cardinalChildrenOf() const { return filterEntities<T>(m_cardinalChildren); }
	template<class T> inline T*			cardinalChildOf() const { M_ASSERT(cardinalChildCountOf<T>()); return childAs<T>(cardinalChildIndexOf<T>()); }
	template<class T> inline int		cardinalChildIndexOf() const { for (int r = 0; r < m_cardinalChildren.size(); ++r) if (childIs<T>(r)) return r; return UndefinedIndex; }
	template<class T> inline int		cardinalChildCountOf() const { int r = 0; foreach (Entity* i, m_cardinalChildren) if (i->isKind<T>()) r++; return r; }

	inline Entity*						sibling(int _i) const { return m_parent ? m_parent->child(_i) : 0; }
	template<class T> inline bool		siblingIs(int _i) const { return m_parent ? m_parent->childIs<T>(_i) : false; }
	template<class T> inline T*			siblingAs(int _i) const { M_ASSERT(m_parent); return m_parent->childAs<T>(_i); }
	template<class T> inline T*			trySibling(int _i) const { return m_parent ? m_parent->tryChild<T>(_i) : 0; }
	inline QList<Entity*>				siblings() const { if (!m_parent) return QList<Entity*>(); QList<Entity*> ret; foreach (Entity* e, m_parent->children()) if (e != this) ret += e; return ret; }
	template<class T> inline QList<T*>	siblingsOf() const { return filterEntities<T>(siblings()); }
	inline int							siblingCount() const { return m_parent ? m_parent->childCount() - 1 : 0; }
	inline QList<Entity*>				cardinalSiblings() const { if (!m_parent) return QList<Entity*>(); QList<Entity*> ret; foreach (Entity* e, m_parent->m_cardinalChildren) if (e != this) ret += e; return ret; }
	template<class T> inline QList<T*>	cardinalSiblingsOf() const { return filterEntities<T>(cardinalSiblings());  }
	inline int							cardinalSiblingCount() const { return m_parent ? m_parent->cardinalChildCount() - (m_index < 0 ? 0 : 1) : 0; }
	inline QList<Entity*>				parentsChildren() const { return m_parent ? m_parent->children() : QList<Entity*>(); }
	template<class T> inline QList<T*>	parentsChildrenOf() const { return m_parent ? m_parent->childrenOf<T>() : QList<T*>(); }
	inline int							parentsChildCount() const { return m_parent ? m_parent->childCount() : 0; }
	inline QList<Entity*>				parentsCardinalChildren() const { return m_parent ? m_parent->cardinalChildren() : QList<Entity*>(); }
	template<class T> inline QList<T*>	parentsCardinalChildrenOf() const { return m_parent ? m_parent->cardinalChildrenOf<T>() : QList<T*>(); }
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
	
	template<class T> QList<T*>			selfAndAncestorsChildrenOf() const { QList<T*> ret = childrenOf<T>(); return parent() ? ret + parent()->selfAndAncestorsChildrenOf<T>() : ret; }
	
	inline int							nonPlaceholderCount() const { int ret = 0; foreach (Entity* i, m_cardinalChildren) if (!i->isPlaceholder()) ret++; return ret; }
	inline QList<Entity*>				nonPlaceholders() const { QList<Entity*> ret; foreach (Entity* i, m_cardinalChildren) if (!i->isPlaceholder()) ret += i; return ret; }
	inline Entity*						nonPlaceholder(int _i) const { int c = 0; foreach (Entity* i, m_cardinalChildren) if (c++ == _i) return i; M_ASSERT(false); return 0; }

	virtual bool						usurpsChild(Entity const*) const { return false; }
	bool								isUsurped() const { return m_parent->usurpsChild(this); }
	
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
	inline static Entity*				spawn(QString const& _kind) { AuxilliaryFace const* f = AuxilliaryRegistrar::get()->auxilliary(_kind); M_ASSERT(f); return f->create(); }

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
	/// @returns true if object is current in heirarchy leading to a BasicRoot.
	inline bool							isInModel() const { return m_rootEntity; }
	/// @returns true if everything is absolutely, 100%, fine.
	bool								isValid() const;

	// Insertion points
	inline InsertionPoint				over() const { return InsertionPoint(parent(), index()); }
	inline InsertionPoint				front() { return InsertionPoint(this, 0); }
	inline InsertionPoint				back() { return InsertionPoint(this, UndefinedIndex); }
	inline InsertionPoint				middle(int _at) { return InsertionPoint(this, _at); }
	/**
	 * Order of preference:
	 * - A named position that requires us to be there.
	 * - A placeholder of a cardinal position.
	 * - The back of the cardinals that requires us to be there.
	 * - A named position that allows us being there.
	 * - The back of the cardinals if it allows us being there.
	 * - Nowhere
	 */
	InsertionPoint						firstFor(Kind const& _k);
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
	static bool							keyPressedOnInsertionPoint(InsertionPoint const&, EntityKeyEvent const*) { return false; }
	
	QString								indexName(int _i) const;
	QString								indexName() const { return m_parent ? m_parent->indexName(m_index) : "[Orphan]"; }
	QList<int>							knownNames() const;
	
	virtual QString						defineLayout(ViewKeys&) const;
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
	static bool							isValidName(QString const& _n);
	template<class T>
	inline static bool					simplePlaceholderKeyPressHandler(InsertionPoint const& _p, EntityKeyEvent const* _e, QString const& _t);
	template<class T>
	inline static bool					simpleInsertionPointKeyPressHandler(InsertionPoint const& _p, EntityKeyEvent const* _e, QString const& _t, bool _ontoNew = true);	

	void								debugTree() const;
	void								debugTree(QString const& _i) const;
	
	/// Builds dependencies of this object. Assumes there are none there yet, since this should only ever be called
	/// directly following a load.
	virtual void						apresLoad() { updateAncestralDependencies(); }
	
	// CHANGE This lot moves to ChangeMan (probably in a different form).
	QList<Entity*>						dependents() const;
	QList<Entity*>						dependencies() const;
	
	virtual void						childrenInitialised();
	virtual void						childAdded(int _newChildsIndex);
	virtual void						childSwitched(Entity* _currentChild, Entity* _exChild);
	virtual void						childRemoved(Entity* _exChild, int _deadChildsIndex);
	virtual void						childMoved(Entity* _child, int _oldIndex);
	virtual void						parentAdded();
	/// We have a different parent entity. Will usually end up with dependencySwitched(). dependencyAdded/Removed() are called if _old/m_parent is null.
	virtual void						parentSwitched(Entity* _exParent);
	virtual void						parentRemoved(Entity* _exParent);
	
	// CHANGE This lot's logic moves to ChangeMan
	void								dependencyAdded(Entity* _e) { change(this, DependencyAdded, _e); onDependencyAdded(_e); }
	void								dependencyRemoved(Entity* _e, int _index = UndefinedIndex) { change(this, DependencyRemoved, _e); onDependencyRemoved(_e, _index); }
	void								dependencyChanged(Entity* _e) { change(this, DependencyChanged, _e); onDependencyChanged(_e); }
	void								dependencySwitched(Entity* _e, Entity* _o) { change(this, DependencySwitched, _e); onDependencySwitched(_e, _o); }
	void								notifyOfChildMove(Entity* _e, int _oI) { change(this, ChildMoved, _e); onChildMoved(_e, _oI); }
	void								indexChanged(int _oI) { change(this, ContextIndexChanged, 0); onIndexChanged(_oI); }
	
	// CHANGE: Moves to Changer.
	/// To be called when something about the object has changed. Notifies dependents.
	/// If _aspect & Visually then it calls a relayoutLater().
	enum { Logically = 0x0001, Visually = 0x0002, LastAspect = Visually, AllAspects = 0xffff };
	void								changed(int _aspect = AllAspects);
	
	inline BasicRoot*					rootEntity() const { return m_rootEntity; }
	
protected:
	virtual ~Entity();

	// CHANGE: This lot moves to Depender, and perhaps the logic into ChangeMan.
	/// Adds a dependency.
	/// Note this will *not* call onDependencyAdded(_e) for you. You must call it yourself if you want it to run.
	void								addDependency(Entity* _e);
	/// Removes a dependency.
	/// Note this will *not* call onDependencyRemoved(_e) for you. You must call it yourself if you want it to run.
	void								removeDependency(Entity* _e);
	/// Removes all dependencies.
	void								removeAllDependencies();
	bool								haveDependency(Entity* _e) const { return m_dependencies.contains(_e); }
	virtual bool						botherNotifying() const { return isComplete(); }

	// CHANGE: This moves to ChangeMan (in another form)
	/// Rejigs our ancestral dependencies. This should be (TODO: and isn't yet) called whenever any of our ancestors have changed parent
	/// or been switched, or when the ouput of ancestralDependencies() changes.
	void								updateAncestralDependencies();







	/// Called when:
	/// - Our index changes and familyDependencies includes DependsOnIndex, but the parent remains the same.
	/// @note By default, it does nothing.
	virtual void						onIndexChanged(int /*_oldIndex*/) {}
	/// Called when:
	/// - A registered or family dependency's state changes (_e is the dependency) and its onChanged() returned true.
	/// - A child changes position, and children are a family dependency (_e is the child).
	/// @note By default, it does nothing. 
	virtual void						onDependencyChanged(Entity* /*_e*/) {}
	/// Called when:
	/// - A registered dependency is removed and another is immediately added (_e is the new dependency).
	/// - The object fulfilling an ancestral dependency has changed, though both are non-null (_e is the old ancestor).
	/// - A family member is replaced, and there is a family dependency (_e is the new family member).
	/// @note By default, it just called the onDependencyChanged() method. 
	virtual void						onDependencySwitched(Entity* _e, Entity* /*_old*/) { onDependencyChanged(_e); }
	/// Called when familyDependencies includes DependsOnChildOrder and:
	/// - A child entity has had its index changed (usually through the insertion of another child at an earlier
	/// position).
	/// @note By default, it just called the onDependencyChanged() method. 
	virtual void						onChildMoved(Entity* _e, int /*_oldIndex*/) { onDependencyChanged(_e); }
	/// Called when:
	/// - A new parent is set where before it was null and parent is a family dependency (_e is the new parent).
	/// - A new dependent ancestor is set where before it was null (_e is the new ancestor).
	/// - A new child is added and children are a family dependency (_e is the child).
	/// @note By default, it just called the onDependencyChanged() method. 
	virtual void						onDependencyAdded(Entity* _e) { onDependencyChanged(_e); }
	/// Called when:
	/// - The null parent is set where there was one before and parent is a family dependency (_e is the old parent).
	/// - A child is removed and children are a family dependency (_e is the old child).
	/// - What was a dependent ancestor is removed (_e is the old ancestor).
	/// - A registered dependency has removed itself (_e is the old dependency).
	/// @note By default, it does nothing. 
	virtual void						onDependencyRemoved(Entity*, int /*_index*/) {}
	/// Called when we depend on children and:
	/// - This entity has been created with prepareChildren called on it.
	/// - This entity has usurped all of another entity's children.
	/// - This entity has had its children "validified", whereby invalid ones are removed and new ones added
	///   as necessary. This is only called if > 1 child was added during the operation.
	/// This is called instead of a number of onDependencyAdded()s, and removed confusion about the state of the object
	/// as each dependency is added.
	/// @note By default, it calls onDependencyAdded() for every child entity (whether recently added or not). 
	/// @note If you intend to use this, you may find it useful to change notificationRequirements() so it doesn't
	/// include BeInModel.
	virtual void						onChildrenInitialised();

	enum { DependsOnNothing = 0, DependsOnParent = 1, DependsOnChildren = 2, DependsOnBoth = 3, DependsOnIndex = 4, TestOnOrder = 8, DependsOnChildOrder = DependsOnChildren | TestOnOrder };
	virtual int							familyDependencies() const { return DependsOnNothing; }
	
	virtual Kinds						ancestralDependencies() const { return Kinds(); }







	/// Convenience methods.
	/// Notes parentSwitched to child, and child removal to old parent.
	/// @important This does *not* notify the new parent of the child addition; you must do that yourself! 
	/// This assumes _old is still valid!
	inline void							parentSwitchedWithChildRemoved(InsertionPoint const& _old)
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
	
protected:
	BasicRoot*							m_rootEntity;

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
	
	/// Just makes sure that the rootEntity is the parent's root entity. Should only be called from the parent.
	void								checkRoot();

	/// Helper function for validifyChildren()
	bool								validifyChild(int _i, int* _added);

	/// Remove all backlinks (i.e. all those dependencies that target us).
	/// Calls onDependencyRemoved() on each of the registered dependents.
	void								clearDependents();

	Entity*								m_parent;
	int									m_index;
	QList<Entity*>						m_cardinalChildren;
	QHash<int, Entity*>					m_namedChildren;
	QSet<Entity*>						m_dependencies;
	QList<Entity*>						m_ancestralDependencies;
	int									m_notifiedOfChange;
	
	/// The cache
	QList<Entity*>						m_dependents;
};

class EntityStylist
{
public:
	virtual ~EntityStylist() {}
	
	virtual QString defineLayout(Entity* _e, ViewKeys& _k)
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
		M_ASSERT(!_dependency || _dependency->asKind<Entity>()->isInModel());
		M_ASSERT(!old || old->asKind<Entity>()->isInModel());
		if (old)
			removeDependency(old);
		_dependencyVariable = _dependency;
		if (_dependency)
		{
			addDependency(_dependency->asKind<Entity>());
			dependencySwitched(_dependency->asKind<Entity>(), old);
		}
		changed();
	}
}

template<class T>
bool Martta::Entity::simplePlaceholderKeyPressHandler(InsertionPoint const& _p, EntityKeyEvent const* _e, QString const& _t)
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
bool Martta::Entity::simpleInsertionPointKeyPressHandler(InsertionPoint const& _p, EntityKeyEvent const* _e, QString const& _t, bool _ontoNew)
{
	if (_e->text() == _t && _p.allowedToBeKind<T>())
	{
		T* n = new T;
		if (_p.exists() && !_p->isPlaceholder() && n->firstFor(_p->kind()) != Nowhere)
		{
			// insert
			// when pressed on _p refers to a, changes from x->(a->(d, e), b, c) to x->(N->(a->(d, e)), b, c)
			Entity* e = _p.entity();
			e->insert(n);
			_e->noteStrobeCreation(e, n);
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
