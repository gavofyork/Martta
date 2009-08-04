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
#include "CodeViewWK.h"

namespace Martta
{

CodeViewWK::CodeViewWK(QWidget* _parent):
	QWebView					(_parent),
	m_subject					(0),
	m_stylist					(new Stylist),
	m_silent					(false)
{
	init();
}

CodeViewWK::~CodeViewWK()
{
	delete m_stylist;
}

void CodeViewWK::setSubject(Entity* _s)
{
	m_subject = _s;
	init();
}

void CodeViewWK::setStylist(Stylist* _s)
{
	delete m_stylist;
	m_stylist = _s;
	init();
}

Entity* CodeViewWK::current() const
{
	return (Entity*)(page()->mainFrame()->evaluateJavaScript("g_currentIterator.referenceNode.parentNode.id").toInt());
}

void CodeViewWK::setCurrent(Entity const* _s)
{
	if (!m_silent)
	{
		refresh();
	}
	page()->mainFrame()->evaluateJavaScript(QString("setCurrentById('%1')").arg((int)_s));
}

void CodeViewWK::onCurrentAboutToChange()
{
	if (!m_silent)
		currentAboutToChange();
}

void CodeViewWK::onCurrentChanged(QString const& _oldId)
{
	if (!m_silent)
	{
		Entity* e = current();
		CodeScene::currentChanged(e, (Entity*)(_oldId.toInt()));
		emit currentChanged(e);
		update();
	}
}

void CodeViewWK::navigateAway(Entity* _from, NavigationDirection _d)
{
	refresh();
	page()->mainFrame()->evaluateJavaScript(QString("setCurrentById('%1'); go%2()").arg((int)_from).arg(_d == Forwards ? "Next" : "Previous"));
}

bool CodeViewWK::isFocusable(Entity const* _e) const
{
	return page()->mainFrame()->evaluateJavaScript(QString("thisNode(document.getElementById('%1')) != null").arg((int)_e)).toBool();
}

bool CodeViewWK::isInScene(Entity const* _e) const
{
	return page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1') != null").arg((int)_e)).toBool();
}

void CodeViewWK::refresh()
{
	if (WebStylistRegistrar::get()->hasChanged())
		init();
	else
		// If we're dirty then update the HTML.
		foreach (Entity* e, m_dirty)
			if (e)
			{
				QString html = qs(e->defineHtml());
				if (html.startsWith("<span id=\"this\">") && html.endsWith("</span>") && html.lastIndexOf("<span", 1) == 0)
					page()->mainFrame()->evaluateJavaScript(QString("thisNode(document.getElementById('%1')).innerHTML = '%2'").arg((int)e).arg(html.mid(16, html.length() - 23).replace('\'', "\\'")));
				else
				{
					Entity* cur = current();
					page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').innerHTML = '%2'").arg((int)e).arg(html.replace('\'', "\\'")));
					silentlySetCurrent(cur);
				}
			}
	m_dirty.clear();
	qDebug() << page()->mainFrame()->evaluateJavaScript("document.body.innerHTML").toString();
}

void CodeViewWK::paintEvent(QPaintEvent* _ev)
{
	refresh();
	QWebView::paintEvent(_ev);
	qDebug() << page()->mainFrame()->evaluateJavaScript("g_currentIterator.referenceNode.parentNode.id").toString();
	if (current())
	{
		QPainter p(this);
		QRect b = bounds(current());
		p.setPen(QColor().black());
		p.drawRect(b);
	}
}

QRect CodeViewWK::bounds(Entity const* _e) const
{
	return QRect(	page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').getBoundingClientRect().left").arg((int)_e)).toInt(),
					page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').getBoundingClientRect().top").arg((int)_e)).toInt(),
					page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').getBoundingClientRect().width").arg((int)_e)).toInt(),
					page()->mainFrame()->evaluateJavaScript(QString("document.getElementById('%1').getBoundingClientRect().height").arg((int)_e)).toInt());
}

void CodeViewWK::init()
{
	QString css = qs(WebStylistRegistrar::get()->css());
	qDebug() << css;
	setHtml(QString("<!DOCTYPE HTML><html><head><style type=\"text/css\">%1</style></head><body onmousedown=\"procMouseDown(event)\">%2</body></html>").arg(css).arg(qs(m_subject ? toHtml(m_subject) : String::null)));
	page()->mainFrame()->addToJavaScriptWindowObject("CodeView", this);
	page()->mainFrame()->evaluateJavaScript(
		"function onlyThese(node) { if (node.id == 'this') return NodeFilter.FILTER_ACCEPT; return NodeFilter.FILTER_SKIP; }"
		"var g_currentIterator = document.createNodeIterator(document, NodeFilter.SHOW_ELEMENT, onlyThese, false);"
		"function thisNode(_e)"
		"{"
		"	var children = _e.childNodes;"
		"	for (var i = 0; i < children.length; i++)"
		"		if (children[i].id == 'this')"
		"			return children[i];"
		"	return null;"
		"}"
		"function goPrevious()"
		"{"
		"	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)"
		"		return;"
		"	var old = g_currentIterator.referenceNode;"
		"	var oldId = old.parentNode.id;"
		"	CodeView.onCurrentAboutToChange();"
		"	g_currentIterator.previousNode();"
		"	if (g_currentIterator.referenceNode == old)"
		"		g_currentIterator.previousNode();"
		"	CodeView.onCurrentChanged(oldId);"
		"}"
		"function goNext()"
		"{"
		"	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)"
		"		return;"
		"	var old = g_currentIterator.referenceNode;"
		"	var oldId = old.parentNode.id;"
		"	CodeView.onCurrentAboutToChange();"
		"	g_currentIterator.nextNode();"
		"	if (g_currentIterator.referenceNode == old)"
		"		g_currentIterator.nextNode();"
		"	CodeView.onCurrentChanged(oldId);"
		"}"
		"function goUp()"
		"{"
		"	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)"
		"		return;"
		"	var otop = g_currentIterator.referenceNode.parentNode.getBoundingClientRect().top;"
		"	var oldId = g_currentIterator.referenceNode.parentNode.id;"
		"	CodeView.onCurrentAboutToChange();"
		"	while (g_currentIterator.referenceNode.parentNode.getBoundingClientRect().bottom > otop + 1)"
		"		if (g_currentIterator.previousNode() == null)"
		"			break;"
		"	CodeView.onCurrentChanged(oldId);"
		"}"
		"function goDown()"
		"{"
		"	if (!g_currentIterator.referenceNode || !g_currentIterator.referenceNode.parentNode)"
		"		return;"
		"	var obottom = g_currentIterator.referenceNode.parentNode.getBoundingClientRect().bottom;"
		"	var oldId = g_currentIterator.referenceNode.parentNode.id;"
		"	CodeView.onCurrentAboutToChange();"
		"	while (g_currentIterator.referenceNode.parentNode.getBoundingClientRect().top < obottom - 1)"
		"		if (g_currentIterator.nextNode() == null)"
		"			break;"
		"	CodeView.onCurrentChanged(oldId);"
		"}"
		"function setCurrent(_e)"
		"{"
		"	var t = _e;"
		"	while (t)"
		"	{"
		"		if (thisNode(t) != null)"
		"			break;"
		"		t = t.parentNode;"
		"	}"
		"	if (!t)"
		"		return;"
		"	var oldId = g_currentIterator.referenceNode && g_currentIterator.referenceNode.parentNode ? g_currentIterator.referenceNode.parentNode.id : '';"
		"	CodeView.onCurrentAboutToChange();"
		"	g_currentIterator = document.createNodeIterator(document, NodeFilter.SHOW_ELEMENT, onlyThese, false);"
		"	while (g_currentIterator.nextNode() != null)"
		"		if (g_currentIterator.referenceNode.parentNode == t)"
		"		{"
		"			CodeView.onCurrentChanged(oldId);"
		"			return;"
		"		}"
		"	alert('ODD');"
		"}"
		"function setCurrentById(_id)"
		"{"
		"	setCurrent(document.getElementById(_id));"
		"}"
		"function procMouseDown(event)"
		"{"
		"	setCurrent(event.target);"
		"}"
		"g_currentIterator.nextNode();");
	m_dirty.clear();
}

void CodeViewWK::markDirty(Entity* _e)
{
	m_dirty.append(_e);
	update();
}

bool CodeViewWK::keyPressedAsNavigation(KeyEvent const& _e)
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

bool CodeViewWK::event(QEvent* _e)
{
	if (_e->type() == QEvent::KeyPress && subject() && current())
	{
		QKeyEvent* ke = static_cast<QKeyEvent*>(_e);
		wchar_t const* k = translateKey(ke->key());
		KeyEvent e(k ? String(k) : String(ke->text().toLatin1().data()), translateMods(ke->modifiers()));
		keyPressHandler(e);
		if (e.isAccepted())
		{
			_e->accept();
			return true;
		}
	}
	return QWebView::event(_e);
}

}
