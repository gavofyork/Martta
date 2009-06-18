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

#define POOL_ALLOCATOR 0

#if POOL_ALLOCATOR
#include <boost/pool/pool.hpp>
#endif

#include <QDebug>
#include <QList>
#include <QHash>
#include <QPointF>

#include "Meta.h"
#include "Kind.h"
#include "AuxilliaryFace.h"
#include "AuxilliaryFace.h"
#include "Location.h"
#include "ModelPtrFace.h"
#include "InsertionPoint.h"
#include "EntityKeyEvent.h"
#include "SafePointer.h"
#include "SceneLeaver.h"

class QTextStream;
class QDomElement;

namespace Martta
{

class EntityStylist;
class CodeScene;
class DecorationContext;
class EditDelegateFace;
class EntityKeyPress;
class DeclarationEntity;
class TypeDefinition;
class EntityKeyEvent;

extern int s_news;
extern int s_deletes;

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

template<class T> class ModelPtr;


/**
 * Note regarding rootEntity/null-Context: You can never depend on something which does not share the
 * same RootEntity object as you. Objects yet to be inserted into the program have a nullContext and thus
 * no RootEntity. If you make objects within the scene children of such objects they will be moved
 * out of scene, and thus can have no interactions (i.e. dependencies/references) with objects within the
 * scene. This applies even if the situation is temporary, since the check/changes happen at move
 * time.
 */
class Entity: public SafePointerTarget, public_interface SceneLeaver
{
	friend class EntityStylist;
	friend class EditDelegateFace;

public:
	template<class T> void setDependency(ModelPtr<T>& _dependencyVariable, T* _dependency)
	{
		if (_dependencyVariable != _dependency)
		{
			Entity* old = _dependencyVariable->asKind<Entity>();
			if (_dependency)
				M_ASSERT(_dependency->asKind<Entity>()->isInModel());
			if (old)
				M_ASSERT(old->asKind<Entity>()->isInModel());
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
	
	typedef Nothing Super;
	static const bool IsInterface = false;
	static const bool IsPlaceholder = true;
	static const bool IsObject = false;
	
#if POOL_ALLOCATOR
	static QMap<size_t, boost::pool<>* >* s_pools;
	inline void* operator new(size_t _size)
	{
		if (!s_pools)
			s_pools = new QMap<size_t, boost::pool<>* >;
		if (!s_pools->contains(_size))
			s_pools->insert(_size, new boost::pool<>(_size));
		s_news++;
		void* ret = s_pools->value(_size)->malloc();
		return ret;
	}
	inline void operator delete(void* p)
	{
		s_deletes++;
		foreach (boost::pool<>* i, s_pools->values())
			if (i->is_from(p))
			{
				i->free(p);
				return;
			}
		M_ASSERT(0);
	}
#else
	inline void* operator new(size_t _size)
	{
		s_news++;
		void* ret = malloc(_size);
		return ret;
	}
	inline void operator delete(void* p)
	{
		s_deletes++;
		free(p);
	}
#endif
	
	enum { EndOfNamed = INT_MIN, Cardinals = 0 };
	
	MARTTA_INHERITS(SceneLeaver, 0)
	
	inline Entity(): m_rootEntity(0), m_context(0), m_contextIndex(UndefinedIndex), m_notifiedOfChange(false) {}
	
	static void							initialiseClass() {}
	static void							finaliseClass() {}
	
	void								move(InsertionPoint const& _to) { InsertionPoint from = over(); moveVirtually(_to); commitVirtualMove(from); }
	void								remove() { move(Nowhere); }
	
	// This acts according to 
	void								moveVirtually(InsertionPoint const& _newPosition);
	void								commitVirtualMove(InsertionPoint const& _oldPosition);

	inline Entity*						context() const { return m_context; }
	template<class T> inline bool		contextIs() const { return m_context ? m_context->isKind<T>() : false; }
	template<class T> inline T*			contextAs() const { M_ASSERT(m_context); return m_context->asKind<T>(); }
	inline int							contextIndex() const { /*M_ASSERT(m_contextIndex == parentsChildren().indexOf(const_cast<Entity*>(this)));*/ return m_contextIndex; }
	template<class T> inline int		contextIndexExclusive() const { M_ASSERT(isKind<T>()); return parentsChildrenOf<T>().indexOf(static_cast<T*>(const_cast<Entity*>(this))); }
	virtual QList<DeclarationEntity*>	spacesInScope() const;
	template<class T> QList<T*>			entitiesHereAndBeforeOf() const { QList<T*> ret = childrenOf<T>(); return context() ? ret + context()->entitiesHereAndBeforeOf<T>() : ret; }
	virtual Identifiable*				findEntity(QString const& _key) const;
	template<class T> inline ModelPtr<T>locateEntity(QString const& _key) const { return ModelPtr<T>(_key, rootEntity()); }
	template<class T> QList<T*>			findAll() const { QList<T*> ret = childrenOf<T>(); foreach (Entity* i, m_cardinalChildren) ret << i->findAll<T>(); return ret; }
	inline QList<Entity*> const&		cardinalChildren() const { return m_cardinalChildren; }
	inline QList<Entity*>				children(int _i) const { if(_i < 0) return m_namedChildren.values(_i); if (_i < m_cardinalChildren.size()) return QList<Entity*>() << child(_i); return QList<Entity*>(); }
	inline int							cardinalChildCount() const { return m_cardinalChildren.size(); }
	inline int							childCountAt(int _i) const { if (_i >= 0) return _i < m_cardinalChildren.size() ? 1 : 0; int r = 0; QHash<int, Entity*>::ConstIterator i = m_namedChildren.find(_i); while (i != m_namedChildren.end() && i.key() == _i) ++r, ++i; return r; }
	inline QList<Entity*>				children() const { return m_namedChildren.values() + m_cardinalChildren; }
	inline int							childCount() const { return m_namedChildren.size() + m_cardinalChildren.size(); }
	inline Entity*						child(int _i) const { if (_i >= 0 && _i < m_cardinalChildren.size()) { M_ASSERT(m_cardinalChildren[_i]->m_context == this); return m_cardinalChildren[_i]; } else return _i < 0 ? m_namedChildren.value(_i) : 0; }
	template<class T> inline QList<T*>	cardinalChildrenOf() const { return filterEntities<T>(m_cardinalChildren); }
	template<class T> inline int		cardinalChildIndexOf() const { for (int r = 0; r < m_cardinalChildren.size(); ++r) if (childIs<T>(r)) return r; return UndefinedIndex; }
	template<class T> inline int		cardinalChildCountOf() const { int r = 0; foreach (Entity* i, m_cardinalChildren) if (i->isKind<T>()) r++; return r; }
	template<class T> inline QList<T*>	childrenOf() const { return filterEntities<T>(children()); }
	template<class T> inline int		childIndexOf() const { foreach (Entity* e, children()) if (e->isKind<T>()) return e->m_contextIndex; return UndefinedIndex; }
	template<class T> inline int		childCountOf() const { int r = 0; foreach (Entity* i, children()) if (i->isKind<T>()) r++; return r; }
	template<class T> inline bool		childIs(int _i) const { if (Entity* r = child(_i)) return r->isKind<T>(); return false; }
	template<class T> inline T*			childAs(int _i) const { Entity* e = child(_i); M_ASSERT(e); return e->asKind<T>(); }
	template<class T> inline T*			tryChildAs(int _i) const { if (Entity* e = child(_i)) return e->tryKind<T>(); return 0; }
	inline QList<Entity*>				parentsChildren() const { if (!m_context) return QList<Entity*>(); return m_context->m_cardinalChildren; }
	inline int							parentsChildrenCount() const { M_ASSERT(context()); return context()->m_cardinalChildren.size(); }
	inline Entity*						parentsChild(int _i) const { M_ASSERT(context()); return context()->child(_i); }
	template<class T> inline QList<T*>	parentsChildrenOf() const { if (!m_context) return QList<T*>(); return m_context->childrenOf<T>(); }
	template<class T> inline bool		parentsChildIs(int _i) const { return parentsChild(_i) ? parentsChild(_i)->isKind<T>() : false; }
	template<class T> inline T*			parentsChildAs(int _i) const { M_ASSERT(parentsChild(_i)); return parentsChild(_i)->asKind<T>(); }
	inline QList<Entity*>				siblings() const { if (!m_context) return QList<Entity*>(); QList<Entity*> ret; foreach (Entity* e, m_context->m_cardinalChildren) if (e != this) ret += e; return ret; }
	inline int							siblingCount() const { M_ASSERT(context()); return context()->m_cardinalChildren.size() - 1; }
	template<class T> inline QList<T*>	siblingsOf() const { return filterEntities<T>(siblings());  }
	
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
	
	virtual bool						usurpsChild(Entity const*) const { return false; }
	bool								isUsurped() const { return m_context->usurpsChild(this); }
	
	inline int							nonPlaceholderCount() const { int ret = 0; foreach (Entity* i, m_cardinalChildren) if (!i->isPlaceholder()) ret++; return ret; }
	inline QList<Entity*>				nonPlaceholders() const { QList<Entity*> ret; foreach (Entity* i, m_cardinalChildren) if (!i->isPlaceholder()) ret += i; return ret; }
	inline Entity*						nonPlaceholder(int _i) const { int c = 0; foreach (Entity* i, m_cardinalChildren) if (c++ == _i) return i; M_ASSERT(false); return 0; }

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
	 * Destructs the object. Use this before delete. It sets the context to zero while the object is still
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
	
	/**
	 * Moves this to become the first child of _e, and puts _e into our place.
	 * Never removes placeholders (i.e. everything is inserted).
	 * Our children are preserved. _e's children are shunted along to allow us to be the first.
	 */
	Entity*								insert(Entity* _e);
	template<class T> inline T*			insert() { T* r = new T; insert(r); return r; }
	
	/**
	 * Safe, model-aware deletion operation. It keeps everything in order according to allowedKinds().
	 */
	void								deleteAndRefill(Entity* _e = 0, bool _moveToGrave = false);

	inline RootEntity*					rootEntity() const { return m_rootEntity; }
	
	/// Stuff to do with RTTI. [ Same as MARTTA_BASIC macro ]
	static AuxilliaryFace const*		staticAuxilliary();
	static Kind							staticKind;
	template<int i, int d>						
	struct AltSuper {
		typedef Nothing TheType;
	};
	
	template<int m = -1, typename T = void>
	struct AltSuperCount {
		static const int count = AltSuperCount<m + 1, typename AltSuper<m + 1, 0>::TheType>::count;
	};
	
	template<int m>						
	struct AltSuperCount<m, Nothing> {
		static const int count = m;
	};
	
	template<int m, int d = 0>			
	struct ASHelper {
		public: static AuxilliaryFace const** altSupers() {
			static AuxilliaryFace const* r[m];
			r[m - 1] = AltSuper<m - 1, 0>::TheType::staticAuxilliary();
			memcpy(r, ASHelper<m - 1>::altSupers(), (m - 1) * sizeof(AuxilliaryFace const*));
			return r;
		}
	};
	
	template<int d>						
	struct ASHelper<0, d> {
		public: static AuxilliaryFace const** altSupers() {
			static AuxilliaryFace const* r[0]; return r;
		}
	};
	
	inline virtual Kind					kind() const { return this ? staticKind : Kind(Nothing::staticAuxilliary()); }
	
	template<class T> inline bool		isKind() const { return this && kind().isKind(T::staticKind); }
	inline bool							isKind(Kind _k) const { return this && kind().isKind(_k); }
	inline bool							isKind(Kinds _k) const { return this && kind().isKind(_k); }
	
	template<class T> inline T*			asInterface() { return const_cast<T*>(const_cast<Entity const*>(this)->asInterface<T>()); }
	template<class T> inline T const*	asInterface() const { M_ASSERT(isKind<T>()); return reinterpret_cast<T const*>(toInterface(T::staticKind)); }
	template<class T> inline T*			asKind() { M_ASSERT(this); M_ASSERT(isKind<T>()); return T::IsInterface ? asInterface<T>() : tryCast<T*>(this); }
	template<class T> inline T const*	asKind() const { M_ASSERT(this); M_ASSERT(isKind<T>()); return T::IsInterface ? asInterface<T>() : tryCast<T const*>(this); }
	template<class T> inline T*			tryKind() { if (this && isKind<T>()) return asKind<T>(); return 0; }
	template<class T> inline T const*	tryKind() const { if (this && isKind<T>()) return asKind<T>(); return 0; }
	virtual Entity const*				self() const { return this; }
	virtual Entity*						self() { return this; }

	// The following are information for checking and selection mechanisms.
	// These values could change depending upon elements already in place (e.g. dereference operators), so should be rechecked whenever anything changes.

	/// @returns the minimum number of entities than can be allowed for the object still to be valid.
	/// A greater amount than this may be allowed, depending on the output of allowedKinds, but never less than this.
	/// Default returns zero (i.e. no minimum).
	/// @note Do not call this directly; use isAllowed() on the child entity instead, since that is a more complete test.
	virtual int							minimumRequired() const { return 0; }
	/// @returns the minimum number of entities than can be allowed in a sub-zero entity slot.
	/// A greater amount than this may be allowed, but never less than this.
	/// Default returns zero (i.e. no entity strictly needed).
	/// @note Do not call this directly; use isAllowed() on the child entity instead, since that is a more complete test.
	virtual int							minimumRequiredNamed(int) const { return 0; }
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
	/// @returns true if the child at i has an allowable state according to this entity.
	/// @note Derivations should honour the decision of their Super should it be true.
	/// @note Do not call this directly; use isValid() on the child entity instead, since that is a more complete test.
	virtual bool						isChildInValidState(int) const { return true; }
	/// @returns true if we have an allowable state (according to us).
	/// @note Derivations should honour the decision of their Super should it be true.
	/// @note Do not call this directly; use isValid() on the child entity instead, since that is a more complete test.
	virtual bool						isInValidState() const { return true; }
	/// @returns true if parentheses are need around the expression.
	// TODO: figure out if parentheses are needed
	// TODO: Consider moving to somewhere more specialised.
	virtual bool						doINeedParenthesising(Entity const* _child) const { (void)_child; return false; }
	/// @returns true if this object isn't actually a real language entity.
	/// Derivations automatically handled in derivations be MARTTA_OBJECT(_INTERFACE) macros. Do not reimplement.
	virtual bool						isPlaceholder() const { return true; }
	/// Checked after change; if true is returned, this may be deleted.
	virtual bool						isSuperfluous() const;
	
	// Tests; these use the above information in order to determine the answer.
	
	/// @returns true if kind _o is allowed at entity position _i.
	/// Takes into account context's allowedKinds(int) and this object's allowedAncestor.
	bool								isAllowed(int _i, Kind _o) const;
	/// Convenience version of above.
	/// Takes into account context's allowedKinds(int) and this object's allowedAncestor.
	template<class T> inline bool		isAllowed(int _i) const { return isAllowed(_i, Kind::of<T>()); }
	/// @returns true if this entity would be valid if it were an object of a particular type.
	/// Takes into account context's allowedKinds(int) and this object's allowedAncestor.
	inline bool							isAllowed(Kind _o) const { return context() ? context()->isAllowed(contextIndex(), _o) : true; }
	/// Convenience version of above.
	/// Takes into account context's allowedKinds(int) and this object's allowedAncestor.
	template<class T> inline bool		isAllowed() const { return isAllowed(Kind::of<T>()); }
	/// @returns true if this entity, in its current position, is valid.
	/// Takes into account context's allowedKinds(int) and this object's allowedAncestor.
	inline bool							isAllowed() const { return isAllowed(kind()); }
	/// @returns true if this entity cannot be killed without costing the validity of its context's children set.
	bool								isNecessary() const;
	/// @returns true if all entity positions are filled validly, including all those required. 
	/// @warning This uses allowedKinds() and minimumRequired, and so is not safe to be called from your reimplementation of
	/// these or anything that they (indirectly) use.
	bool								isComplete() const;
	/// @returns true if object is current in heirarchy leading to a RootEntity.
	inline bool							isInModel() const { return m_rootEntity; }
	/// @returns true if everything is absolutely, 100%, fine.
	bool								isValid() const;

	// Insertion points
	inline InsertionPoint				over() const { return InsertionPoint(context(), contextIndex()); }
	inline InsertionPoint				front() { return InsertionPoint(this, 0); }
	inline InsertionPoint				back() { return InsertionPoint(this, UndefinedIndex); }
	inline InsertionPoint				middle(int _at) { return InsertionPoint(this, _at); }
	bool								attemptAppend(EntityKeyEvent const* _e);
	bool								attemptInsert(EntityKeyEvent const* _e);

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
	
	QList<Entity*>						dependents() const;
	QList<Entity*>						dependencies() const;
	
	void								childrenInitialised();
	void								childAdded(Entity* _ch) { if (_ch->context() == this) childAdded(_ch->contextIndex()); }
	void								childAdded(int _index);
	void								childSwitched(int _index, Entity* _o) { if (_index >= 0 && _index < m_cardinalChildren.size()) childSwitched(child(_index), _o); }
	void								childSwitched(Entity* _ch, Entity* _o);
	void								childRemoved(Entity* _ch, int _index);
	void								contextAdded() { contextAdded(context()); }
	void								contextAdded(Entity* _con);
	/// Our context has changed object. Will usually end up with dependencySwitched(). dependencyAdded/Removed() are called if _old/m_context is null.
	void								contextSwitched(Entity* _old);
	void								contextRemoved(Entity* _old);
	
	/// Notes contextSwitched to child, and child removal to old context.
	/// @important This does *not* notify the new context of the child addition; you must do that yourself! 
	/// This assumes _old is still valid!
	void								contextSwitchedWithChildRemoved(Entity* _old, int _ourOldContextIndex) { contextSwitched(_old); if (_old) _old->childRemoved(this, _ourOldContextIndex); }
	void								contextSwitchedWithChildRemoved(InsertionPoint const& _old) { contextSwitchedWithChildRemoved(_old.context(), _old.index()); }
	
	void								dependencyAdded(Entity* _e) { change(this, DependencyAdded, _e); onDependencyAdded(_e); }
	void								dependencyRemoved(Entity* _e, int _index = UndefinedIndex) { change(this, DependencyRemoved, _e); onDependencyRemoved(_e, _index); }
	void								dependencyChanged(Entity* _e) { change(this, DependencyChanged, _e); onDependencyChanged(_e); }
	void								dependencySwitched(Entity* _e, Entity* _o) { change(this, DependencySwitched, _e); onDependencySwitched(_e, _o); }
	void								childMoved(Entity* _e, int _oI) { change(this, ChildMoved, _e); onChildMoved(_e, _oI); }
	void								contextIndexChanged(int _oI) { change(this, ContextIndexChanged, 0); onContextIndexChanged(_oI); }
	
	/// To be called when something in the object has changed. Calls onChanged() and notifies dependents.
	void								changed();
	/// This notifies only a single dependent of a change in us.
	/// This is appropriate when this object's state hasn't changed, but some joint aspect of the two objects has
	/// (e.g. the @a _dependent's contextIndex).
	/// If they are not dependent on us, we are not in the model or in an invalid position then nothing will happen.
	void								notifyOfChange(Entity* _dependent);
	
	/// Remove all backlinks (i.e. all those dependencies that target us).
	/// Calls onDependencyRemoved() on each of the registered dependents.
	void								clearDependents();
	
	virtual QString						defineLayout(ViewKeys&) const;
	
protected:
	virtual ~Entity();

	virtual void const*					toInterface(Kind) const { return 0; }
	void const*							tryInterface(Kind) const { return 0; }

	enum { DependsOnNothing = 0, DependsOnContext = 1, DependsOnChildren = 2, DependsOnBoth = 3, DependsOnContextIndex = 4, TestOnOrder = 8, DependsOnChildOrder = DependsOnChildren | TestOnOrder };
	virtual int							familyDependencies() const { return DependsOnNothing; }
	virtual Kinds						ancestralDependencies() const { return Kinds(); }
	enum { NoRequirements = 0, BeComplete = 1, BeInModel = 2 };
	virtual int							notificationRequirements() const { return BeComplete | BeInModel; }
	
	inline bool							botherNotifying() const { return (~notificationRequirements() & BeInModel || isInModel()) && (~notificationRequirements() & BeComplete || isComplete()); }
	
	/// Adds a dependency. Should only be called from inside registerDependencies().
	/// Note this will *not* call onDependencyAdded(_e) for you. You must call it yourself if you want it to run.
	void								addDependency(Entity* _e);
	/// Removes a dependency.
	/// Note this will *not* call onDependencyRemoved(_e) for you. You must call it yourself if you want it to run.
	void								removeDependency(Entity* _e);
	/// Removes all dependencies.
	void								removeAllDependencies();
	bool								haveDependency(Entity* _e) const { return m_dependencies.contains(_e); }
	
	/// Called when the state of this object has changed.
	/// @note This is only called when the object is in the model.
	/// @returns true to notify dependents that we have changed.
	virtual bool						onChanged() { relayoutLater(); return true; }
	/// Called when:
	/// - Our contextIndex changes and familyDependencies includes DependsOnContextIndex, but the context remains the same.
	/// @note By default, it does nothing.
	virtual void						onContextIndexChanged(int /*_oldIndex*/) {}
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
	/// - A child entity has had its contextIndex changed (usually through the insertion of another child at an earlier
	/// position).
	/// @note By default, it just called the onDependencyChanged() method. 
	virtual void						onChildMoved(Entity* _e, int /*_oldIndex*/) { onDependencyChanged(_e); }
	/// Called when:
	/// - A new context is set where before it was null and context is a family dependency (_e is the new context).
	/// - A new dependent ancestor is set where before it was null (_e is the new ancestor).
	/// - A new child is added and children are a family dependency (_e is the child).
	/// @note By default, it just called the onDependencyChanged() method. 
	virtual void						onDependencyAdded(Entity* _e) { onDependencyChanged(_e); }
	/// Called when:
	/// - The null context is set where there was one before and context is a family dependency (_e is the old context).
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
	virtual void						onChildrenInitialised() { foreach (Entity* e, children()) onDependencyAdded(e); }
	
	virtual Entity*						isExpander() const { return 0; }

	/// Rejigs our ancestral dependencies. This should be (TODO: and isn't yet) called whenever any of our ancestors have changed context
	/// or been switched, or when the ouput of ancestralDependencies() changes.
	void								updateAncestralDependencies();
	
protected:
	RootEntity*							m_rootEntity;

private:
	/** This will insert an entity @a _child into our brood at index @a _childsIndex.
	 * It will make sure all children have the correct contextIndex but nothing more. In particular
	 * the new child's context and rootEntity will not be updated.
	 * 
	 * This handles the case of negative and positive child indices, and the index may be
	 * UndefinedIndex, in which case the child is appended.
	 */
	void								insertIntoBrood(int _childsIndex, Entity* _child);
	/** This removes a previously inserted @a _child from our brood.
	 * It makes sure all children have the correct contextIndex but nothing more. In particular
	 * the old child's contextIndex, context and rootEntity are not updated.
	 * 
	 * This handles the case of negative and position child indices.
	 */
	void								removeFromBrood(int _childsIndex, Entity* _child);
	/** Removes all children at index @a _childsIndex from the brood of children.
	 * 
	 * It makes sure all children have the correct contextIndex but nothing more. In particular
	 * the old children's contextIndex, context and rootEntity are not updated.
	 * 
	 * This handles the case of negative and position child indices.
	 */
	void								removeAllFromBrood(int _childsIndex);
	
	/// Just makes sure that the rootEntity is the context's root entity. Should only be called from the context.
	void								checkRoot();

	virtual int							virtualEndOfNamed() const { return EndOfNamed; }

	/// Helper function for validifyChildren()
	bool								validifyChild(int _i, int* _added);

	Entity*								m_context;
	int									m_contextIndex;
	QList<Entity*>						m_cardinalChildren;
	QHash<int, Entity*>					m_namedChildren;
	QSet<Entity*>						m_dependencies;
	QList<Entity*>						m_ancestralDependencies;
	bool								m_notifiedOfChange;
	
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
		if (_p.exists() && !_p->isPlaceholder() && n->back().allowedToBeKind(_p->kind()))
		{
			// insert
			// when pressed on _p refers to a, changes from x->(a->(d, e), b, c) to x->(N->(a->(d, e)), b, c)
			Entity* e = _p.childType();
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
