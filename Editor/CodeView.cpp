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
	m_showInvalids				(true),
	m_routing					(0)
{
	m_stylist->setCodeScene(this);
	init();

	connect(this, SIGNAL(selectionChanged()), SLOT(onSelectionChanged()));
	setAutoFillBackground(true);
//	setAttribute(Qt::WA_OpaquePaintEvent, false);
#ifndef Q_WS_WIN
//	setRenderHint(QPainter::TextAntialiasing, true);
//	setRenderHint(QPainter::Antialiasing, true);
#endif

	m_properties = defaultProperties();
}

CodeView::~CodeView()
{
	delete m_stylist;
}

void CodeView::onConfigure()
{
	QDialog d(this);
	d.setLayout(new QVBoxLayout);
	Hash<String, String> p = properties();
	QTableWidget* t = new QTableWidget(p.size(), 1, &d);
	t->setVerticalHeaderLabels(qs(p.keys()));
	int i = 0;
	foreach (String s, p.values())
		t->setItem(i++, 0, new QTableWidgetItem(qs(s)));

	d.layout()->addWidget(t);
	d.setModal(true);
	d.exec();

	i = 0;
	foreach (String s, p.keys())
	{
		if (qs(p[s]) != t->item(i, 0)->text())
			setProperty(s, qs(t->item(i, 0)->text()));
		i++;
	}
	delete t;
	
	init();
}

Hash<String, String> CodeView::defaultProperties() const
{
	Hash<String, String> ret;
	ret[L"Id-Martta::Argument"] = L"<span class=\"Argument-Fader\"><span class=\"Argument\">_</span></span>fooBar";
	ret[L"Id-Martta::Field"] = L"<span class=\"Argument-Fader\"><span class=\"Member\">M</span></span>fooBar";
	ret[L"Id-Martta::TopLevelType"] = L"FooBar";
	ret[L"Id-Martta::Namespace"] = L"FooBar";
	ret[L"Id-Martta::TypeArgument"] = L"FooBar";
	ret[L"Id-Martta::Variable"] = L"fooBar";
	ret[L"Id-Martta::Function"] = L"fooBar";

	ret[L"Id-Martta::Primary"] = L"fooBar";

	ret[L"Id-Martta::MemberValue"] = L"fooBar";
	ret[L"Id-Martta::MemberLambda"] = L"fooBar";
	ret[L"Id-Martta::MemberEnumeration"] = L"FooBar";
	ret[L"Operation-Parenthesise"] = L"false";
	ret[L"SimpleBinaryOperation-ProperMaths"] = L"true";
	ret[L"CSS-Simple"] = L"false";
	ret[L"CodeView-Preview Brackets"] = L"false";
	return ret;
}

void CodeView::onPropertiesChanged()
{
	m_stylist->setProperties(properties());
}

QString CodeView::toHtml() const
{
	QFile support(":/CodeView/Support.js");
	support.open(QFile::ReadOnly);
	return QString("<!DOCTYPE HTML><html><head><script type=\"text/javascript\">%3</script><style type=\"text/css\">%1</style></head><body onmousedown=\"procMouseDown(event)\">%2</body></html>").arg(qs(m_stylist->css())).arg(qs(m_stylist->toHtml(m_subject))).arg(support.readAll().data());
}

void CodeView::renderTo(QPaintDevice* _dev)
{
	QPainter p(_dev);
	page()->mainFrame()->render(&p);
}

void CodeView::setSubject(Concept* _s)
{
	m_subject = _s;
	init();
}

void CodeView::setStylist(WebStylist* _s)
{
	delete m_stylist;
	m_stylist = _s;
	_s->setCodeScene(this);
	init();
}

Concept* CodeView::current() const
{
	Concept* r = (Concept*)(page()->mainFrame()->evaluateJavaScript("document.getElementById(g_currentIterator.referenceNode.parentNode.id).id").toInt());
	if (r)
		return r;
	const_cast<CodeView*>(this)->restoreCurrent();
	return (Concept*)(page()->mainFrame()->evaluateJavaScript("document.getElementById(g_currentIterator.referenceNode.parentNode.id).id").toInt());
}

void CodeView::rememberCurrent()
{
	if (current())
	{
		m_remembered = bounds(current()).topLeft() + QPoint(5, 5);
		m_rememberedParent = current()->parent();
	}
}

void CodeView::restoreCurrent()
{
	refresh();
	bool s = m_silent;
	m_silent = true;
	page()->mainFrame()->evaluateJavaScript(QString("restoreCurrent(%1, %2, %3)").arg(m_remembered.x()).arg(m_remembered.y()).arg((long)&*m_rememberedParent));
	m_silent = s;
}

void CodeView::setCurrent(Concept const* _s)
{
	if (!m_silent)
		refresh();
	page()->mainFrame()->evaluateJavaScript(QString("setCurrentById('%1')").arg((long)_s));
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
		Concept* e = current();
//		qDebug() << "onCurrentChanged:"<<e;
		CodeScene::currentChanged(e, m_oldCurrent);
		m_oldCurrent = e;
		rememberCurrent();
		emit currentChanged(e);
		update();
	}
}

void CodeView::navigateInto(Concept* _centre)
{
	/// Selects _centre's leftmost, innermost focusable child. e.g. X on ()s: (++X + 4)
	refresh();
	page()->mainFrame()->evaluateJavaScript(QString("navigateInto('%1');").arg((long)_centre));
}

void CodeView::navigateOnto(Concept* _shell)
{
	/// Selects _shell's leftmost focusable child. e.g. ++X on ()s: (++X + 4)
	refresh();
	page()->mainFrame()->evaluateJavaScript(QString("navigateOnto('%1');").arg((long)_shell));
}

void CodeView::navigateToNew(Concept* _from)
{
	/// Selects closest focusable sibling-owned entity visually forwards from _from, or parent if none.
	refresh();
	page()->mainFrame()->evaluateJavaScript(QString("navigateToNew('%1');").arg((long)_from));
}

void CodeView::navigateAway(Concept* _from, NavigationDirection _d)
{
	refresh();
	page()->mainFrame()->evaluateJavaScript(QString("setCurrentById('%1'); go%2()").arg((long)_from).arg(_d == Forwards ? "Next" : "Previous"));
}

bool CodeView::attemptEdit(int _e)
{
	Concept* e = (Concept*)_e;
	if (isFocusable(e))
		setEditing(e);
	return isEditing(e);
}

bool CodeView::isFocusable(Concept const* _e) const
{
	if (!isInScene(_e))
		return false;
	return page()->mainFrame()->evaluateJavaScript(QString("thisNode(document.getElementById('%1')) != null").arg((long)_e)).toBool();
}

bool CodeView::isInScene(Concept const* _e) const
{
	if (page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1') != null").arg((long)_e)).toBool())
		return true;
	const_cast<CodeView*>(this)->refresh();
	return page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1') != null").arg((long)_e)).toBool();
}

bool CodeView::manageKeyPress(KeyEvent const& _e, Concept const* _fe)
{
	return page()->mainFrame()->evaluateJavaScript(QString("routeKeyPress(document.getElementById('%1'), '%2')").arg((long)_fe).arg(qs(_e.text()).replace('\'', "\\'"))).toBool();
}

void CodeView::refresh()
{
	Concept* e;
	if (WebStylistRegistrar::get()->hasChanged())
		init();
	else if (!m_dirty.isEmpty())
	{
		// If we're dirty then update the HTML.
		while (m_dirty.count())
		{
			if ((e = m_dirty.takeLast()))
			{
				Concept* cur = current();
				QString s;
				foreach (Concept* i, e->children())
					if ((s = page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').outerHTML").arg((long)i)).toString().replace('\\', "&#92;")) != QString::null)
						m_stylist->addToHtmlCache(i, qs(s));
				String h = m_stylist->rejiggedHtml(e);
				if (m_stylist->property(L"CodeView", L"Preview Brackets").toBool())
					for (int i = 0; i < isBracketed(e->over()); i++)
						if (m_stylist->property(L"CSS", L"Simple").toBool())
							h = L"(" + h + L")";
						else
							h = L"<span class=\"symbol\">(</span>" + h + L"<span class=\"symbol\">)</span>";

				page()->mainFrame()->evaluateJavaScript(QString("changeContent('%1', '%2')").arg((long)e).arg(qs(h).replace('\'', "\\'")));
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
	mInfo() << "";
}

void CodeView::checkInvalids()
{
	m_invalids.removeAll(0);
	while (m_invalidsToCheck.count())
		if (Concept* e = m_invalidsToCheck.takeLast())
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
//	TIME_FUNCTION;
	Concept* c = current();
	foreach (Position i, m_bracketed)
		if (!i.exists() || (i.concept() != c && !c->hasAncestor(i.concept())))
			removeBracket(i);

//	TIME_STATEMENT("refresh")
	refresh();
//	TIME_STATEMENT("checkInvalids")
	checkInvalids();
	c = current();
//	mInfo() << c;

/*	{
		QPainter p(this);
		p.fillRect(rect(), Qt::white);
	}

	if (c)
	{
		QPainter p(this);
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
	}*/
//	TIME_STATEMENT("WVpaintEvent")
		QWebView::paintEvent(_ev);

	if (c)
	{
		QPainter p(this);
		QRect br = bounds(c);

		p.setPen(Qt::NoPen);
		foreach (Position i, m_bracketed)
		{
			QRect br = bounds(i.concept());
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
			QRect marker;
			for (int i = 0; i < 2; i++)
				if (_ev->region().contains(marker = QRect(i ? width() - 8 - page()->mainFrame()->scrollBarGeometry(Qt::Vertical).width() : 0, br.y(), 8, br.height())))
				{
					QLinearGradient g(marker.topLeft(), marker.bottomLeft());
					g.setColorAt(0.f, editDelegate() ? QColor(224, 0, 0, 255) : QColor(0, 128, 255, 64));
					g.setColorAt(1.f, editDelegate() ? QColor(224, 0, 0, 64) : QColor(0, 128, 255, 192));
					p.setPen(Qt::NoPen);
					p.setBrush(g);
					p.drawRect(marker);
				}
		}

		if (m_showDependencyInfo)
		{
			p.setBrush(Qt::NoBrush);
			p.setPen(QColor(0, 255, 0, 128));
			foreach (Dependee* i, ChangeMan::get()->dependeesOf(c))
			{
				if (i && isInScene(i->concept()))
				{
					QRect br = bounds(i->concept());
					if (_ev->region().contains(br))
						p.drawRect(br);
				}
			}
			p.setPen(QColor(255, 0, 0, 128));
			foreach (Depender* i, ChangeMan::get()->dependersOf(c))
				if (isInScene(static_cast<Concept*>(i)))
				{
					QRect br = bounds(static_cast<Concept*>(i));
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
		foreach (Concept* e, m_invalids)
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
					case ChangeMan::ConceptChildrenInitialised: c = QColor(0, 255, 0); break;
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

QRect CodeView::bounds(Concept const* _e) const
{
	QStringList l = page()->mainFrame()->evaluateJavaScript(QString("bounds('%1')").arg((long)_e)).toString().split(" ");
	if (l.size() == 4)
		return QRect(l[0].toInt(), l[1].toInt(), l[2].toInt(), l[3].toInt());
	else
		return QRect(-1, -1, 0, 0);
}

void CodeView::init()
{
	m_bracketed.clear();
	m_stylist->setProperties(properties());
	QString css = qs(m_stylist->css());
	setHtml(QString("<!DOCTYPE HTML><html><head><style type=\"text/css\">%1</style></head><body onmousedown=\"procMouseDown(event)\">%2</body></html>").arg(css).arg(qs(m_stylist->toHtml(m_subject))));
	page()->mainFrame()->addToJavaScriptWindowObject("CodeView", this);
	QFile support(":/CodeView/Support.js");
	support.open(QFile::ReadOnly);
	page()->mainFrame()->evaluateJavaScript(support.readAll().data());
	m_dirty.clear();
	m_invalidsToCheck.clear();
	List<Concept*> q;
	q.append(m_subject);
	while (q.size())
		if (Concept* e = q.takeLast())
		{
			m_invalidsToCheck.append(e);
			foreach (Concept* i, e->children())
				q.append(i);
		}
}

void CodeView::relayout(Concept* _e)
{
	if (_e && _e == editConcept())
	{
		// Must handle the edit entity (if there is one) separately since refinedHtml (also using from toHtml) doesn't understand
		// that defineHtml should be redirected to the editDelegate if there is one (and couldn't since it doesn't know which is
		// the active CodeScene).
		QString html = qs(m_stylist->editHtml(_e, this));
		if (m_stylist->property(L"CodeView", L"Preview Brackets").toBool())
			for (int i = 0; i < isBracketed(_e->over()); i++)
				if (m_stylist->property(L"CSS", L"Simple").toBool())
					html = "(" + html + ")";
				else
					html = "<span class=\"symbol\">(</span>" + html + "<span class=\"symbol\">)</span>";
		page()->mainFrame()->evaluateJavaScript(QString("changeEditContent(%1, '%2')").arg((long)_e).arg(html.replace('\'', "\\'")));
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
	if (!m_oldCurrent)
		m_oldCurrent = current();
	if (_e.text() == UpKey && !_e.modifiers())
		page()->mainFrame()->evaluateJavaScript("goUp()");
	else if (_e.text() == DownKey && !_e.modifiers())
		page()->mainFrame()->evaluateJavaScript("goDown()");
	else if (_e.text() == LeftKey && !_e.modifiers())
		page()->mainFrame()->evaluateJavaScript("goPrevious()");
	else if (_e.text() == RightKey && !_e.modifiers())
		page()->mainFrame()->evaluateJavaScript("goNext()");
	else if (_e.text() == PageUpKey && !_e.modifiers())
		page()->mainFrame()->evaluateJavaScript("goOut()");
	else if (_e.text() == PageDownKey && !_e.modifiers())
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
			if (m_routing)
				m_routing->setHtml(qs(lastKeyPress()));
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
