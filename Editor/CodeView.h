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

#include <QWebView>

#include <Entity.h>
#include <ChangeMan.h>
#include <WebStylist.h>
#include <CodeScene.h>

namespace Martta
{

class CodeView: public QWebView, public CodeScene, public ChangeListener
{
	Q_OBJECT

public:
	CodeView(QWidget* _p = 0);
	~CodeView();

	// Set the current attributes.
	virtual Entity*				subject() const { return &*m_subject; }
	virtual void				setSubject(Entity* _subject);

	// Stylist
	virtual WebStylist*			stylist() const { return m_stylist; };
	virtual void				setStylist(WebStylist* _s);

	// What's happening?
	virtual Entity*				current() const;
	virtual bool				isCurrent(Entity const* _e) const { return _e == current(); }
	virtual bool				isFocusable(Entity const* _e) const;
	virtual bool				isInScene(Entity const* _e) const;
	virtual void				rememberCurrent();
	virtual void				restoreCurrent();

	// Focus changers (often make use of above).
	virtual void				setCurrent(Entity const* _e);
	virtual void				navigateInto(Entity* _centre);
	virtual void				navigateOnto(Entity* _shell);
	virtual void				navigateToNew(Entity* _from);
	virtual void				navigateAway(Entity* _from, NavigationDirection _d = Forwards);

	// Hacks
	virtual void				silentlySetCurrent(Entity* _e) { bool os = m_silent; m_silent = true; setCurrent(_e); m_silent = os; }

public slots:
	void						onCurrentAboutToChange();
	void						onCurrentChanged();
	bool						attemptEdit(int _e);
	void						markDirty(int _e) { relayout((Entity*)_e); }

	void						setShowDependencyInfo(bool _v) { m_showDependencyInfo = _v; update(); }
	void						setShowChanges(bool _v) { m_showChanges = _v; update(); }
	void						setShowOneChange(bool _v) { m_showOneChange = _v; update(); }

signals:
	void						currentChanged(Entity*);

private slots:
	void						onSelectionChanged();

private:
	virtual void						onChanged(Entity* _e, int _aspect) { if (_aspect & Dependee::Visually) relayout(_e); (void)(_aspect); }
	virtual void						onChildrenInitialised(Entity* _e) { relayout(_e); }
	virtual void						onChildAdded(Entity* _e, int _index) { relayout(_e); (void)(_index); }
	virtual void						onChildSwitched(Entity* _e, Entity* _child, Entity* _oldChild) { relayout(_e); (void)(_child); (void)(_oldChild); }
	virtual void						onDependencySwitched(Entity* _e, Entity* _current, Entity* _old) { (void)(_e); (void)(_current); (void)(_old); }
	virtual void						onChildRemoved(Entity* _e, Entity* _old, int _oldIndex) { relayout(_e); (void)(_old); (void)(_oldIndex); }
	virtual void						onChildMoved(Entity* _e, Entity* _child, int _oldIndex) { relayout(_e); (void)(_child); (void)(_oldIndex); }
	virtual void						onParentAdded(Entity* _e) { (void)(_e); }
	virtual void						onParentSwitched(Entity* _e, Entity* _old) { (void)(_e); (void)(_old); }
	virtual void						onParentRemoved(Entity* _e, Entity* _old) { (void)(_e); (void)(_old); }
	virtual void						onAncestorAdded(Entity* _e, Entity* _ancestor) { (void)(_e); (void)(_ancestor); }
	virtual void						onAncestorSwitched(Entity* _e, Entity* _ancestor, Entity* _old) { (void)(_e); (void)(_ancestor); (void)(_old); }
	virtual void						onAncestorRemoved(Entity* _e, Entity* _old) { (void)(_e); (void)(_old); }

	void						init();
	void						refresh();
	QRect						bounds(Entity const* _e) const;

	virtual bool				keyPressedAsNavigation(KeyEvent const&);
	virtual bool				manageKeyPress(KeyEvent const&, Entity const* _fe);

	virtual bool				event(QEvent* _e);
	virtual void				paintEvent(QPaintEvent* _ev);

	virtual void				relayout(Entity* _e);

	List<SafePointer<Entity> >	m_dirty;

	SafePointer<Entity>			m_subject;
	WebStylist*					m_stylist;
	bool						m_silent;
	QPoint						m_remembered;
	SafePointer<Entity>			m_rememberedParent;

	SafePointer<Entity, true>	m_oldCurrent;

	bool						m_showDependencyInfo;
	bool						m_showChanges;
	bool						m_showOneChange;
};

}
