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
#include <Stylist.h>
#include <CodeScene.h>

namespace Martta
{

class CodeView: public QWebView, public CodeScene
{
	Q_OBJECT

public:
	CodeView(QWidget* _p = 0);
	~CodeView();

	// Set the current attributes.
	virtual Entity*				subject() const { return &*m_subject; }
	virtual void				setSubject(Entity* _subject);

	// Stylist
	virtual Stylist*			stylist() const { return m_stylist; };
	virtual void				setStylist(Stylist* _s);

	// What's happening?
	virtual Entity*				current() const;
	virtual bool				isCurrent(Entity const* _e) const { return _e == current(); }
	virtual bool				isFocusable(Entity const* _e) const;
	virtual bool				isInScene(Entity const* _e) const;

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
	void						onCurrentChanged(QString const& _old);
	bool						attemptEdit(int _e);
	void						markDirty(int _e) { markDirty((Entity*)_e); }

signals:
	void						currentChanged(Entity*);

private:
	void						init();
	void						refresh();
	QRect						bounds(Entity const* _e) const;

	virtual bool				keyPressedAsNavigation(KeyEvent const&);
	virtual bool				manageKeyPress(KeyEvent const&, Entity const* _fe);

	virtual bool				event(QEvent* _e);
	virtual void				paintEvent(QPaintEvent* _ev);

	virtual void				markDirty(Entity* _e);

	List<SafePointer<Entity> >	m_dirty;

	SafePointer<Entity>			m_subject;
	Stylist*					m_stylist;
	bool						m_silent;
};

}
