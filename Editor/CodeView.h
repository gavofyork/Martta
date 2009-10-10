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

#include <Concept.h>
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
	virtual Concept*						subject() const { return &*m_subject; }
	virtual void						setSubject(Concept* _subject);

	// Stylist
	virtual WebStylist*					stylist() const { return m_stylist; };
	virtual void						setStylist(WebStylist* _s);

	// What's happening?
	virtual Concept*						current() const;
	virtual bool						isCurrent(Concept const* _e) const { return _e == current(); }
	virtual bool						isFocusable(Concept const* _e) const;
	virtual bool						isInScene(Concept const* _e) const;
	virtual void						rememberCurrent();
	virtual void						restoreCurrent();

	// Focus changers (often make use of above).
	virtual void						setCurrent(Concept const* _e);
	virtual void						navigateInto(Concept* _centre);
	virtual void						navigateOnto(Concept* _shell);
	virtual void						navigateToNew(Concept* _from);
	virtual void						navigateAway(Concept* _from, NavigationDirection _d = Forwards);

	// Hacks
	virtual void						silentlySetCurrent(Concept* _e) { bool os = m_silent; m_silent = true; setCurrent(_e); m_silent = os; }

public slots:
	void								onCurrentAboutToChange();
	void								onCurrentChanged();
	bool								attemptEdit(int _e);
	void								markDirty(int _e) { relayout((Concept*)_e); }

	void								setShowDependencyInfo(bool _v) { m_showDependencyInfo = _v; update(); }
	void								setShowChanges(bool _v) { m_showChanges = _v; update(); }
	void								setShowOneChange(bool _v) { m_showOneChange = _v; update(); }
	void								setShowInvalids(bool _v) { m_showInvalids = _v; update(); }

signals:
	void								currentChanged(Concept*);

private slots:
	void								onSelectionChanged();

private:
	virtual void						onChanged(Concept* _e, int _aspect) { if (_aspect & Dependee::Visually) relayout(_e); else if (_aspect & Dependee::Logically) checkInvalid(_e); }
	virtual void						onChildrenInitialised(Concept* _e) { relayout(_e); foreach (Concept* e, _e->children()) checkInvalid(e); }
	virtual void						onChildAdded(Concept* _e, int _index) { relayout(_e); checkInvalid(_e->child(_index)); }
	virtual void						onChildSwitched(Concept* _e, Concept* _child, Concept* _oldChild) { relayout(_e); checkInvalid(_child); (void)(_oldChild); }
	virtual void						onDependencySwitched(Concept* _e, Concept* _current, Concept* _old) { checkInvalid(_e); (void)(_e); (void)(_current); (void)(_old); }
	virtual void						onChildRemoved(Concept* _e, Concept* _old, int _oldIndex) { relayout(_e); (void)(_old); (void)(_oldIndex); }
	virtual void						onChildMoved(Concept* _e, Concept* _child, int _oldIndex) { relayout(_e); checkInvalid(_child); checkInvalid(_e->child(_oldIndex)); }
	virtual void						onParentAdded(Concept* _e) { (void)(_e); }
	virtual void						onParentSwitched(Concept* _e, Concept* _old) { (void)(_e); (void)(_old); }
	virtual void						onParentRemoved(Concept* _e, Concept* _old) { (void)(_e); (void)(_old); }
	virtual void						onAncestorAdded(Concept* _e, Concept* _ancestor) { checkInvalid(_e); (void)(_ancestor); }
	virtual void						onAncestorSwitched(Concept* _e, Concept* _ancestor, Concept* _old) { checkInvalid(_e); (void)(_ancestor); (void)(_old); }
	virtual void						onAncestorRemoved(Concept* _e, Concept* _old) { checkInvalid(_e); (void)(_old); }

	void								init();
	void								refresh();
	void								checkInvalids();
	QRect								bounds(Concept const* _e) const;
	inline void							checkInvalid(Concept* _e) { if (!m_invalidsToCheck.contains(_e) && isInScene(_e)) m_invalidsToCheck.append(_e); }

	virtual bool						keyPressedAsNavigation(KeyEvent const&);
	virtual bool						manageKeyPress(KeyEvent const&, Concept const* _fe);

	virtual bool						event(QEvent* _e);
	virtual void						paintEvent(QPaintEvent* _ev);

	virtual void						relayout(Concept* _e);

	List<SafePointer<Concept> >			m_dirty;

	SafePointer<Concept>					m_subject;
	WebStylist*							m_stylist;
	bool								m_silent;
	SafePointer<Concept, true>			m_oldCurrent;
	QPoint								m_remembered;
	SafePointer<Concept>					m_rememberedParent;

	bool								m_showDependencyInfo;
	bool								m_showChanges;
	bool								m_showOneChange;

	bool								m_showInvalids;
	List<SafePointer<Concept> >			m_invalidsToCheck;
	List<SafePointer<Concept> >			m_invalids;
};

}
