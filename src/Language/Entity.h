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
#include "Auxilliary.h"
#include "Location.h"
#include "ModelPtrFace.h"
#include "InsertionPoint.h"
#include "EntityKeyEvent.h"
#include "SafePointer.h"

class QTextStream;
class QDomElement;

namespace Martta
{

class EntityStylist;
class CodeScene;
class DecorationContext;
class EditDelegateFace;
class EntityKeyPress;
class FundamentalOperator;
class DeclarationEntity;
class TypeDefinition;
class EntityKeyEvent;

extern int s_news;
extern int s_deletes;

enum ChangeOperation { EntityChanged = 0, DependencyAdded, DependencyRemoved, DependencyChanged, DependencySwitched, ContextIndexChanged };
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

class Entity;
class Nothing { public: static AuxilliaryFace const* staticAuxilliary() { return 0; } };

template<class T> struct TotalOffset { static const int value = TotalOffset<typename T::Super>::value + T::OffsetForDerivatives; };
template<> struct TotalOffset<Entity> { static const int value = 0; };

/**
 * Note regarding rootEntity/null-Context: You can never depend on something which does not share the
 * same RootEntity object as you. Objects yet to be inserted into the program have a nullContext and thus
 * no RootEntity. If you make objects within the scene children of such objects they will be moved
 * out of scene, and thus can have no interactions (i.e. dependencies/references) with objects within the
 * scene. This applies even if the situation is temporary, since the check/changes happen at setContext
 * time.
 */
class Entity: public SafePointerTarget
{
	friend class EntityStylist;
	friend class EditDelegateFace;

public:
	static const int OffsetForDerivatives = 0;
	static const int MyOffset = 0;
	
	template<class T> void setDependency(ModelPtr<T>& M_S, T* _S)
	{
		if (M_S != _S)
		{
			if (M_S)
				removeDependency(M_S->asKind<Entity>());
			M_S = _S;
			if (M_S)
			{
				addDependency(M_S->asKind<Entity>());
				dependencySwitched(M_S->asKind<Entity>());
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
	
	inline Entity(): m_rootEntity(0), m_context(0), m_contextIndex(-1), m_notifiedOfChange(false) {}
	
	static void							initialiseClass() {}
	static void							finaliseClass() {}
	
	void								setContext(Entity* _e);
	void								setContextTentatively(Entity* _e);
	void								commitTentativeSetContext(InsertionPoint const& _oldPosition);
	void								undoTentativeSetContext(InsertionPoint const& _oldPosition);
	void								moveToPosition(int _index);
	inline Entity*						context() const { return m_context; }
	template<class T> inline bool		contextIs() const { return m_context ? m_context->isKind<T>() : false; }
	template<class T> inline T*			contextAs() const { M_ASSERT(m_context); return m_context->asKind<T>(); }
	inline int							contextIndex() const { /*M_ASSERT(m_contextIndex == parentsChildren().indexOf(const_cast<Entity*>(this)));*/ return m_contextIndex; }
	template<class T> inline int		contextIndexExclusive() const { M_ASSERT(isKind<T>()); return parentsChildrenOf<T>().indexOf(static_cast<T*>(const_cast<Entity*>(this))); }
	virtual QList<DeclarationEntity*>	spacesInScope() const;
	template<class T> QList<T*>			entitiesHereAndBeforeOf() const { QList<T*> ret = entitiesOf<T>(); return context() ? ret + context()->entitiesHereAndBeforeOf<T>() : ret; }
	virtual DeclarationEntity*			findEntity(QString const& _key) const;
	template<class T> inline ModelPtr<T>locateEntity(QString const& _key) const { return ModelPtr<T>(_key, rootEntity()); }
	template<class T> QList<T*>			findAll() const { QList<T*> ret = entitiesOf<T>(); foreach (Entity* i, m_children) ret << i->findAll<T>(); return ret; }
	inline QList<Entity*> const&		entities() const { return m_children; }
	inline int							entityCount() const { return m_children.size(); }
	inline Entity*						entity(int _i) const { if (_i >= 0 && _i < m_children.size()) { M_ASSERT(m_children[_i]->m_context == this); return m_children[_i]; } else return 0; }
	template<class T> inline QList<T*>	entitiesOf() const { return filterEntities<T>(m_children); }
	template<class T> inline int		entityIndexOf() const { for (int r = 0; r < m_children.size(); ++r) if (entityIs<T>(r)) return r; return -1; }
	template<class T> inline int		entityCountOf() const { int r = 0; foreach (Entity* i, m_children) if (i->isKind<T>()) r++; return r; }
	template<class T> inline bool		entityIs(int _i) const { return (_i >= 0 && _i < m_children.size() && m_children[_i]) ? m_children[_i]->isKind<T>() : false; }
	template<class T> inline T*			entityAs(int _i) const { M_ASSERT(_i >= 0 && _i < m_children.size() && m_children[_i]); return m_children[_i]->asKind<T>(); }
	template<class L> inline QList<Entity*>			localsFor() const { return m_children.mid(TotalOffset<L>::value); }
	template<class L> inline int					localCountFor() const { return m_children.size() - TotalOffset<L>::value; }
	template<class L> inline Entity*				localFor(int _i) const { if (_i >= 0 && _i < m_children.size() - TotalOffset<L>::value) { M_ASSERT(m_children[_i + TotalOffset<L>::value]->m_context == this); return m_children[TotalOffset<L>::value + _i]; } else return 0; }
	template<class L, class T> inline QList<T*>		localsOfFor() const { return filterEntities<T>(m_children.mid(TotalOffset<L>::value)); }
	template<class L, class T> inline bool			localIsFor(int _i) const { return (_i >= 0 && _i < m_children.size() - TotalOffset<L>::value && m_children[_i + TotalOffset<L>::value]) ? m_children[_i + TotalOffset<L>::value]->isKind<T>() : false; }
	template<class L, class T> inline T*			localAsFor(int _i) const { M_ASSERT(_i >= 0); M_ASSERT(_i < m_children.size() - TotalOffset<L>::value); M_ASSERT(m_children[_i + TotalOffset<L>::value]); return m_children[_i + TotalOffset<L>::value]->asKind<T>(); }
	inline QList<Entity*>				parentsChildren() const { if (!m_context) return QList<Entity*>(); return m_context->m_children; }
	inline int							parentsChildrenCount() const { M_ASSERT(context()); return context()->m_children.size(); }
	inline Entity*						parentsChild(int _i) const { M_ASSERT(context()); return context()->entity(_i); }
	template<class T> inline QList<T*>	parentsChildrenOf() const { if (!m_context) return QList<T*>(); return m_context->entitiesOf<T>(); }
	template<class T> inline bool		parentsChildIs(int _i) const { return parentsChild(_i) ? parentsChild(_i)->isKind<T>() : false; }
	template<class T> inline T*			parentsChildAs(int _i) const { M_ASSERT(parentsChild(_i)); return parentsChild(_i)->asKind<T>(); }
	inline QList<Entity*>				siblings() const { if (!m_context) return QList<Entity*>(); QList<Entity*> ret; foreach (Entity* e, m_context->m_children) if (e != this) ret += e; return ret; }
	inline int							siblingCount() const { M_ASSERT(context()); return context()->m_children.size() - 1; }
	template<class T> inline QList<T*>	siblingsOf() const { return filterEntities<T>(siblings());  }
	template<class T> T*				ancestor() const { Entity* r = ancestor(T::staticKind); return r ? r->asKind<T>() : 0; }
	Entity*								ancestor(Kind _k) const;
	template<class T> bool				hasAncestor() const { return hasAncestor(T::staticKind); }
	bool								hasAncestor(Kind _k) const;
	template<class T> int				ancestorIndex() const { return ancestorIndex(T::staticKind); }
	int									ancestorIndex(Kind _k) const;
	template<class T> T*				selfAncestor() const { Entity* r = selfAncestor(T::staticKind); return r ? r->asKind<T>() : 0; }
	Entity*								selfAncestor(Kind _k) const;
	template<class T> bool				hasSelfAncestor() const { return hasSelfAncestor(T::staticKind); }
	bool								hasSelfAncestor(Kind _k) const;
	template<class T> int				selfAncestorIndex() const { return selfAncestorIndex(T::staticKind); }
	int									selfAncestorIndex(Kind _k) const;
	bool								hasAncestor(Entity* _a) const { return ancestorIndex(_a) != -1; }
	int									ancestorIndex(Entity* _a) const;
	
	virtual bool						usurpsChild(Entity const*) const { return false; }
	bool								isUsurped() const { return m_context->usurpsChild(this); }
	
	inline int							nonPlaceholderCount() const { int ret = 0; foreach (Entity* i, m_children) if (!i->isPlaceholder()) ret++; return ret; }
	inline QList<Entity*>				nonPlaceholders() const { QList<Entity*> ret; foreach (Entity* i, m_children) if (!i->isPlaceholder()) ret += i; return ret; }
	inline Entity*						nonPlaceholder(int _i) const { int c = 0; foreach (Entity* i, m_children) if (c++ == _i) return i; M_ASSERT(false); return 0; }

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
	/// Is called directly following preparation of children.
	/// Can be used to do initialisation that must be called when the class is (children-wise) complete.
	virtual void						onChildrenPrepared() {}
	
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
	static Entity*						spawn(QString const& _kind);

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

	// The following are information for checking and selection mechanisms.
	// These values could change depending upon elements already in place (e.g. dereference operators), so should be rechecked whenever anything changes.

	/// @returns the minimum number of entities than can be allowed for the object still to be valid.
	/// A greater amount than this may be allowed, depending on the output of allowedKinds, but never less than this.
	/// Default returns zero (i.e. no minimum).
	/// @note Do not call this directly; use isAllowed() on the child entity instead, since that is a more complete test.
	virtual int							minimumRequired() const { return 0; }
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
	inline InsertionPoint				back() { return InsertionPoint(this, -1); }
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
	virtual bool						activated(CodeScene* _s);
	virtual bool						keyPressed(EntityKeyEvent const*);
	static bool							keyPressedOnInsertionPoint(InsertionPoint const&, EntityKeyEvent const*) { return false; }
	
	/// We become current in all code scenes.
	void								setCurrent();
	void								setEditing(CodeScene* _s);
	bool								isCurrent() const;
	bool								isCurrent(CodeScene* _s) const;
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
	
	static void							keyPressEventStarter(EntityKeyEvent* _e);
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
	
	void								childAdded(Entity* _ch) { if (_ch->context() == this) childAdded(_ch->contextIndex()); }
	void								childAdded(int _index);
	void								childSwitched(int _index) { if (_index >= 0 && _index < m_children.size()) childSwitched(entity(_index)); }
	void								childSwitched(Entity* _ch);
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
	
	void								dependencyAdded(Entity* _e) { if (isInModel()) { change(this, DependencyAdded, _e); onDependencyAdded(_e); } }
	void								dependencyRemoved(Entity* _e, int _index = -1) { if (isInModel()) { change(this, DependencyRemoved, _e); onDependencyRemoved(_e, _index); } }
	void								dependencyChanged(Entity* _e) { if (isInModel()) { change(this, DependencyChanged, _e); onDependencyChanged(_e); } }
	void								dependencySwitched(Entity* _e) { if (isInModel()) { change(this, DependencySwitched, _e); onDependencySwitched(_e); } }
	void								contextIndexChanged(int _oI) { if (isInModel()) { change(this, ContextIndexChanged, 0); onContextIndexChanged(_oI); } }
	
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
	
	virtual QString						defineLayout(ViewKeys&) const { return "^;'[]'"; }
	
protected:
	virtual ~Entity();

	virtual void const*					toInterface(Kind) const { return 0; }
	void const*							tryInterface(Kind) const { return 0; }

	enum { DependsOnNothing = 0, DependsOnContext = 1, DependsOnChildren = 2, DependsOnBoth = 3, DependsOnContextIndex = 4 };
	virtual int							familyDependencies() const { return DependsOnNothing; }
	virtual Kinds						ancestralDependencies() const { return Kinds(); }
	
	/// Adds a dependency. Should only be called from inside registerDependencies().
	/// Note this will *not* call onDependencyAdded(_e) for you. You must call it yourself if you want it to run.
	void								addDependency(Entity* _e);
	/// Removes a dependency.
	/// Note this will *not* call onDependencyRemoved(_e) for you. You must call it yourself if you want it to run.
	void								removeDependency(Entity* _e);
	/// Removes all dependencies.
	void								removeAllDependencies();
	
	/// Called when the state of this object has changed.
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
	virtual void						onDependencySwitched(Entity* _e) { onDependencyChanged(_e); }
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
	virtual void						onDependencyRemoved(Entity* _e) { onDependencyChanged(_e); }
	virtual void						onDependencyRemoved(Entity* _e, int /*_index*/) { onDependencyRemoved(_e); }
	
	/// Called when our rootEntity is changing. This currently just means either going into or out of the program model.
	virtual void						onLeaveScene(RootEntity* _new, RootEntity* _old) { (void)_new; (void)_old; }
	
	virtual Entity*						isExpander() const { return 0; }
	
protected:
	RootEntity*							m_rootEntity;

private:
	/// Rejigs our ancestral dependencies. This should be (TODO: and isn't yet) called whenever any of our ancestors have changed context
	/// or been switched.
	void								updateAncestralDependencies();
	
	/// Just makes sure that the rootEntity is the context's root entity. Should only be called from the context.
	void								checkRoot();

	Entity*								m_context;
	int									m_contextIndex;
	QList<Entity*>						m_children;
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
