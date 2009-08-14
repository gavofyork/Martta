/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include <QtGui>
#include <QtWebKit>

#include <Stylist.h>
#include <EditDelegate.h>
#include <WebStylistRegistrar.h>

#include "Timer.h"
#include "SpecialKeys.h"
#include "CodeView.h"

namespace Martta
{

CodeView::CodeView(QWidget* _parent):
	QWebView					(_parent),
	m_subject					(0),
	m_stylist					(new Stylist),
	m_silent					(false)
{
	init();
}

CodeView::~CodeView()
{
	delete m_stylist;
}

void CodeView::setSubject(Entity* _s)
{
	m_subject = _s;
	init();
}

void CodeView::setStylist(Stylist* _s)
{
	delete m_stylist;
	m_stylist = _s;
	init();
}

Entity* CodeView::current() const
{
	return (Entity*)(page()->mainFrame()->evaluateJavaScript("g_currentIterator.referenceNode.parentNode.id").toInt());
}

void CodeView::setCurrent(Entity const* _s)
{
	if (!m_silent)
		refresh();
	page()->mainFrame()->evaluateJavaScript(QString("setCurrentById('%1')").arg((int)_s));
}

void CodeView::onCurrentAboutToChange()
{
	if (!m_silent)
		currentAboutToChange();
}

void CodeView::onCurrentChanged(QString const& _oldId)
{
	if (!m_silent)
	{
		Entity* e = current();
		CodeScene::currentChanged(e, (Entity*)(_oldId.toInt()));
		emit currentChanged(e);
		update();
	}
}

void CodeView::navigateInto(Entity* _centre)
{
	/// Selects _centre's leftmost, innermost focusable child. e.g. X on ()s: (++X + 4)
	refresh();
	page()->mainFrame()->evaluateJavaScript(QString("navigateInto('%1');").arg((int)_centre));
}

void CodeView::navigateOnto(Entity* _shell)
{
	/// Selects _shell's leftmost focusable child. e.g. ++X on ()s: (++X + 4)
	refresh();
	page()->mainFrame()->evaluateJavaScript(QString("navigateOnto('%1');").arg((int)_shell));
}

void CodeView::navigateToNew(Entity* _from)
{
	/// Selects closest focusable sibling-owned entity visually forwards from _from, or parent if none.
	refresh();
	page()->mainFrame()->evaluateJavaScript(QString("navigateToNew('%1');").arg((int)_from));
}

void CodeView::navigateAway(Entity* _from, NavigationDirection _d)
{
	refresh();
	page()->mainFrame()->evaluateJavaScript(QString("setCurrentById('%1'); go%2()").arg((int)_from).arg(_d == Forwards ? "Next" : "Previous"));
}

bool CodeView::attemptEdit(int _e)
{
	Entity* e = (Entity*)_e;
	if (isFocusable(e))
		setEditing(e);
	return isEditing(e);
}

bool CodeView::isFocusable(Entity const* _e) const
{
	if (!isInScene(_e))
		return false;
	return page()->mainFrame()->evaluateJavaScript(QString("thisNode(document.getElementById('%1')) != null").arg((int)_e)).toBool();
}

bool CodeView::isInScene(Entity const* _e) const
{
	if (page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1') != null").arg((int)_e)).toBool())
		return true;
	const_cast<CodeView*>(this)->refresh();
	return page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1') != null").arg((int)_e)).toBool();
}

bool CodeView::manageKeyPress(KeyEvent const& _e, Entity const* _fe)
{
	return page()->mainFrame()->evaluateJavaScript(QString("routeKeyPress(document.getElementById('%1'), '%2')").arg((int)_fe).arg(qs(_e.text()).replace('\'', "\\'"))).toBool();
}

void CodeView::refresh()
{
	Entity* e;
	if (WebStylistRegistrar::get()->hasChanged())
		init();
	else if (!m_dirty.isEmpty())
	{
		// Must handle the edit entity (if there is one) separately since refinedHtml (also using from toHtml) doesn't understand
		// that defineHtml should be redirected to the editDelegate if there is one (and couldn't since it doesn't know which is
		// the active CodeScene).
		if (editDelegate())
		{
			QString html = qs(editDelegate()->defineHtml());
			page()->mainFrame()->evaluateJavaScript(QString("thisNode(document.getElementById('%1')).innerHTML = '%2'").arg((int)editEntity()).arg(html.replace('\'', "\\'")));
		}
		// If we're dirty then update the HTML.
		while (m_dirty.count())
			if ((e = m_dirty.takeLast()) && e != editEntity())
			{
				Entity* cur = current();
				QString s;
				foreach (Entity* i, e->children())
					if (m_dirty.contains(i) && i != editEntity())
						m_dirty.removeAll(i);
					else if ((s = page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').outerHTML").arg((int)i)).toString().replace('\\', "&#92;")) != QString::null)
						addToHtmlCache(i, qs(s));
				page()->mainFrame()->evaluateJavaScript(QString("changeContent('%1', '%2')").arg((int)e).arg(qs(refinedHtml(e)).replace('\'', "\\'")));
				clearHtmlCache();
				silentlySetCurrent(cur);
			}
	}
	m_dirty.clear();
}

void CodeView::paintEvent(QPaintEvent* _ev)
{
	refresh();
	QWebView::paintEvent(_ev);
	if (current())
	{
		QPainter p(this);
		QRect b = bounds(current());
		p.setPen(QColor().black());
		p.drawRect(b);
	}
}

QRect CodeView::bounds(Entity const* _e) const
{
	return QRect(	page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').getBoundingClientRect().left").arg((int)_e)).toInt(),
					page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').getBoundingClientRect().top").arg((int)_e)).toInt(),
					page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').getBoundingClientRect().width").arg((int)_e)).toInt(),
					page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').getBoundingClientRect().height").arg((int)_e)).toInt());
}

void CodeView::init()
{
	QString css = qs(WebStylistRegistrar::get()->css());
	setHtml(QString("<!DOCTYPE HTML><html><head><style type=\"text/css\">%1</style></head><body onmousedown=\"procMouseDown(event)\">%2</body></html>").arg(css).arg(qs(m_subject ? toHtml(m_subject) : String::null)));
	page()->mainFrame()->addToJavaScriptWindowObject("CodeView", this);
	QFile support(":/CodeView/Support.js");
	support.open(QFile::ReadOnly);
	page()->mainFrame()->evaluateJavaScript(support.readAll().data());
	m_dirty.clear();
}

void CodeView::markDirty(Entity* _e)
{
	if (!m_dirty.contains(_e))
		m_dirty.append(_e);
	update();
}

bool CodeView::keyPressedAsNavigation(KeyEvent const& _e)
{
	if (_e.text() == UpKey)
		page()->mainFrame()->evaluateJavaScript("goUp()");
	else if (_e.text() == DownKey)
		page()->mainFrame()->evaluateJavaScript("goDown()");
	else if (_e.text() == LeftKey)
		page()->mainFrame()->evaluateJavaScript("goPrevious()");
	else if (_e.text() == RightKey)
		page()->mainFrame()->evaluateJavaScript("goNext()");
	else if (_e.text() == PageUpKey)
		page()->mainFrame()->evaluateJavaScript("goOut()");
	else if (_e.text() == PageDownKey)
		page()->mainFrame()->evaluateJavaScript("goIn()");
	else
		return CodeScene::keyPressedAsNavigation(_e);
	return true;
}

bool CodeView::event(QEvent* _e)
{
	if (_e->type() == QEvent::KeyPress && subject() && current())
	{
		QKeyEvent* ke = static_cast<QKeyEvent*>(_e);
		wchar_t const* k = translateKey(ke->key());
		KeyEvent e(k ? String(k) : String(ke->text().toLatin1().data()), translateMods(ke->modifiers()));
		if (e.text().length() > 0)
		{
			keyPressHandler(e);
			if (e.isAccepted())
			{
				_e->accept();
				return true;
			}
		}
	}
	return QWebView::event(_e);
}

}
