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

#include <WebStylist.h>
#include <WebStylistRegistrar.h>
#include <EditDelegate.h>

#include "Timer.h"
#include "SpecialKeys.h"
#include "CodeView.h"

namespace Martta
{

CodeView::CodeView(QWidget* _parent):
	QWebView					(_parent),
	m_subject					(0),
	m_stylist					(new WebStylist),
	m_silent					(false),
	m_showDependencyInfo		(true),
	m_showChanges				(false),
	m_showOneChange				(false),
	m_showInvalids				(true)
{
	init();
	connect(this, SIGNAL(selectionChanged()), SLOT(onSelectionChanged()));
	setAttribute(Qt::WA_OpaquePaintEvent, false);
	setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);
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

void CodeView::setStylist(WebStylist* _s)
{
	delete m_stylist;
	m_stylist = _s;
	init();
}

Entity* CodeView::current() const
{
	Entity* r = (Entity*)(page()->mainFrame()->evaluateJavaScript("document.getElementById(g_currentIterator.referenceNode.parentNode.id).id").toInt());
	if (r)
		return r;
	const_cast<CodeView*>(this)->restoreCurrent();
	return (Entity*)(page()->mainFrame()->evaluateJavaScript("document.getElementById(g_currentIterator.referenceNode.parentNode.id).id").toInt());
}

void CodeView::rememberCurrent()
{
	m_remembered = bounds(current()).topLeft() + QPoint(5, 5);
	m_rememberedParent = current()->parent();
}

void CodeView::restoreCurrent()
{
	refresh();
	bool s = m_silent;
	m_silent = true;
	page()->mainFrame()->evaluateJavaScript(QString("restoreCurrent(%1, %2, %3)").arg(m_remembered.x()).arg(m_remembered.y()).arg((int)&*m_rememberedParent));
	m_silent = s;
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

void CodeView::onCurrentChanged()
{
	if (!m_silent)
	{
		Entity* e = current();
		CodeScene::currentChanged(e, m_oldCurrent);
		m_oldCurrent = e;
		rememberCurrent();
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
		// If we're dirty then update the HTML.
		while (m_dirty.count())
		{
			if ((e = m_dirty.takeLast()))
			{
				Entity* cur = current();
				QString s;
				foreach (Entity* i, e->children())
					if ((s = page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').outerHTML").arg((int)i)).toString().replace('\\', "&#92;")) != QString::null)
						m_stylist->addToHtmlCache(i, qs(s));
				page()->mainFrame()->evaluateJavaScript(QString("changeContent('%1', '%2')").arg((int)e).arg(qs(m_stylist->rejiggedHtml(e)).replace('\'', "\\'")));
				silentlySetCurrent(cur);
			}
		}
	}
	m_dirty.clear();
}

inline QRect operator+(QRect const& _a, int _x)
{
	return QRect(_a.x() - _x, _a.y() - _x, _a.width() + 2 * _x, _a.height() + 2 * _x);
}

void CodeView::onSelectionChanged()
{
	mInfo();
}

void CodeView::checkInvalids()
{
	m_invalids.removeAll(0);
	while (m_invalidsToCheck.count())
		if (Entity* e = m_invalidsToCheck.takeLast())
		{
			bool validity = e->isValid();
			if (validity && m_invalids.contains(e))
				m_invalids.removeOne(e);
			else if (!validity && !m_invalids.contains(e))
				m_invalids.append(e);
		}
}

void CodeView::paintEvent(QPaintEvent* _ev)
{
	TIME_FUNCTION;
	refresh();
	checkInvalids();
	Entity* c = current();

	foreach (Position i, m_bracketed)
		if (!i.exists() || (i.entity() != c && !c->hasAncestor(i.entity())))
			m_bracketed.removeAll(i);

	if (c)
	{
		QPainter p(this);

		p.fillRect(rect(), QColor(255, 255, 255, 235));

//		p.setRenderHint(QPainter::Antialiasing, true);
		QRect br = bounds(c);
		br = QRect(0, br.y(), width(), br.height());
		if (_ev->region().contains(br))
		{
			QLinearGradient g(br.topLeft(), br.bottomLeft());
			g.setColorAt(0.f, editDelegate() ? QColor(224, 0, 0, 64) : QColor(0, 128, 255, 16));
			g.setColorAt(1.f, editDelegate() ? QColor(224, 0, 0, 16) : QColor(0, 128, 255, 48));
			p.setPen(Qt::NoPen);
			p.setBrush(g);
			p.drawRect(br);
		}
	}
	TIME_STATEMENT("WVpaintEvent")
		QWebView::paintEvent(_ev);

	if (c)
	{
		QPainter p(this);
		QRect br = bounds(c);

		p.setPen(Qt::NoPen);
		foreach (Position i, m_bracketed)
		{
			QRect br = bounds(i.entity());
			QRect obr = QRect(br.x() - 2.f, br.y(), br.width() + 4.f, br.height());
			if (_ev->region().contains(obr))
			{
				QLinearGradient g(br.topLeft(), br.bottomLeft());
				g.setColorAt(0.f, QColor(224, 255, 192, 32));
				g.setColorAt(1.f, QColor(192, 255, 127, 32));
				QLinearGradient go(br.topLeft(), br.bottomLeft());
				go.setColorAt(0.f, QColor(224, 255, 192, 128));
				go.setColorAt(1.f, QColor(192, 255, 127, 128));
				p.setBrush(g);
				p.drawRect(br);
				p.setBrush(go);
				p.drawRect(obr);
			}
		}

		if (_ev->region().contains(br + 3))
		{
			p.setBrush(Qt::NoBrush);
			p.setPen(editDelegate() ? QColor(255, 0, 0, 128) : QColor(0, 64, 128, 128));
			p.drawRect(br);
			p.setPen(editDelegate() ? QColor(255, 0, 0, 64) : QColor(0, 64, 128, 64));
			p.drawRect(br + 1.f);
			p.setPen(editDelegate() ? QColor(255, 0, 0, 32) : QColor(0, 64, 128, 32));
			p.drawRect(br + 2.f);
			p.setPen(editDelegate() ? QColor(255, 0, 0, 16) : QColor(0, 64, 128, 16));
			p.drawRect(br + 3.f);
		}

		if (m_showDependencyInfo)
		{
			p.setBrush(Qt::NoBrush);
			p.setPen(QColor(0, 255, 0, 128));
			foreach (Dependee* i, ChangeMan::get()->dependeesOf(c))
			{
				if (i && isInScene(i->self()))
				{
					QRect br = bounds(i->self());
					if (_ev->region().contains(br))
						p.drawRect(br);
				}
			}
			p.setPen(QColor(255, 0, 0, 128));
			foreach (Depender* i, ChangeMan::get()->dependersOf(c))
				if (isInScene(i->self()))
				{
					QRect br = bounds(i->self());
					if (_ev->region().contains(br))
						p.drawRect(br);
				}
		}
	}
	if (m_showInvalids)
	{
		QPainter p(this);
		p.setPen(QPen(QColor(255, 0, 0, 128), 1.5f, Qt::DotLine));
		p.setBrush(QBrush(QColor(0, 0, 0, 64), Qt::BDiagPattern));
		foreach (Entity* e, m_invalids)
			if (e != c || !editDelegate())
			{
				QRect b = bounds(e);
				if (_ev->region().contains(b))
					p.drawRect(b);
			}
	}
	if ((m_showChanges || m_showOneChange) && !ChangeMan::get()->changesDone().isEmpty())
	{
		QPainter p(this);
		List<ChangeMan::Entry>& changes = ChangeMan::get()->changesDone();
		foreach (ChangeMan::Entry i, m_showOneChange ? changes.mid(0, 1) : changes)
			if (isInScene(i.m_depender))
			{
				QColor c;
				switch (i.m_op)
				{
					case ChangeMan::DependencyChanged: c = QColor(0, 0, 255); break;
					case ChangeMan::DependencySwitched: c = QColor(0, 127, 255); break;
					case ChangeMan::DependencyAdded: c = QColor(0, 127, 0); break;
					case ChangeMan::EntityChildrenInitialised: c = QColor(0, 255, 0); break;
					case ChangeMan::DependencyRemoved: c = QColor(255, 0, 0); break;
					case ChangeMan::ChildMoved: c = QColor(0, 255, 255); break;
					case ChangeMan::IndexChanged: c = QColor(255, 0, 255); break;
				}
				p.setPen(QPen(c, 2.f));
				p.setBrush(QColor(c.red(), c.green(), c.blue(), 64));
				QRect br = bounds(i.m_depender) + 1.f;
				if (_ev->region().contains(br))
					p.drawRect(br);
				if (i.m_object1 && isInScene(i.m_object1))
				{
					QRect obr = bounds(i.m_object1) + 1.f;
					if (_ev->region().contains(QRect(br.center(), obr.center())))
						p.drawLine(br.center(), obr.center());
					p.setPen(QPen(QColor(0, 0, 0, 128), 2.f));
					if (_ev->region().contains(obr))
						p.drawRect(obr);
				}
			}
	}
}

QRect CodeView::bounds(Entity const* _e) const
{
	QStringList l = page()->mainFrame()->evaluateJavaScript(QString("bounds('%1')").arg((int)_e)).toString().split(" ");
	if (l.size() == 4)
		return QRect(l[0].toInt(), l[1].toInt(), l[2].toInt(), l[3].toInt());
	else
		return QRect(-1, -1, 0, 0);
}

void CodeView::init()
{
	QString css = qs(WebStylistRegistrar::get()->css());
	setHtml(QString("<!DOCTYPE HTML><html><head><style type=\"text/css\">%1</style></head><body onmousedown=\"procMouseDown(event)\">%2</body></html>").arg(css).arg(qs(m_stylist->toHtml(m_subject))));
	page()->mainFrame()->addToJavaScriptWindowObject("CodeView", this);
	QFile support(":/CodeView/Support.js");
	support.open(QFile::ReadOnly);
	page()->mainFrame()->evaluateJavaScript(support.readAll().data());
	QPalette pal = palette();
	pal.setBrush(QPalette::Base, Qt::transparent);
	page()->setPalette(pal);
	m_dirty.clear();
	m_invalidsToCheck.clear();
	List<Entity*> q;
	q.append(m_subject);
	while (q.size())
		if (Entity* e = q.takeLast())
		{
			m_invalidsToCheck.append(e);
			foreach (Entity* i, e->children())
				q.append(i);
		}
}

void CodeView::relayout(Entity* _e)
{
	if (_e && _e == editEntity())
	{
		// Must handle the edit entity (if there is one) separately since refinedHtml (also using from toHtml) doesn't understand
		// that defineHtml should be redirected to the editDelegate if there is one (and couldn't since it doesn't know which is
		// the active CodeScene).
		QString html = qs(m_stylist->editHtml(_e, this));
		page()->mainFrame()->evaluateJavaScript(QString("changeEditContent(%1, '%2')").arg((int)_e).arg(html.replace('\'', "\\'")));
		update();
	}
	else if (isInScene(_e))
	{
		if (!m_invalidsToCheck.contains(_e))
			m_invalidsToCheck.append(_e);
		if (!m_dirty.contains(_e))
			m_dirty.append(_e);
		update();
	}
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
