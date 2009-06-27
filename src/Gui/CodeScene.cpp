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

#include <QtSvg>
#include <QtGui>

#include "CommonGraphics.h"
#include "DecorationContext.h"
#include "EditDelegate.h"
#include "CodeScene.h"

namespace Martta
{

QList<CodeScene*> CodeScene::s_allScenes;
	
CodeScene::CodeScene(QWidget* _p):
	QWidget				(_p),
	m_subject			(0),
	m_current			(0),
	m_showDependencyInfo(false),
	m_showChanges		(false),
	m_showOneChange		(true),
	m_leavingEdit		(false),
	m_showHover			(true),
	m_hover				(0),
	m_lastRealCurrent	(0),
	m_editDelegate		(0),
	m_navigated			(false),
	m_strobeCreation	(0),
	m_strobeChild		(0),
	m_strobeFocus		(0),
	m_strobeText		(0),
	m_insert			(false),
	m_insertLock		(false),
	m_lastDefiniteX		(-1.f),
	m_reinterpretCurrentKeyEvent(false),
	m_stylist			(new EntityStylist),
	m_borderOffset		(8.f, 8.f),
	m_ensureCurrentVisible(false)
{
	s_allScenes.append(this);
	if (m_showHover)
		setMouseTracking(true);
}

CodeScene::~CodeScene()
{
	s_allScenes.removeAll(this);
}

void CodeScene::leaving(Entity* _e, InsertionPoint const&)
{
	if (m_subject == _e)
	{
		qDebug() << "Gaa! Subject leaving! Don't know what to do!";
		// TODO: Move to a safe (dormant) state, until we get told of a new subject.
		int i = 9;
		i++;
	}
	
//	qDebug() << "Removing from scene " << _e;

	Entity* e = 0;
	if (m_current == _e)
	{
//		qDebug() << "Trying to calculate next...";
//		_e->debugTree();
		Entity* n = next(_e);
		M_ASSERT(!n || n->parent());
		while (n && n->hasAncestor(_e)) n = next(n);
		Entity* p = previous(_e);
		M_ASSERT(!p || p->parent());
		while (p && p->hasAncestor(_e)) p = previous(p);
		/*{
			qDebug() << "Leaving entity:";
			_e->debugTree();
			if (n) n->debugTree(); else qDebug() << "Nothing next";
			if (p) p->debugTree(); else qDebug() << "Nothing previous";
		}*/
		if (n)
			e = n;
		if (p)
			e = p;
	}
	
	foreach (Entity* i, _e->children())
		leaving(i);
	
	m_pictures.remove(_e);
	m_viewKeys.remove(_e);
	m_bounds.remove(_e);
	m_orders.remove(_e);
	foreach (Entity* i, m_orders.keys())
		m_orders[i].removeAll(_e);
	m_visible.remove(_e);
	m_leftmostChild.remove(_e);
	while (Entity* k = m_leftmostChild.key(_e, 0))
		m_leftmostChild.remove(k);
	m_rightmostChild.remove(_e);
	while (Entity* k = m_rightmostChild.key(_e, 0))
		m_rightmostChild.remove(k);
	m_listCache.remove(_e);
	m_cacheKey.remove(_e);
	
	if (m_current == _e)
		if (Entity* ne = e ? e : nearest(_e))
			setCurrent(ne);
	if (m_hover == _e)
		m_hover = 0;
	if (m_lastRealCurrent == _e)
		m_lastRealCurrent = 0;
}

void CodeScene::killStrobe()
{
	m_strobeChild = 0;
	m_strobeFocus = 0;
	m_strobeCreation = 0;
	m_strobeText = "";
}

void CodeScene::setEditing(Entity* _e)
{
	if (_e == editEntity())
		return;
	
	if (m_editDelegate)
	{
		// If the edit delegate is half-way to destruction, we can allow it to have a null subject.
		M_ASSERT(m_editDelegate->subject() == m_current || m_current->usurpsChild(m_editDelegate->subject()) || !m_editDelegate->subject());
		if (m_visible.contains(m_editDelegate->subject()))
		{
			editDelegate()->tryCommit();
			m_leavingEdit = true;
			m_editDelegate->leavingEditIntact();
			// at this point, m_editDelegate and m_current may no longer be valid.
			m_leavingEdit = false;
		}
		if (m_editDelegate)
		{
			delete m_editDelegate;
			relayoutLater(m_current);
		}
		if (!isInScene(m_current))
			doRefreshLayout();
	}
	
	if (_e)
	{
		if (!isInScene(_e))
			doRefreshLayout();
		if (isFocusable(_e))
			setCurrent(_e);
		else if (_e->isUsurped())
			setCurrent(_e->parent());
		else
			return;
		
		_e->newDelegate(this);
		if (m_editDelegate)
		{
			m_editDelegate->initialised();
			update();
		}
		relayoutLater(current());
	}
}

InsertionPoint CodeScene::nearestBracket(InsertionPoint const& _p) const
{
	int n = UndefinedIndex;
	InsertionPoint ret;
	foreach (InsertionPoint i, m_bracketed)
		if (_p == i)
		{
			n = 0;
			return i;
		}
		else if (int d = _p->hasAncestor(i.entity()))
		{
			if (d < n)
			{
				n = d;
				ret = i;
			}
		}
	return ret;
}

bool CodeScene::event(QEvent* _e)
{
	if (_e->type() == QEvent::KeyPress)
	{
		QKeyEvent* ke = static_cast<QKeyEvent*>(_e);
		if (ke->key() == Qt::Key_Tab)
		{
			keyPressEvent(ke);
			return true;
		}
	}
	return QWidget::event(_e);
}

QString CodeScene::layoutString(Entity* _e)
{
	if (isEditing(_e))
		return editDelegate()->defineLayout(m_viewKeys[_e]);
	return m_stylist->defineLayout(_e, m_viewKeys[_e]);
}

void CodeScene::recacheLayoutList(Entity* _e, QString const& _s)
{
	QString s = _s;
	m_cacheKey[_e] = qHash(s);
	QStringList list;
	if (!s.isEmpty())
	{
		if (_e == m_subject && s.right(2) != ";n" && s.right(3) != ";n;")
			s += ";n";
		s.replace("\\;", QString(QChar(0xff69)));
		s.replace(";", QString(QChar(0xff70)));
		s.replace(QString(QChar(0xff69)), ";");
	
		list = s.split(QString(QChar(0xff70)));
		if (_e->isUsurped())
			list.removeAll("^");
		if (!list.contains("^"))
			list << "^!";
	}
	m_listCache[_e] = list;
}

void CodeScene::paintEvent(QPaintEvent*)
{
	if (!m_current || !m_subject)
	{
		M_ASSERT(!m_subject);
		return;
	}
	
	if (!m_pictures.contains(m_subject) || !isInScene(m_current))
		doRefreshLayout();
	
	setMinimumSize(m_borderOffset.x() * 2.f + m_pictures[m_subject].width(), m_borderOffset.y() * 2.f + m_pictures[m_subject].height());
	
	if (isInScene(m_current))
		m_lastRealCurrent = m_current;
	else
	{
		qDebug() << "paintEvent: setting current to" << m_lastRealCurrent << "(LRC)";
		m_current = m_lastRealCurrent;
	}
	
	if (!m_current)
	{
		qDebug() << "paintEvent: setting current to" << m_subject << "(subject)";
		m_current = m_subject;
	}

	foreach (InsertionPoint i, m_bracketed)
		if (!i.exists() || (i.entity() != m_current && !m_current->hasAncestor(i.entity())))
			m_bracketed.removeAll(i);
	
	QPainter p(this);
	p.translate(m_borderOffset);

	p.setPen(Qt::NoPen);
	foreach (InsertionPoint i, m_bracketed)
	{
		QRectF br = bounds(i.entity());
		QLinearGradient g(br.topLeft(), br.bottomLeft());
		g.setColorAt(0.f, QColor(224, 255, 192, 32));
		g.setColorAt(1.f, QColor(192, 255, 127, 32));
		QLinearGradient go(br.topLeft(), br.bottomLeft());
		go.setColorAt(0.f, QColor(224, 255, 192, 128));
		go.setColorAt(1.f, QColor(192, 255, 127, 128));
		p.setBrush(g);
		drawRoundRect(&p, br);
		p.setBrush(go);
		drawRoundRect(&p, QRectF(br.x() - 2.f, br.y(), br.width() + 4.f, br.height()));
	}
	
	QRectF br = bounds(m_current);
	{
		QLinearGradient g(br.topLeft(), br.bottomLeft());
		g.setColorAt(0.f, QColor(m_editDelegate ? 255 : 192, 224, m_editDelegate ? 192 : 255, 32));
		g.setColorAt(1.f, QColor(m_editDelegate ? 255 : 127, 192, m_editDelegate ? 127 : 255, 32));
		p.setBrush(g);
	}
	p.drawRect(br);
	p.drawRect(QRectF(-m_borderOffset.x(), br.y(), width(), br.height()));

	if (m_showHover)
	{
		if (m_hover && m_hover != m_current)
		{
			QRectF br = bounds(m_hover);
			{
				QLinearGradient g(br.topLeft(), br.bottomLeft());
				g.setColorAt(0.f, QColor(224, 224, 224, 32));
				g.setColorAt(1.f, QColor(160, 160, 160, 32));
				p.setBrush(g);
			}
			p.setPen(QColor(160, 160, 160, 64));
			p.drawRect(br);
		}
		if (m_hover && m_showDependencyInfo)
		{
			p.setBrush(Qt::NoBrush);
			p.setPen(QColor(0, 255, 0, 128));
			foreach (Entity* i, m_hover->dependents())
			{
				if (isInScene(i))
				{
					QRectF br = bounds(i);
					p.drawRect(br);
				}
			}
			p.setPen(QColor(255, 0, 0, 128));
			foreach (Entity* i, m_hover->dependencies())
				if (isInScene(i))
				{
					QRectF br = bounds(i);
					p.drawRect(br);
				}
		}
	}
	
	p.drawPicture(0, 0, m_pictures[m_subject]);
	
	p.setBrush(Qt::NoBrush);
	p.setPen(QColor(0, 0, 0, 64));
	p.drawRect(br);
	p.setPen(QColor(0, 0, 0, 32));
	p.drawRect(br + 1.f);
	p.setPen(QColor(0, 0, 0, 16));
	p.drawRect(br + 2.f);
	p.setPen(QColor(0, 0, 0, 8));
	p.drawRect(br + 3.f);

	if (m_showChanges || m_showOneChange)
		foreach (ChangeEntry i, m_showOneChange ? changes().mid(0, 1) : changes())
			if (isInScene(i.m_subject))
			{
				QColor c;
				switch (i.m_op)
				{
					case EntityChanged: c = QColor(0, 0, 255); break;
					case DependencyAdded: c = QColor(0, 255, 0); break;
					case EntityChildrenInitialised: c = QColor(0, 127, 0); break;
					case DependencyRemoved: c = QColor(255, 0, 0); break;
					case DependencyChanged: c = QColor(0, 255, 255); break;
					case DependencySwitched: c = QColor(255, 255, 0); break;
					case ChildMoved: c = QColor(255, 255, 127); break;
					case ContextIndexChanged: c = QColor(255, 0, 255); break;
				}
				p.setPen(QPen(c, 2.f));
				QRectF br = bounds(i.m_subject) + 1.f;
				p.drawRect(br);
				p.drawLine(br.topLeft(), br.bottomRight());
				p.drawLine(br.topRight(), br.bottomLeft());
				if (i.m_object && isInScene(i.m_object))
				{
					QRectF obr = bounds(i.m_object) + 1.f;
					p.drawLine(br.center(), obr.center());
					p.setPen(QPen(QColor(255, 255, 255, 128), 2.f));
					p.drawRect(obr);
				}
			}
}

Entity* CodeScene::editEntity() const
{
	return m_editDelegate ? m_editDelegate->subject() : 0;
}

void CodeScene::keyPressEvent(QKeyEvent* _e)
{
	if (!m_subject || !current()) return;

	SafePointer<Entity> n = current();
	InsertionPoint currentPoint = n->over();
	
	m_doInsert = false;

	EntityKeyEvent e(*_e, 0, false, false, UndefinedIndex, 0);
	e.setAccepted(false);
	
	if (m_strobeFocus && !_e->text().isEmpty() && _e->text() != " ")
	{
		Entity* originalStrobeChild = m_strobeChild;
		InsertionPoint sCrPoint;
		InsertionPoint sChPoint;
		if (m_strobeCreation)
		{
//			m_strobeCreation->debugTree();
			M_ASSERT(m_strobeChild);
			sCrPoint = m_strobeCreation->over();
			sChPoint = m_strobeChild->over();
			m_strobeCreation->prepareMove(Nowhere);
			m_strobeChild->prepareMove(sCrPoint);
		}
		qDebug() << "strobeText: " << m_strobeText;
		e = EntityKeyEvent(*_e, m_strobeText, &*m_strobeFocus, true, m_strobeFocus->isPlaceholder(), -1, this);
		e.setAccepted(false);
		Entity::keyPressEventStarter(&e);
		if (e.isAccepted())
		{
			// TODO: Move this stuff to the EKE's notifyStrobeCreation... This will avoid having to test the child for originality.
			if (sCrPoint)
				m_strobeCreation->commitMove(sCrPoint);
			// If the child wasn't replaced by something else.
			if (m_strobeChild == originalStrobeChild && sChPoint)	// && c because we only need to move the strobeChild if there was a strobe creation (before, anyways).
			{
				m_strobeChild->commitMove(sChPoint);
				if (m_strobeCreation && m_strobeChild->parent())
				{
					m_strobeChild->parentSwitched(m_strobeCreation);
					m_strobeChild->relayoutLater();
				}
				else if (m_strobeCreation)
					m_strobeChild->parentRemoved(m_strobeCreation);
				else if (m_strobeChild->parent())
					m_strobeChild->parentAdded();
				if (m_strobeChild->parent())	// CHECK!!!
				{
					if (m_strobeCreation)
						m_strobeChild->parent()->childSwitched(m_strobeChild, m_strobeCreation);
					else
						m_strobeChild->parent()->childAdded(m_strobeChild->index());
					m_strobeChild->parent()->resetLayoutCache();
				}
			}
			if (m_strobeCreation)
			{
				m_strobeCreation->oneFootInTheGrave();
				// Other notifications?
				m_strobeCreation->killAndDelete();
			}
			
			if (e.strobeChild())
			{
				m_strobeCreation = e.strobeCreation();
				m_strobeChild = e.strobeChild();	// CRASH Could have been deleted - the case when you do 'i++' in an empty main().
			}
			else
			{
				// Strobe child died - cancel strobe and issue warning (can't strobe further now).
				killStrobe();
				qCritical() << "ERROR: Strober killed strobe child so cannot continue strobing.";
			}
		}
		else if (sCrPoint && sChPoint)
		{
			m_strobeChild->prepareMove(sChPoint);
			m_strobeCreation->prepareMove(sCrPoint);
		}	
	}

	if (!e.isAccepted())
	{
		// rejig for single key press.
		e = EntityKeyEvent(*_e, n, true, n->isPlaceholder(), -1, this);
		e.setAccepted(false);
		Entity::keyPressEventStarter(&e);
		if (e.isAccepted())
			killStrobe();
	}
	
	bool allowStrobeInit = true;
	
	// Navigation keys or the strobe kill key.
	if (!e.isAccepted() && (keyPressedAsNavigation(&e) || _e->text() == " "))
	{
		e.accept();
		killStrobe();
		// Navigation keys can never initialise a strobe.
		allowStrobeInit = false;
	}
	
	if (m_reinterpretCurrentKeyEvent) allowStrobeInit = false;
	
	if (e.isAccepted())
	{
		m_insert = m_insertLock || m_doInsert;
		if (m_reinterpretCurrentKeyEvent)
		{
			m_reinterpretCurrentKeyEvent = false;
			keyPressEvent(_e);
		}
	}
	
	if (!_e->text().isEmpty() && _e->text()[0] > 32 && allowStrobeInit)
	{
		// Add to m_strobe.
		if (!m_strobeFocus && currentPoint.exists() && n && n->parent() == currentPoint.entity())
		{
			// Kick off the strobe sequence.
			m_strobeFocus = n;
			m_strobeChild = n;
			m_strobeCreation = currentPoint.entity();
		}
		else if (!m_strobeFocus && n && e.strobeCreation() && e.strobeCreation() != n)
		{
			// Kick off the strobe sequence.
			m_strobeFocus = n;
			m_strobeCreation = e.strobeCreation();
			m_strobeChild = e.strobeChild();
		}
		else if (!m_strobeFocus && n && !e.isAccepted())
		{
			// Kick off the strobe sequence.
			m_strobeFocus = n;
			m_strobeCreation = 0;
			m_strobeChild = 0;
		}
//		if (m_strobeCreation && m_strobeFocus)
//			m_strobeChild = m_strobeCreation->child(m_strobeFocus->ancestorIndex(&*m_strobeCreation));
//		else
//			m_strobeChild = 0;
		if (m_strobeFocus)
			m_strobeText += _e->text()[0];
	}
}

bool CodeScene::keyPressedAsNavigation(EntityKeyEvent const* _e)
{
	if (_e->matches(QKeySequence::MoveToNextChar))
	{
		leaveEdit();
		m_navigated = true;
		navigateAway(current(), Forwards);
		m_lastDefiniteX = bounds(current()).center().x();
	}
	else if (_e->matches(QKeySequence::MoveToPreviousChar))
	{
		leaveEdit();
		m_navigated = true;
		navigateAway(current(), Backwards);
		m_lastDefiniteX = bounds(current()).center().x();
	}
	else if (_e->matches(QKeySequence::MoveToNextLine))
	{
		leaveEdit();
		if (m_lastDefiniteX == -1.f)
			m_lastDefiniteX = bounds(current()).center().x();
		m_navigated = true;
		if (Entity* j = traverse(current(), false, m_lastDefiniteX))
			setCurrent(j);
	}
	else if (_e->matches(QKeySequence::MoveToPreviousLine))
	{
		leaveEdit();
		if (m_lastDefiniteX == -1.f)
			m_lastDefiniteX = bounds(current()).center().x();
		m_navigated = true;
		if (Entity* j = traverse(current(), true, m_lastDefiniteX))
			setCurrent(j);
	}
	else if (_e->key() == Qt::Key_PageUp)
	{
		leaveEdit();
		if (m_navigated)
		{
			m_navigated = false;
			m_pagingRoute.clear();
		}
		Entity const* e = current();
		while (e->parent() && isInScene(e->parent()) && !isFocusable(e->parent()))
		{
			m_pagingRoute << current()->index();
			e = e->parent();
		}
		if (e->parent() && isFocusable(e->parent()))
		{
			m_pagingRoute << current()->index();
			setCurrent(current()->parent());
			m_lastDefiniteX = bounds(current()).center().x();
		}
	}
	else if (_e->key() == Qt::Key_PageDown)
	{
		leaveEdit();
		Entity const* e = current();
		while (e->child(m_pagingRoute.size() ? m_pagingRoute.last() : 0) &&
			   isInScene(e->child(m_pagingRoute.size() ? m_pagingRoute.last() : 0)) &&
			   !isFocusable(e->child(m_pagingRoute.size() ? m_pagingRoute.last() : 0)))
			e = e->child(m_pagingRoute.size() ? m_pagingRoute.takeLast() : 0);
		if (e->child(m_pagingRoute.size() ? m_pagingRoute.last() : 0) &&
			isFocusable(e->child(m_pagingRoute.size() ? m_pagingRoute.last() : 0)))
		{
			setCurrent(e->child(m_pagingRoute.size() ? m_pagingRoute.takeLast() : 0));
			m_lastDefiniteX = bounds(current()).center().x();
		}
	}
	else if (_e->key() == Qt::Key_Insert && m_insert && !m_insertLock)
		m_insertLock = true;
	else if (_e->key() == Qt::Key_Insert && m_insertLock)
		m_insertLock = false;
	else if (_e->key() == Qt::Key_Insert && !m_insert)
		m_doInsert = true;
	else if (_e->key() == Qt::Key_Enter && m_editDelegate)
		setEditing(0);
	else
		return false;
	return true;
}	

Entity* CodeScene::nearest(Entity* _e)
{
	if (_e)
	{
		Entity* i = _e;
		do
		{
			if (isFocusable(i))
				return i;
			i = m_leftmostChild.value(i, i->child(0));
		}
		while (i && i->hasAncestor(_e));
	
		i = _e;
		while (i && !isFocusable(i))
			i = i->parent();
		if (i)
			return i;
	}
	// Fallback
	if (m_orders.contains(m_subject) && m_orders[m_subject].size())
		return m_orders[m_subject].first();
	else
		return m_subject;
}

void CodeScene::silentlySetCurrent(Entity* _e)
{
	if (m_leavingEdit)
	{
//		qDebug() << "silentlySetCurrent: setting current to" << _e;
		m_current = _e;
	}
}

void CodeScene::setCurrent(Entity* _e)
{
	M_ASSERT(_e);

//	qDebug() << "";
//	qDebug() << "setCurrent: want to set current to" << _e;
//	qDebug() << "";
	if (m_current == _e || !m_subject)
		return;

	leaveEdit();
	
	Entity* old = 0;
	if (isInScene(m_current) && isFocusable(m_current))
		m_lastRealCurrent = m_current;
	if (m_current)
		old = m_current;
	
	// A bit bizarre, not necessarily wrong, though.
	M_ASSERT(_e != m_subject);
	M_ASSERT(_e);
	
	if (isInScene(_e) && !isFocusable(_e))
	{
		qDebug() << "In scene and not focusable - definitely wrong.";
		_e->debugTree();
		Entity* ne = nearest(_e);
		if (!isFocusable(ne))
		{
			qDebug() << "!!! ERROR nearest returned a non-focusable entity.";
			ne = nearest(_e);
		}
		_e = ne;
	}
		
/*	qDebug() << "";
	qDebug() << "setCurrent: setting current to" << _e;
	qDebug() << "";
	qDebug() << "";*/
//	_e->debugTree();
	m_current = _e;
	
	
	if (old)
	{
		for (Entity* e = old; e && m_current && !m_current->hasAncestor(e); e = e->parent())
			e->checkForCullingLater();
	}
	currentChanged(current());
	
	if (QScrollArea* sa = qobject_cast<QScrollArea*>(parent()->parent()->parent()))
	{
		if (!m_bounds.contains(m_current))
			doRefreshLayout();
		QRectF br = bounds(m_current);
		br.translate(m_borderOffset);
		sa->ensureVisible(br.center().x(), br.center().y(), br.width() / 2 + 16, br.height() / 2 + 16);
	}
	
	update();
}

void CodeScene::repaint(Entity*)
{
	// TODO: only update specific area required.
	update();
}

bool CodeScene::markDirty(Entity* _e)
{
	resetLayoutCache(_e);
	bool relayoutActuallyNeeded = false;
	for (Entity* i = _e; i; i = i->parent())
	{
		// TODO: There should be a break condition.
		
		// Visible contains i only if i is visible; it isn't necessarily up-to-date.
		if (m_visible.contains(i))
			relayoutActuallyNeeded = true;
		// Pictures contains i whether or not i is hidden, as long as it was once viewed and is (well, was) up-to-date.
		if (m_pictures.contains(i))
			m_pictures.remove(i);
	}
	return relayoutActuallyNeeded;
}

void CodeScene::resetLayoutCache(Entity* _e)
{
	if (m_listCache.contains(_e))
	{
		QString s = layoutString(_e);
		if (qHash(s) != m_cacheKey[_e])
			recacheLayoutList(_e, s);
	}
}

void CodeScene::relayoutLater(Entity* _e)
{
	if (markDirty(_e))
		update();
}

void CodeScene::relayout(Entity* _e)
{
	if (markDirty(_e))
	{
		doRefreshLayout();
		update();
	}
}

void CodeScene::leaveEdit()
{
	setEditing(0);
}

void CodeScene::mouseDoubleClickEvent(QMouseEvent* _e)
{
	if (Entity* e = at(_e->pos()))
		e->activateEvent(this);
}

void CodeScene::mouseMoveEvent(QMouseEvent* _e)
{
	if (m_showHover)
	{
		Entity* e = at(_e->pos());
		if (m_hover != e)
		{
			m_hover = e;
			update();
		}
	}
}

void CodeScene::mousePressEvent(QMouseEvent* _e)
{
	if (Entity* e = at(_e->pos()))
		setCurrent(e);
}

Entity* CodeScene::at(QPointF const& _p) const
{
	if (!m_subject)
		return 0;
	Entity* e = m_subject;
	Entity* r = m_subject;
	QPointF pos = _p - m_borderOffset;
NEXT:
	foreach (Entity* i, e->children())
	{
		if (m_bounds.contains(i) && m_bounds[i].contains(pos))
		{
			e = i;
			pos -= m_bounds[i].topLeft();
			if (isFocusable(i))
				r = i;
			goto NEXT;
		}
	}
	if (r != m_subject)
		return r;
	return 0;
}

void CodeScene::setSubject(Entity* _subject)
{
//	if (m_subject == _subject)
//		return;
	// Memory reuse means two completely different models could have the same address with one following the death of the other.
		
	m_pictures.clear();
	m_viewKeys.clear();
	m_bounds.clear();
	m_orders.clear();
	m_leftmostChild.clear();
	m_rightmostChild.clear();
	m_bracketed.clear();
	m_editDelegate = 0;
	
	// If the model's going down then make sure we reset all our pointers.
	// m_current, at least, needs to be reset with a different subject.
//	if (!m_subject)
	{
		delete m_editDelegate;
		m_editDelegate = 0;
		m_current = 0;
		m_lastRealCurrent = 0;
		m_strobeCreation = 0;
		m_strobeChild = 0;
		m_strobeFocus = 0;
	}
	
	// Set subject
	m_subject = _subject;
	doRefreshLayout();
	if (m_subject)
		navigateOnto(m_subject);
	update();
}

void CodeScene::navigateInto(Entity* _centre)
{
	leaveEdit();
	
	// TODO: Only layout _centre itself.
	if (!isInScene(_centre))
		doRefreshLayout();
	
	qInformation() << "Navigating into " << _centre;
//	_centre->debugTree();
	Entity* n = m_leftmostChild.value(_centre, _centre);
	if (!n)
	{
		qDebug() << "Can't find anything here.";
		n = m_leftmostChild.value(_centre, _centre);
	}
	setCurrent(n ? n : nearest(_centre));
}

void CodeScene::navigateOnto(Entity* _shell)
{
	leaveEdit();
	
	// TODO: Only layout _shell itself.
	if (!isInScene(_shell))
		doRefreshLayout();
	
	Entity* e = _shell;
	while (m_leftmostChild.contains(e))
		e = m_leftmostChild.value(e);
	setCurrent(nearest(e));
}

void CodeScene::navigateToNew(Entity* _e)
{
	leaveEdit();

	if (!isInScene(_e) || !isInScene(_e->parent()))
		doRefreshLayout();
	
	QList<Entity*> o = m_orders[_e->parent()];
	int i = o.indexOf(_e);
	if (i != UndefinedIndex && i < o.count() - 1)
		navigateInto(o[i + 1]);
	else
		setCurrent(nearest(_e->parent()));
}

void CodeScene::navigateAway(Entity* _from, NavigationDirection _d)
{
	leaveEdit();
	
	Entity* n;
	if (_d == Forwards && (n = next(_from)))
		setCurrent(n);
	else if (_d == Backwards && (n = previous(_from)))
		setCurrent(n);
}

QRectF CodeScene::bounds(Entity* _e)
{
	QRectF ret = m_bounds[_e];
	for (Entity* e = _e->parent(); e && m_bounds.contains(e); e = e->parent())
		ret.moveTopLeft(ret.topLeft() + m_bounds[e].topLeft());
	return QRectF(ret.x() - 1.f, ret.y(), ret.width() + 1.f, ret.height());
}

Entity* CodeScene::traverse(Entity* _e, bool _upwards, float _x)
{
	// Step 1: Find a direct ancestor that has siblings of non-overlapping, Y positions.
	float ourBound = _upwards ? bounds(_e).top() : bounds(_e).bottom();
	Entity* e = _e;
	int ci = UndefinedIndex;
	float mb = -1.f;
	for (; e && isInScene(e); ci = e->index(), e = e->parent())
	{
		foreach (Entity* i, e->children())
		{
			if (i->index() != ci && isInScene(i))
			{
				QRectF b = bounds(i);
				if (_upwards ? b.bottom() <= ourBound : (b.top() >= ourBound))
					if (mb == -1.f || (_upwards ? b.bottom() > mb : (b.top() < mb)))
						mb = _upwards ? b.bottom() : b.top();
			}
		}
		if (mb != -1.f) break;
	}
	// e is an ancestor that has at least one sibling completely above/below us ala _upwards.
	// ci is our ancestor's index into e.
	// mb is maximum bottom value that is less than our top.

	// Step 1b: If we don't have such an ancestor, return 0.
	if (mb == -1.f) return 0;

	// Step 2: Search all siblings to determine closest to us in Y axis aside from our direct ancestor.
	float d = -1.f;
	Entity* x;
	foreach (Entity* i, e->children())
	if (i->index() != ci && isInScene(i))
	{
		QRectF b = bounds(i);
		if ((_upwards ? b.bottom() : b.top()) == mb)
		{
			float di = qMin(fabs(b.left() - _x), fabs(b.right() - _x));
			if (d == -1.f || di < d)
			{
				d = di;
				x = i;
			}
		}
	}
	// x is the nearest-ancestor-sibling that has the has the closest (but completely different) Y-axis area.

	// Step 3: Search all (great-)children to determine closest to us in the X axis.
	e = x;
	while (isInScene(e) && e->children().size())
	{
		QRectF cb = bounds(e);
		float mb = -1.f;
		float maxtop = cb.top();
		float minbot = cb.bottom();
		foreach (Entity* i, e->children())
			if (isInScene(i))
			{
				QRectF b = bounds(i);
				maxtop = qMax<float>(maxtop, b.top());
				minbot = qMin<float>(minbot, b.bottom());
				if (mb == -1.f || _upwards ? b.bottom() > mb : (b.top() < mb))
					mb = _upwards ? b.bottom() : b.top();
			}
		bool onlyOneRow = maxtop < minbot;

		float d = -1.f;
		Entity* x = 0;
		if (isFocusable(e) && onlyOneRow)
		{
			float di = (fabs(bounds(e).left() - _x) + fabs(bounds(e).right() - _x));
			if (d == -1.f || di < d)
				d = di;
		}
		foreach (Entity* i, e->children())
			if (isInScene(i))
			{
				QRectF b = bounds(i);
				if ((_upwards ? b.bottom() : b.top()) == mb)
				{
					float di = (fabs(b.left() - _x) + fabs(b.right() - _x));
					if (d == -1.f || di < d)
					{
						d = di;
						x = i;
					}
				}
			}
		if (!x) break;
		e = x;
	}

	return e;
}

static QHash<QString, QSvgRenderer*> s_picCache;
static QHash<QString, QPixmap*> s_imgCache;

class ItemToBe
{
public:
	ItemToBe(QRectF _br): br(_br) {}
	void rejig(float _mH) { br = QRectF(br.x(), br.y() + (_mH - br.height()) / 2.f, br.width(), br.height()); }
	QRectF br;
};

class TextToBe: public ItemToBe
{
public:
	TextToBe(QRectF _br, QString const& _text, QColor _colour, QColor _shadow, QColor _emboss, QFont _font):
	ItemToBe(_br), text(_text), colour(_colour), shadow(_shadow), emboss(_emboss), font(_font) {}
	QString text;
	QColor colour;
	QColor shadow;
	QColor emboss;
	QFont font;
};

class RenToBe: public ItemToBe
{
public:
	RenToBe(QRectF _br, QPixmap* _renderer):
	ItemToBe(_br), renderer(_renderer) {}
	QPixmap* renderer;
};

class PicToBe: public ItemToBe
{
public:
	PicToBe(QRectF _br, QPicture const& _p):
	ItemToBe(_br), picture(_p), boundsFor(0), cs(0) {}
	QPicture picture;
	Entity* boundsFor;
	CodeScene* cs;
};

#define CACHING 0

// Several members in here are references. They may or may not be data that the object "owns".
// It's controlled by refreshLayout() method which relies upon symetrical sets of [/[[/[[[s.
// It's about as unrobust as it comes.
// It's a horrible hack to save a miniscule amount of effort, but there you go.
// This all needs to be rewritten (or at the least tidied up and code-standardified), but there
// are more important things to be doing.
class LayoutFrame
{
public:
	LayoutFrame(QColor& _cC, QColor& _cS, QColor& _cE, QFont& _cF, Entity* _s):
		minimumLineWidth(1.f), 
		minimumLineHeight(0.f),
		leftMargin(0.f),
		topMargin(0.f),
		rightMargin(0.f),
		bottomMargin(0.f),
		nextX(0.f),
		nextY(0.f),
		maxHeight(minimumLineHeight),
		maxWidth(minimumLineWidth),
		curCol(_cC),
		curShadow(_cS),
		curEmboss(_cE),
		curFont(_cF),
		back(0, 0, 0, 0),
		p(&pic),
		subject(_s)
	{
	}
	
	bool interpret(QString e)
	{
		if (e.startsWith("m"))
		{
			QStringList m = e.mid(1).split(",");
			leftMargin = m[0].toInt();
			topMargin = m[1].toInt();
			rightMargin = m[2].toInt();
			bottomMargin = m[3].toInt();
			nextX = leftMargin;
			nextY = topMargin;
		}
		else if (e.startsWith("B"))
		{
			back = colourByName(e.mid(1));
		}
		else if (e == "Hfull")
		{
		}
		else if (e == "(")
		{
			captured << QRectF(nextX, nextY, -1.f, -1.f);
		}
		else if (e == ")")
		{
			for (int i = captured.size() - 1; i >= 0; i--)
				if (captured[i].size() == QSizeF(-1.f, -1.f))
				{
					captured[i] = QRectF(captured[i].x(), captured[i].y(), nextX - captured[i].x(), -1.f);
					break;
				}
		}
		else if (e == "n" || e.startsWith("v") || e == "]" || e == "]]" || e == "]]]")
		{
			// newline.
			maxWidth = qMax(nextX + rightMargin, maxWidth);
			foreach (TextToBe i, textsToBe)
			{
				i.rejig(maxHeight);
				p.setFont(i.font);
				if (i.shadow.alpha() != 0)
				{
					p.setBrush(i.shadow);
					p.setPen(i.shadow);
					p.drawText(QRectF(i.br.topLeft() - QPointF(1.f, 1.f), i.br.size()), Qt::AlignLeft|Qt::AlignTop, i.text);
				}
				if (i.emboss.alpha() != 0)
				{
					p.setBrush(i.emboss);
					p.setPen(i.emboss);
					p.drawText(QRectF(i.br.topLeft() + QPointF(1.f, 1.f), i.br.size()), Qt::AlignLeft|Qt::AlignTop, i.text);
				}
				p.setPen(i.colour);
				p.drawText(i.br, Qt::AlignLeft|Qt::AlignTop, i.text);
			}
			textsToBe.clear();
			foreach (RenToBe i, rensToBe)
			{
				i.rejig(maxHeight);
				p.drawPixmap(i.br.topLeft(), *i.renderer);
			}
			rensToBe.clear();
			foreach (PicToBe i, picsToBe)
			{
				i.rejig(maxHeight);
				p.drawPicture(i.br.topLeft(), i.picture);
				if (i.boundsFor)
					i.cs->setLocalBounds(i.boundsFor, i.br);
			}
			picsToBe.clear();
			nextY += maxHeight;
			for (int i = 0; i < captured.size(); i++)
				if (captured[i].width() != -1.f && captured[i].height() == -1.f)
					captured[i] = QRectF(captured[i].x(), captured[i].y(), captured[i].width(), nextY - captured[i].y());
			nextX = leftMargin;
			maxHeight = minimumLineHeight; // minimum line height
			
			if (e == "v")
				nextY += e.mid(1).toInt();
		}
		else if (e.startsWith("p"))
		{
			if (!s_picCache.contains(e.mid(1)))
			{
				s_picCache[e.mid(1)] = new QSvgRenderer(e.mid(1));
			}
			if (!s_imgCache.contains(e.mid(1)))
			{
				s_imgCache[e.mid(1)] = new QPixmap(s_picCache[e.mid(1)]->defaultSize());
				s_imgCache[e.mid(1)]->fill(QColor(0, 0, 0, 0));
				QPainter p(s_imgCache[e.mid(1)]);
				s_picCache[e.mid(1)]->render(&p, QRectF(QPointF(0.f, 0.f), s_picCache[e.mid(1)]->defaultSize()));
			}
			rensToBe << RenToBe(QRectF(QPointF(nextX, nextY), s_picCache[e.mid(1)]->defaultSize()), s_imgCache[e.mid(1)]);
			nextX += s_picCache[e.mid(1)]->defaultSize().width();
			maxHeight = qMax<float>(s_picCache[e.mid(1)]->defaultSize().height(), maxHeight);
		}
		else if (e.startsWith("'"))
		{
			QString t = e.mid(1, e.length() - 2);
			QSizeF br = textSize(t, curFont);
			textsToBe << TextToBe(QRectF(QPointF(nextX, nextY), br), t, curCol, curShadow, curEmboss, curFont);
			nextX += br.width();
			maxHeight = qMax<float>(br.height(), maxHeight);
		}
		else if (e == "toend")
		{
			nextX = qMax<float>(nextX, maxWidth);
		}
		else if (e.startsWith("M"))
		{
			if (e == "Mi")
				nextX += 2.f;
			else if (e == "Mo")
				nextX += 4.f;
			else
				nextX += e.mid(1).toInt();
		}
		else if (e.startsWith("h"))
			nextX += e.mid(1).toInt();
		else if (e == "i")
		{
			nextX += 16.f;
		}
		else if (e == "-i")
		{
			nextX -= 16.f;
		}
		else if (e.startsWith("y"))
		{
			if (e == "ynormal")
			{
				curCol = QColor(0, 0, 0);
				curShadow = QColor(0, 0, 0, 0);
				curEmboss = QColor(0, 0, 0, 0);
				curFont.setBold(false);
				curFont.setItalic(false);
				curFont.setPixelSize(QFontInfo(QFont()).pixelSize());
			}
			else if (e == "ygrayed")
			{
				curCol = QColor(0, 0, 0, 96);
				curShadow = QColor(0, 0, 0, 0);
				curEmboss = QColor(0, 0, 0, 0);
				curFont.setBold(false);
				curFont.setItalic(false);
				curFont.setPixelSize(QFontInfo(QFont()).pixelSize());
			}
			else if (e == "yminor")
			{
				curCol = QColor(0, 0, 0, 96);
				curShadow = QColor(0, 0, 0, 0);
				curEmboss = QColor(0, 0, 0, 0);
				curFont.setBold(false);
				curFont.setItalic(false);
				curFont.setPixelSize(QFontInfo(QFont()).pixelSize() * 3 / 4);
			}
			else if (e == "ymajor")
			{
				curCol = QColor(0, 0, 0);
				curShadow = QColor(0, 0, 0, 0);
				curEmboss = QColor(0, 0, 0, 0);
				curFont.setBold(true);
				curFont.setItalic(false);
				curFont.setPixelSize(QFontInfo(QFont()).pixelSize());
			}
			else if (e == "ycode")
			{
				curCol = QColor(0, 0, 0, 160);
				curShadow = QColor(0, 0, 0, 0);
				curEmboss = QColor(0, 0, 0, 0);
				curFont.setBold(true);
				curFont.setItalic(false);
				curFont.setPixelSize(QFontInfo(QFont()).pixelSize());
			}
		}
		else if (e.startsWith("fb"))
		{
			curFont.setBold(e != "fb0");
		}
		else if (e.startsWith("fi"))
		{
			curFont.setItalic(e != "fi0");
		}
		else if (e.startsWith("fs"))
		{
			if (e == "fs")
				curFont.setPixelSize(QFontInfo(QFont()).pixelSize());
			else if (e.startsWith("fs+"))
				curFont.setPixelSize(QFontInfo(curFont).pixelSize() + e.mid(3).toInt());
			else if (e.startsWith("fs-"))
				curFont.setPixelSize(QFontInfo(curFont).pixelSize() - e.mid(3).toInt());
			else
				curFont.setPixelSize(e.mid(2).toInt());
		}
		else if (e.startsWith("c"))
		{
			if (e == "c")
				curCol = QColor(0, 0, 0, 255);
			else
				curCol = colourByName(e.mid(1));
		}
		else if (e.startsWith("s"))
		{
			if (e == "s")
				curShadow = QColor(0, 0, 0, 0);
			else
				curShadow = colourByName(e.mid(1));
		}
		else if (e.startsWith("e"))
		{
			if (e == "e")
				curEmboss = QColor(0, 0, 0, 0);
			else
				curEmboss = colourByName(e.mid(1));
		}
		else
			return false;
		return e != "]" && e != "]]" && e != "]]]";
	}
	
	QList<QRectF> captured;
	QList<Entity*> order;
	float minimumLineWidth;
	float minimumLineHeight;
	float leftMargin;
	float topMargin;
	float rightMargin;
	float bottomMargin;
	float nextX;
	float nextY;
	float maxHeight;
	float maxWidth;
	QColor& curCol;
	QColor& curShadow;
	QColor& curEmboss;
	QFont& curFont;
	QColor back;
	QList<TextToBe> textsToBe;
	QList<PicToBe> picsToBe;
	QList<RenToBe> rensToBe;
	QPicture pic;
	QPainter p;
	Entity* subject;
};

void CodeScene::doRefreshLayout()
{
//	TIME_FUNCTION;
	if (!m_subject)
		return;
	
	QList<Entity*> oldBounds = m_bounds.keys();
	QStringList list = layoutList(m_subject);
	
	// Actual program.
	QStack<LayoutFrame*> frames;
	LayoutFrame* f = new LayoutFrame(*new QColor(0, 0, 0, 255), *new QColor(0, 0, 0, 0), *new QColor(0, 0, 0, 0), *new QFont, m_subject);

	QList<float> times;
	for (int i = 0; i < list.size(); i++)
	{
		QTime timer;
		timer.start();
		QString e = list[i];
		if (f->interpret(e))
		{
		}
		else if (e == "[")
		{
			frames.push(f);
			f = new LayoutFrame(f->curCol, f->curShadow, f->curEmboss, f->curFont, f->subject);
		}
		else if (e == "[[" || e == "[[[")
		{
			frames.push(f);
			/// Isolate by giving fake references.
			f = new LayoutFrame(*new QColor(0, 0, 0), *new QColor(0, 0, 0, 0), *new QColor(0, 0, 0, 0), *new QFont, f->subject);
		}
		else if ((e == "]" || e == "]]" || e == "]]]") && frames.size())
		{
			if (true)
			{
				f->p.end();
				QRect br(0, 0, f->maxWidth, f->nextY + f->bottomMargin);
			
				// Construct the final picture (add background, decoration and foreground layers)
				QPicture fullPic;
				if (e == "]]]")
					fullPic = f->pic;
				else
				{
					QPainter p(&fullPic);
					if (f->back.alpha())
					{
						p.setPen(Qt::NoPen);
						p.setBrush(f->back);
						p.drawRect(br);
					}
					f->subject->decorate(DecorationContext(&p, br, f->captured));
					p.drawPicture(0, 0, f->pic);
				}
			
				fullPic.setBoundingRect(br);
				frames.top()->picsToBe += PicToBe(QRectF(QPointF(frames.top()->nextX, frames.top()->nextY), br.size()), fullPic);
				frames.top()->nextX += br.width();
				frames.top()->maxHeight = qMax<float>(frames.top()->maxHeight, br.height());
				frames.top()->order << f->order;
				m_orders[f->subject] = f->order;
			}
			
			if (e != "]")
			{
				// If we're in an isolated block, kill the old col/font/shadow.
				delete &f->curCol;
				delete &f->curShadow;
				delete &f->curEmboss;
				delete &f->curFont;
			}
			delete f;
			f = frames.pop();
		}
		else if (e.startsWith("!cache") && f->subject->child(e.mid(6).toInt()))
		{
			m_pictures[f->subject->child(e.mid(6).toInt())] = f->picsToBe.last().picture;
			f->picsToBe.last().boundsFor = f->subject->child(e.mid(6).toInt());
			f->picsToBe.last().cs = this;
		}
		else if (e.startsWith("include") && f->subject->child(e.mid(7).toInt()))
		{
			f->subject = f->subject->child(e.mid(7).toInt());
			int k = 1;
			foreach (QString j, layoutList(f->subject))
				list.insert(i + k++, j);
		}
		else if (e == "^")
		{
			f->order << f->subject;
			if (!m_leftmostChild.contains(f->subject->parent()))
				m_leftmostChild[f->subject->parent()] = f->subject;
			m_rightmostChild[f->subject->parent()] = f->subject;
		}
		else if (e == "^!")
		{
			if (!m_leftmostChild.contains(f->subject->parent()))
				m_leftmostChild[f->subject->parent()] = m_leftmostChild[f->subject];
			m_rightmostChild[f->subject->parent()] = m_rightmostChild[f->subject];
		}
		else if (QRegExp("-?[0-9]+").exactMatch(e) && f->subject->child(e.toInt()) && m_pictures.contains(f->subject->child(e.toInt())))
		{
			Entity* c = f->subject->child(e.toInt());
			m_visible.insert(c);
			QPicture const& p = m_pictures[c];
			f->picsToBe += PicToBe(QRectF(QPointF(f->nextX, f->nextY), p.boundingRect().size()), p);
			f->picsToBe.last().boundsFor = c;
			f->picsToBe.last().cs = this;
			f->order << m_orders[c];
			f->nextX += p.boundingRect().width();
			f->maxHeight = qMax<float>(f->maxHeight, p.boundingRect().height());
		}
		else if (QRegExp("!-?[0-9]+").exactMatch(e) && f->subject->child(e.mid(1).toInt()) || QRegExp("-?[0-9]+").exactMatch(e) && f->subject->child(e.toInt()))
		{
			QString s = e.startsWith("!") ? e.mid(1) : e;
			Entity* c = f->subject->child(s.toInt());
			if (layoutList(c).size())
			{
				// Remove all of child's entities.
				m_visible.subtract(QSet<Entity*>::fromList(c->children()));
				list.insert(i + 1, e.startsWith("!") ? "[" : "[[");
				list.insert(i + 2, "include" + s);
				list.insert(i + 3, e.startsWith("!") ? "]" : "]]");
				list.insert(i + 4, "!cache" + s);
				m_visible.insert(c);
			}
		}
		times << timer.elapsed();
	}
	f->p.end();
	QRect br(0, 0, f->maxWidth, f->nextY + f->bottomMargin);
	f->pic.setBoundingRect(br);
	m_visible.insert(m_subject);
	m_bounds[m_subject] = br;
	m_pictures[m_subject] = f->pic;
	m_orders[m_subject] = f->order;
	
	delete &f->curCol;
	delete &f->curShadow;
	delete &f->curEmboss;
	delete &f->curFont;
	delete f;
	M_ASSERT(frames.isEmpty());

//	qDebug() << list;
//	static int count = 0;
//	qDebug() << count++ << list.size() << "commands.";
//	for (int i = 0; i < list.size(); i++)
//		qDebug() << list[i] << times[i];
}

}
