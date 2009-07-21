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

#include <msStringList.h>
#include <msString.h>
#undef inline
using namespace MarttaSupport;

#include <QSet>
#include <QWidget>
#include <QPicture>

#include <Entity.h>
#include <CodeScene.h>
#undef inline

namespace Martta
{

class EditDelegateFace;

class CodeView: public QWidget, public CodeScene
{
	Q_OBJECT

public:
	CodeView(QWidget* _p = 0);
	~CodeView();

	// Set the current attributes.
	void						setSubject(Entity* _subject);
	void						setShowDependencyInfo(bool _on) { if (m_showDependencyInfo == _on) return; m_showDependencyInfo = _on; update(); }
	void						setShowChanges(bool _on) { if (m_showChanges == _on) return; m_showChanges = _on; update(); }
	void						setShowOneChange(bool _on) { if (m_showOneChange == _on) return; m_showOneChange = _on; update(); }

	// Stylist
	Stylist*				stylist() const { return m_stylist; }
	void						setStylist(Stylist* _s) { m_stylist = _s; }

	// What's happening?
	inline Entity*				current() const { return m_current ? (Entity*)m_current : m_subject; }
	Entity*						editEntity() const;
	inline EditDelegateFace*	editDelegate() const { return m_editDelegate; }
	inline bool					isCurrent(Entity* _e) const { return current() == _e; }
	inline bool					isEditing(Entity* _e) const { return editEntity() == _e; }
	inline bool					isInScene(Entity* _e) const { return m_visible.contains(_e); }
	inline bool					isFocusable(Entity* _e) const { return _e && m_orders[_e->parent()].contains(_e); }
	QRectF						bounds(Entity* _e);
	Entity*						at(QPointF const&) const;

	// Relative determiners.
	Entity*						next(Entity* _e) const { List<Entity*> o = m_orders[m_subject]; for (int i = 0; i < o.size() - 1; i++) if (o[i] == _e) return o[i + 1]; return 0; }
	Entity*						previous(Entity* _e) const { List<Entity*> o = m_orders[m_subject]; for (int i = 1; i < o.size(); i++) if (o[i] == _e) return o[i - 1]; return 0; }
	Entity*						traverse(Entity* _e, bool _upwards, float _x);
	Entity*						nearest(Entity* _e);

	// Focus changers (often make use of above).
	void						setCurrent(Entity* _e);											/// Selects the focusable entity nearest to _e.
	void						navigateInto(Entity* _centre);									/// Selects _centre's leftmost, innermost focusable child. e.g. X on ()s: (++X + 4)
	void						navigateOnto(Entity* _shell);									/// Selects _shell's leftmost focusable child. e.g. ++X on ()s: (++X + 4)
	void						navigateAway(Entity* _from, NavigationDirection _d = Forwards);	/// Selects closest focusable entity visually _d from _from. e.g. 4 on ()s: (++X + 4)
	void						navigateToNew(Entity* _from);									/// Selects closest focusable sibling-owned entity visually forwards from _from, or parent if none.

	String						layoutString(Entity* _e);		///< @returns the layout string for the entity _e.
	StringList					layoutList(Entity* _e) { if (!m_listCache.contains(_e)) recacheLayoutList(_e, layoutString(_e)); return m_listCache[_e]; }			///< @returns the layout list for the entity _e.

	/// Marks the given entity as requiring a repaint.
	bool						markDirty(Entity* _e);
	/// Sets the local (relative to parent) bounds of the given entity. This is called only by the layout code.
	void						setLocalBounds(Entity* _e, QRectF const& _br) { m_bounds[_e] = _br; }

	/// @returns the status of the insertion flag.
	bool						insert() const { return m_insert; }
	/// Notifies us that the current key event should be recycled (i.e. handled again).
	void						reinterpretCurrentKeyEvent() { m_reinterpretCurrentKeyEvent = true; }

	// Hacks
	void						silentlySetCurrent(Entity* _e);									/// Selects the focusable entity _e; this will do nothing unless you're leaving the edit.

public slots:
	/// Set the focused item to that which represents _e.
	void						setEditing(Entity* _e);
	void						leaveEdit();

	void						repaint(Entity* _e);
	void						relayout(Entity* _e);
	/// Resets the parent's defineLayout cache.
	void						resetLayoutCache(Entity* _e);
	/// For when an entity has changed in the scene.
	void						relayoutLater(Entity* _e);
	inline void					leaving(Entity* _e) { leaving(_e, _e->over()); }
	void						leaving(Entity* _e, Position const& _grave);

	void						killStrobe();

signals:
	/// A new entity has been selected.
	void						currentChanged(Entity* _e);

protected:
	virtual void				paintEvent(QPaintEvent* _e);
	virtual void				keyPressEvent(QKeyEvent* _e);
	virtual void				mouseMoveEvent(QMouseEvent* _e);
	virtual void				mousePressEvent(QMouseEvent* _e);
	virtual void				mouseDoubleClickEvent(QMouseEvent* _e);
	virtual bool				event(QEvent* _e);

private:
	bool						keyPressedAsNavigation(QKeyEvent const* _e);
	void						doRefreshLayout();
	void						recacheLayoutList(Entity* _e, String const& _s);			///< @returns the layout list for the entity _e.

	// Properties
	Entity*				m_subject;
	Entity*				m_current;
	bool				m_showDependencyInfo;
	bool				m_showChanges;
	bool				m_showOneChange;
	bool				m_leavingEdit;
	bool				m_showHover;

	// State
	Entity*				m_hover;
	Entity*				m_lastRealCurrent;

	Hash<Entity*, QPicture>				m_pictures;
	Hash<Entity*, QRectF>					m_bounds;
	Hash<Entity*, List<Entity*> >			m_orders;
	QSet<Entity*>							m_visible;
	Hash<Entity*, Entity*>					m_leftmostChild;
	Hash<Entity*, Entity*>					m_rightmostChild;
	Hash<Entity*, StringList>				m_listCache;
	Hash<Entity*, uint>					m_cacheKey;

	List<int>			m_pagingRoute;
	bool				m_navigated;
	SafePointer<Entity>	m_strobeCreation;
	SafePointer<Entity>	m_strobeChild;
	SafePointer<Entity>	m_strobeFocus;
	String				m_strobeText;
	bool				m_insert;
	bool				m_doInsert;
	bool				m_insertLock;
	float				m_lastDefiniteX;
	bool				m_reinterpretCurrentKeyEvent;
	Stylist*		m_stylist;
	QPointF				m_borderOffset;
	bool				m_ensureCurrentVisible;
};

}

