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

#include <QtXml>

#include "BasicRoot.h"	// Might be able to get rid of this later.

#include "CommonGraphics.h"
#include "DecorationContext.h"
#include "CodeScene.h"
#include "TypeDefinition.h"
#include "EditDelegate.h"
#include "Entity.h"

#include <msSupport.h>
using namespace MarttaSupport;

namespace Martta
{

int s_news = 0;
int s_deletes = 0;
	
MARTTA_PLACEHOLDER_CPP(Entity);

QList<ChangeEntry> s_changes;
void change(Entity* _s, ChangeOperation _op, Entity* _o)
{
	static const QStringList ops = QStringList() << "EntityChanged" << "ChildrenInitialised" << "DependencyAdded" << "DependencyRemoved" << "DependencyChanged" << "DependencySwitched" << "ChildMoved" << "ContextIndexChanged";
	s_changes << ChangeEntry(_s, _op, _o);
}

QString Entity::indexName(int _i) const
{
	if (_i == UndefinedIndex)
		return "[Undefined]";
	if (_i >= 0)
		return QString::number(_i);
	if (_i < virtualEndOfNamed())
		return QString((char)('A' + _i - INT_MIN));
	return AuxilliaryRegistrar::get()->nameOfArbitrary(_i);
}

QList<int> Entity::knownNames() const
{
	QList<int> ret = AuxilliaryRegistrar::get()->names();
	for (int i = INT_MIN; i < virtualEndOfNamed(); ++i)
		ret << i;
	return ret;
}

Entity::~Entity()
{
	M_ASSERT(!m_parent);
	foreach (Entity* i, children())
		i->killAndDelete();
	M_ASSERT(!m_cardinalChildren.size());
	M_ASSERT(!m_namedChildren.size());
}

// Model structure management.
bool Entity::hasAncestor(Kind _k) const
{
	for (Entity* e = this ? parent() : 0; e; e = e->parent())
		if (e->isKind(_k))
			return true;
	return false;
}
Entity* Entity::ancestor(Kind _k) const
{
	for (Entity* e = this ? parent() : 0; e; e = e->parent())
		if (e->isKind(_k))
			return e;
	return 0;
}
int Entity::ancestorIndex(Kind _k) const
{
	int ret = index();
	for (Entity* e = this ? parent() : 0; e; ret = e->index(), e = e->parent())
		if (e->isKind(_k))
			return ret;
	return UndefinedIndex;
}
bool Entity::hasSelfAncestor(Kind _k) const
{
	for (Entity* e = const_cast<Entity*>(this); e; e = e->parent())
		if (e->isKind(_k))
			return true;
	return false;
}
Entity* Entity::selfAncestor(Kind _k) const
{
	for (Entity const* e = this; e; e = e->parent())
		if (e->isKind(_k))
			return const_cast<Entity*>(e);
	return 0;
}
bool Entity::hasSelfAncestor(Entity const* _a) const
{
	for (Entity const* i = this; i; i = i->parent())
		if (i == _a)
			return true;
	return false;
}
int Entity::ancestorIndex(Entity const* _a) const
{
	for (Entity const* i = this; i; i = i->parent())
		if (i->parent() == _a)
			return i->index();
	return UndefinedIndex;
}

// Validity/status checking
bool Entity::isValidName(QString const& _n)
{
	return QRegExp("[a-zA-Z][a-zA-Z0-9_]*").exactMatch(_n);
}
bool Entity::isNecessary() const
{
	if (!parent())
		return false;
	if (m_index >= 0)
	{
		if (parent()->cardinalChildCount() <= parent()->minRequired(Cardinals))
			return true;
		for (int i = index() + 1; i < parent()->cardinalChildCount(); i++)
			if (!parent()->child(i)->isKind(parent()->allowedKinds(i - 1)))
				return true;
	}
	else
		if (parent()->childCountAt(m_index) - 1 < parent()->minRequired(m_index))
			return true;
	return false;
}
bool Entity::isComplete() const
{
	QList<Entity*> e = cardinalChildren();
	if (e.size() < minRequired(Cardinals))
		return false;
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		if (childCountAt(i) < minRequired(i))
			return false;
	foreach (int i, AuxilliaryRegistrar::get()->names())
		if (childCountAt(i) < minRequired(i))
			return false;
	foreach (Entity* i, children())
		if (!i->isAllowed())
			return false;
	return true;
}
bool Entity::isValid() const
{
	if (!isInModel())
		return true;
	
	if (!isAllowed())
	{
		qInformation() << this << "invalid because it's not allowed here by parent." << parent();
		return false;
	}
	if (!isInValidState())
	{
		qInformation() << this << "invalid because it's in an invalid entity state.";
		return false;
	}
	if (parent())
		if (!parent()->isChildInValidState(index()))
		{
			qInformation() << this << "invalid because its context considers it in an invalid state.";
			return false;
		}
	return true;
}
bool Entity::isSuperfluous() const
{
	return isPlaceholder() && !isNecessary() || !isAllowed();
}
bool Entity::isAllowed(int _i, Kind _o) const
{
	if (_o.isKind(deniedKinds(_i)))
		return false;
	if (!_o.isKind(allowedKinds(_i)))
		return false;
	return true;
}

// Culling
void Entity::checkForCullingLater()
{
	if (rootEntity())
		rootEntity()->checkCull(this);
}
bool Entity::cull()
{
	Entity* c = parent();
	if (c && !isCurrentOrAncestor() && isSuperfluous())
	{
		deleteAndRefill();
		c->relayoutLater();
		return true;
	}
	return false;
}

// These do *not* send notifications. Be careful when using.
void Entity::kill(Entity* _substitute)
{
//	if (isInModel())
//		qDebug() << "Killing" << this;
	if (_substitute)
		rewirePointer(_substitute);
	silentRemove();
	foreach (Entity* i, m_dependencies)
		removeDependency(i);
	clearDependents();
}
void Entity::clearEntities()
{
	QList<Entity*> tbd;
	while (m_cardinalChildren.size())
	{
		tbd << m_cardinalChildren.last();
		m_cardinalChildren.last()->kill();
	}
	while (m_namedChildren.size())
	{
		tbd << m_namedChildren.values().last();
		m_namedChildren.values().last()->kill();
	}
	while (tbd.size())
		delete tbd.takeLast();
}

// Debug
void Entity::debugTree() const
{
	QList<Entity const*> ancestors;
	for (Entity const* i = parent(); i; i = i->parent())
		ancestors.push_front(i);
	QString indent = "";
	foreach (Entity const* i, ancestors)
	{
		qDebug(qPrintable(indent + i->kind().name() + " (%x)"), i);
		indent += "    ";
	}
	debugTree(indent);
}
void Entity::debugTree(QString const& _i) const
{
	qDebug(qPrintable(_i + kind().name() + " (%x)"), this);
	for (QHash<int, Entity*>::ConstIterator i = m_namedChildren.begin(); i != m_namedChildren.end(); ++i)
		if (i.key() < virtualEndOfNamed())
			i.value()->debugTree(_i + "|   [" + QString::number(i.key() - INT_MIN) + "] ");
		else
			i.value()->debugTree(_i + "|   [" + AuxilliaryRegistrar::get()->nameOfArbitrary(i.key()) + "] ");
	foreach (Entity* i, m_cardinalChildren)
		i->debugTree(_i + "|   ");
}

// I/O
void Entity::exportDom(QDomElement& _element) const
{
	foreach (Entity* e, children())
	{
		QDomElement n = _element.ownerDocument().createElement("entity");
		n.setAttribute("kind", e->kind().name());
		if (e->index() < 0)
		{	if (e->index() >= virtualEndOfNamed())	// proper name - store as name
				n.setAttribute("childname", AuxilliaryRegistrar::get()->nameOfArbitrary(e->index()));
			else
				n.setAttribute("contextindex", e->index());
		}
		e->exportDom(n);
		_element.appendChild(n);
	}
}
void Entity::importDom(QDomElement const& _element)
{
	for (QDomNode i = _element.firstChild(); !i.isNull(); i = i.nextSibling())
		if (i.isElement() && i.toElement().tagName() == "entity")
		{
			Entity* e = spawn(i.toElement().attribute("kind"));
			if (i.toElement().hasAttribute("contextindex"))
				e->silentMove(middle(i.toElement().attribute("contextindex").toInt()));
			else if (i.toElement().hasAttribute("childname"))
				e->silentMove(middle(AuxilliaryRegistrar::get()->arbitraryOfName(i.toElement().attribute("childname"))));
			else
				e->silentMove(back());
			if (e)
				e->importDom(i.toElement());
			else
				qCritical() << "Unknown element of kind" << i.toElement().attribute("kind");
		}
}

// Navigation in CodeScene(s)
bool Entity::isCurrent() const
{
	foreach (CodeScene* i, CodeScene::all())
		if (isCurrent(i))
			return true;
	return false;
}
bool Entity::isCurrent(CodeScene* _s) const
{
	return _s->current() == this;
}
bool Entity::isCurrentOrAncestor() const
{
	foreach (CodeScene* i, CodeScene::all())
		if (isCurrentOrAncestor(i))
			return true;
	return false;
}
bool Entity::isCurrentOrAncestor(CodeScene* _s) const
{
	return _s->current()->hasSelfAncestor(this);
}
void Entity::setCurrent()
{
	foreach (CodeScene* i, CodeScene::all())
		i->setCurrent(this);
}
void Entity::navigateInto(CodeScene* _s)
{
	_s->navigateInto(this);
}
void Entity::navigateToNew(CodeScene* _s)
{
	_s->navigateToNew(this);
}
void Entity::navigateOnto(CodeScene* _s)
{
	_s->navigateOnto(this);
}
void Entity::dropCursor()
{
	QList<Entity*> s = children();
	while (s.size())
	{
		Entity* e = s.takeFirst();
		if (e->isPlaceholder())
		{
			e->setCurrent();
			return;
		}
		else
			s = e->children() + s;
	}
	setCurrent();
}

// Editting in CodeScene(s)
bool Entity::isEditing() const
{
	foreach (CodeScene* i, CodeScene::all())
		if (isEditing(i))
			return true;
	return false;
}
bool Entity::isEditing(CodeScene* _s) const
{
	return _s->editEntity() == this;
}
void Entity::setEditing(CodeScene* _s)
{
	_s->setEditing(this);
}
void Entity::clearEditing()
{
	foreach (CodeScene* i, CodeScene::all())
		i->setEditing(0);
}
EditDelegateFace* Entity::editDelegate(CodeScene* _s)
{
	return _s->editDelegate();
}

// Drawing
void Entity::resetLayoutCache()
{
	if (isInModel())
		foreach (CodeScene* i, CodeScene::all())
			i->resetLayoutCache(this);
}
void Entity::relayout()
{
	foreach (CodeScene* i, CodeScene::all())
		relayout(i);
}
void Entity::relayout(CodeScene* _s)
{
	_s->relayout(this);
}
void Entity::relayoutLater()
{
	if (isInModel())
		foreach (CodeScene* i, CodeScene::all())
			relayoutLater(i);
}
void Entity::relayoutLater(CodeScene* _s)
{
	_s->relayoutLater(this);
}
void Entity::repaint(CodeScene* _s)
{
	if (_s->isInScene(this))
		_s->repaint(this);
	else if (parent())
		parent()->repaint(_s);
}
void Entity::repaint()
{
	foreach (CodeScene* i, CodeScene::all())
		repaint(i);
}
QString Entity::defineLayout(ViewKeys&) const
{
	return "^;ycode;'[]'";
}
void Entity::decorate(DecorationContext const& _c) const
{
	if (!isValid())
	{
		_c->setPen(QPen(QColor(255, 128, 128), 2, Qt::DotLine, Qt::RoundCap));
		_c->drawLine(QPointF(_c().left(), _c().bottom() - 2.f), QPointF(_c().right() - 2.f, _c().bottom() - 2.f));
	}
}

// Keypress/UI event handlers.
void Entity::activateEvent(CodeScene* _s)
{
	if (!activated(_s) && parent())
		parent()->activateEvent(_s);
}
bool Entity::activated(CodeScene* _s)
{
	if (onActivated(_s))
		return true;
	else if (Entity* e = isExpander())
	{
		_s->viewKeys(this)["expanded"] = !(_s->viewKeys(this)["expanded"].toBool());
		relayout(_s);
		
		if (_s->viewKeys(this)["expanded"].toBool())
			e->setCurrent();
		else
			setCurrent();
		return true;
	}
	else
	{
		setEditing(_s);
		return isEditing(_s);
	}
}
void Entity::keyPressEventStarter(EntityKeyEvent* _e, bool _abortive)
{
	Entity* f = _e->focus();
	foreach (Entity* e, f->children())
		if (f->usurpsChild(e))
		{
			_e->setFocus(e);
			keyPressEventStarter(_e, true);
			_e->setFocus(f);
			if (_e->isAccepted())
				return;
		}
	
	if (_e->focus()->isEditing(_e->codeScene()) && _e->codeScene()->editDelegate() && _e->codeScene()->editDelegate()->keyPressed(_e))
	{
		_e->accept();
		if (_e->codeScene()->editDelegate())
		{
			_e->codeScene()->editDelegate()->lazyCommit();
			_e->codeScene()->relayout(_e->focus());
		}
		return;
	}
	
	if (_e->text().isEmpty())
		return;
	
	SafePointer<Entity> fe = _e->focus();
	_e->codeScene()->setEditing(0);

	while (fe)
	{
		if (fe && fe->keyPressed(_e) || fe && fe->attemptInsert(_e))
		{
			_e->accept();
			return;
		}
	
		if (fe && fe->parent())
		{
			_e->setIsFocused(false);
			_e->setFocalIndex(fe->index());
			fe = fe->parent();
		}
		else
			fe = 0;
		
		if (_abortive)
			return;
	}
}
void Entity::keyPressEvent(EntityKeyEvent* _e)
{
	if (keyPressed(_e) || attemptInsert(_e))
	{
		_e->accept();
		return;
	}
	
	if (parent())
	{
		_e->setIsFocused(false);
		_e->setFocalIndex(index());
		parent()->keyPressEvent(_e);
	}
}
bool Entity::keyPressed(EntityKeyEvent const* _e)
{
	InsertionPoint p = over();
	if (_e->codeScene()->isCurrent(this) && (_e->key() == Qt::Key_Delete && _e->modifiers() == Qt::ShiftModifier || _e->key() == Qt::Key_Backspace && isEditing(_e->codeScene())))
	{
//		p.parent()->debugTree();
//		qDebug() << p.index();
//		debugTree();
		deleteAndRefill(0, false);	// NOTE: Was true; changed to false to avoid erroneous currents being set. May need a rethink.
//		p.parent()->debugTree();
//		qDebug() << p.index();
		if (p.exists())
			_e->codeScene()->setCurrent(p.entity());
	}
	else if (_e->codeScene()->isCurrent(this) && _e->key() == Qt::Key_Delete)
	{
//		p.parent()->debugTree();
//		qDebug() << p.index();
//		debugTree();
		if (nonPlaceholderCount() == 1 && isAllowed(nonPlaceholder(0)->kind()))
			deleteAndRefill(nonPlaceholder(0), false);	// SEE ABOVE.
		else
			deleteAndRefill(0, false);	// SEE ABOVE.
//		p.parent()->debugTree();
//		qDebug() << p.index();
		if (p.exists())
			_e->codeScene()->setCurrent(p.entity());
	}
	else if (_e->codeScene()->isCurrent(this) && (_e->key() == Qt::Key_Escape) && isEditing(_e->codeScene()))
		_e->codeScene()->setEditing(0);
	else if (_e->codeScene()->isCurrent(this) && _e->key() == Qt::Key_Tab && !isEditing(_e->codeScene()))
		_e->codeScene()->setEditing(this);
	else if (_e->key() == Qt::Key_Tab)
		activated(_e->codeScene());
	else if (_e->text() == "{" && !_e->codeScene()->viewKeys(this)["expanded"].toBool() && isExpander())
	{
		_e->codeScene()->viewKeys(this)["expanded"] = true;
		relayout(_e->codeScene());
		isExpander()->setCurrent();
	}
	else if (_e->text() == "}" && _e->codeScene()->viewKeys(this)["expanded"].toBool() && isExpander())
	{
		_e->codeScene()->viewKeys(this)["expanded"] = false;
		relayout(_e->codeScene());
		setCurrent();
	}
	else
		return false;
	return true;
}
bool Entity::attemptInsert(EntityKeyEvent const* _e)
{
	if (parent())
		foreach (Kind i, parent()->allowedKinds(index()))
			if (AuxilliaryRegistrar::get()->auxilliary(i.name())->dispatchKeyPress(over(), _e))
				return true;
	return false;
}
bool Entity::attemptAppend(EntityKeyEvent const* _e)
{
	foreach (Kind i, allowedKinds(cardinalChildCount()))
		if (AuxilliaryRegistrar::get()->auxilliary(i.name())->dispatchKeyPress(back(), _e))
			return true;
	return false;
}

// Context/position changing.
void Entity::prepareMove(InsertionPoint const& _newPosition)
{
	if (_newPosition == over())
		return;

	if (m_parent == _newPosition.parent())
		m_parent->moveWithinBrood(m_index, _newPosition.index());
	else
	{
		if (m_parent && m_parent != _newPosition.parent())
			m_parent->removeFromBrood(m_index, this);
		
		m_parent = _newPosition.parent();
		
		if (m_parent)
			m_parent->insertIntoBrood(_newPosition.index(), this);	//m_index set by this, too.
		else
			m_index = UndefinedIndex;
	}
}
void Entity::commitMove(InsertionPoint const& _oldPosition)
{
	if (_oldPosition.parent() == m_parent)
		return;

	BasicRoot* oldRoot = m_rootEntity;
	BasicRoot* newRoot = m_parent ? m_parent->m_rootEntity : 0;
	
	// Tell scene we're leaving if we had a non-null root entity and it's different.
	if (oldRoot && newRoot != oldRoot)
		foreach (CodeScene* i, CodeScene::all())
			i->leaving(this, _oldPosition);
	// Notify ourself if the root entity has changed.
	if (newRoot != oldRoot)
		onLeaveScene(newRoot, oldRoot);
	
	// Change root here, since onLeaveScene requries it to be how it was (existing model poointers will use the cached pointer's - our - rootEntity to unhook itself).
	m_rootEntity = newRoot;
	
	// Update the root of all entities to which we are an ancestor.
	foreach (Entity* e, children())
		e->checkRoot();
		
	if (newRoot)
		newRoot->setChanged();
	if (oldRoot && oldRoot != newRoot)
		oldRoot->setChanged();
}
void Entity::removeFromBrood(int _index, Entity* _e)
{
	M_ASSERT(_index < m_cardinalChildren.size());
	if (_index >= 0)
	{
		Entity* t = m_cardinalChildren.takeAt(_index);
		M_ASSERT(t == _e);	// confirm that we're taken the right one. if this fails, the parent has something else in the position that the child thinks it occupies.
		for (int i = _index; i < m_cardinalChildren.size(); i++)
			m_cardinalChildren[i]->m_index = i;
	}
	else
		for (QHash<int, Entity*>::iterator i = m_namedChildren.find(_index); i != m_namedChildren.end() && i.key() == _index; ++i)
			if (i.value() == _e)
			{
				m_namedChildren.erase(i);
				break;
			}
}
void Entity::removeAllFromBrood(int _index)
{
	M_ASSERT(_index < m_cardinalChildren.size());
	if (_index >= 0)
	{
		m_cardinalChildren.removeAt(_index);
		for (int i = _index; i < m_cardinalChildren.size(); i++)
			m_cardinalChildren[i]->m_index = i;
	}
	else
		m_namedChildren.remove(_index);
}
void Entity::insertIntoBrood(int _index, Entity* _e)
{
	if (_index == UndefinedIndex)
		_index = m_cardinalChildren.size();
	M_ASSERT(_index <= m_cardinalChildren.size());
	if (_index < 0)
	{
		_e->m_index = _index;
		m_namedChildren.insertMulti(_index, _e);
	}
	else
	{
		m_cardinalChildren.insert(_index, _e);
		for (int i = _index; i < m_cardinalChildren.size(); i++)
			m_cardinalChildren[i]->m_index = i;
	}
}
void Entity::moveWithinBrood(int _old, int _new)
{
	if (_old == UndefinedIndex)
		_old = m_cardinalChildren.size() - 1;
	if (_new == UndefinedIndex)
		_new = m_cardinalChildren.size() - 1;
	M_ASSERT(_new <= m_cardinalChildren.size());
	M_ASSERT(child(_old));
	if (_new == _old)
		return;
		
	int rejigFrom = UndefinedIndex;
	if (_old >= 0 && _new >= 0)
	{
		m_cardinalChildren.move(_old, _new);
		rejigFrom = min(_old, _new);
	}
	else if (_old >= 0)
	{
		m_namedChildren.insert(_new, m_cardinalChildren.takeAt(_old));
		m_namedChildren[_old]->m_index = _old;
		rejigFrom = _old;
	}
	else if (_new >= 0)
	{
		m_cardinalChildren.insert(_new, m_namedChildren.take(_old));
		rejigFrom = _new;
	}
	else
		m_namedChildren.insert(_new, m_namedChildren.take(_old));
	
	if (rejigFrom != UndefinedIndex)
		for (int i = rejigFrom; i < m_cardinalChildren.size(); i++)
			m_cardinalChildren[i]->m_index = i;
}
void Entity::checkRoot()
{
	if (parent()->rootEntity() != rootEntity())
	{
		onLeaveScene(parent()->rootEntity(), rootEntity());
		m_rootEntity = parent()->rootEntity();
	}
	foreach (Entity* e, children())
		e->checkRoot();
}

// Freeform dependency management.
QList<Entity*> Entity::dependencies() const
{
	if (!isInModel())
		return QList<Entity*>();
	QSet<Entity*> ret = m_dependencies;
	foreach (Entity* e, children())
		if (familyDependencies() & DependsOnChildren)
			ret << e;
	if (familyDependencies() & DependsOnParent)
		ret << parent();
	return ret.values();
}
QList<Entity*> Entity::dependents() const
{
	if (!isInModel())
		return QList<Entity*>();
	QList<Entity*> ret = m_dependents;
	foreach (Entity* e, children())
		if (e->familyDependencies() & DependsOnParent)
			ret << e;
	if (parent() && (parent()->familyDependencies() & DependsOnChildren))
		ret << parent();
	return ret;
}
void Entity::clearDependents()
{
	foreach (Entity* i, m_dependents)
	{
		i->removeDependency(this);
		i->dependencyRemoved(this);
	}
}
void Entity::addDependency(Entity* _e)
{
	if (_e && !m_dependencies.contains(_e))
	{
		m_dependencies << _e;
		_e->m_dependents << this;
	}
}
void Entity::removeDependency(Entity* _e)
{
	if (_e && m_dependencies.contains(_e))
	{
		m_dependencies.remove(_e);
		_e->m_dependents.removeAt(_e->m_dependents.indexOf(this));
	}
}
void Entity::removeAllDependencies()
{
	foreach (Entity* i, m_dependencies)
	{
		m_dependencies.remove(i);
		i->m_dependents.removeAt(i->m_dependents.indexOf(this));
	}
}
void Entity::updateAncestralDependencies()
{
	int i = 0;
	foreach (Kind k, ancestralDependencies())
	{
		Entity* oa = (i < m_ancestralDependencies.size()) ? m_ancestralDependencies[i] : 0;
		Entity* na = ancestor(k);
		if (oa != na)
		{
			// Ancestor changed...
			removeDependency(oa);
			addDependency(na);
			
			if (na && !oa)
				dependencyAdded(na);
			else if (!na && oa)
				dependencyRemoved(oa);
			else
				dependencySwitched(na, oa);
			
			if (i == m_ancestralDependencies.size())
				m_ancestralDependencies << na;
			else if (i < m_ancestralDependencies.size())
				m_ancestralDependencies[i] = na;
			else
				M_ASSERT(0);
		}
		i++;
	}
}
void Entity::changed(int _aspects)
{
	if (!isInModel() || m_notifiedOfChange&_aspects == _aspects)
		return;
	int oldNOC = m_notifiedOfChange;
	m_notifiedOfChange |= _aspects;
	m_rootEntity->setChanged();
	if (!isEditing())
		checkForCullingLater();
	
	if (_aspects & Visually)
		relayoutLater();
	change(this, EntityChanged);
	foreach (Entity* e, dependents())
	{
		change(e, DependencyChanged, this);
		e->onDependencyChanged(this);
	}
	m_notifiedOfChange = oldNOC;
}

InsertionPoint Entity::firstFor(Kind const& _k)
{
	int pi = definePreferedFor(_k);
	if (pi != NonePrefered)
		return middle(pi);
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		if (middle(i).allowedToBeKind(_k) && (middle(i).exists() && middle(i)->isPlaceholder() || childCountAt(i) < minRequired(i)))
			return middle(i);
	foreach (int i, AuxilliaryRegistrar::get()->names())
		if (middle(i).allowedToBeKind(_k) && (middle(i).exists() && middle(i)->isPlaceholder() || childCountAt(i) < minRequired(i)))
			return middle(i);
	if (back().allowedToBeKind(_k) && cardinalChildCount() < minRequired(Cardinals))
		return back();
	foreach (Entity* i, cardinalChildren())
		if (i->isPlaceholder() && i->isAllowed(_k))
			return i->over();
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		if (middle(i).allowedToBeKind(_k))
			return middle(i);
	foreach (int i, AuxilliaryRegistrar::get()->names())
		if (middle(i).allowedToBeKind(_k))
			return middle(i);
	if (back().allowedToBeKind(_k))
		return back();
	return Nowhere;
}


//****************************************************************
//****************************************************************
//****************************************************************
//****************************************************************
//
// Stuff that causes changes to model and notifies thereof.
//
//****************************************************************
//****************************************************************
//****************************************************************
//****************************************************************

bool Entity::validifyChild(int _i, int* _added)
{
	bool ret = false;
	for (QHash<int, Entity*>::Iterator j = m_namedChildren.find(_i); j != m_namedChildren.end() && j.key() == _i;)
		if (!j.value()->isAllowed())
		{
			j = m_namedChildren.erase(j);
			ret = true;
		}
		else
			++j;
	while (childCountAt(_i) < minRequired(_i))
	{
		middle(_i).spawnPreparedSilent()->parentAdded();
		*_added = (*_added == INT_MAX - 1) ? _i : INT_MAX;
		ret = true;
	}
	if (ret)
		resetLayoutCache();
	return ret;
}
bool Entity::validifyChildren()
{
	bool ret = false;
	int added = INT_MAX - 1;
	for (int i = 0; i < qMax(minRequired(Cardinals), m_cardinalChildren.size()); i++)
	{
		if (i >= m_cardinalChildren.size())
		{
			back().spawnPreparedSilent()->parentAdded();
			added = (added == INT_MAX - 1) ? i : INT_MAX;
		}
		else if (!m_cardinalChildren[i]->isAllowed() && i < minRequired(Cardinals))
			m_cardinalChildren[i]->deleteAndRefill();
		else if (!m_cardinalChildren[i]->isAllowed())
			m_cardinalChildren[i]->killAndDeleteWithNotification();
		else
			continue;
		ret = true;
	}
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		ret = ret || validifyChild(i, &added);
	foreach (int i, AuxilliaryRegistrar::get()->names())
		ret = ret || validifyChild(i, &added);
	
	if (added == INT_MAX)
		childrenInitialised();
	else if (added < INT_MAX - 1)
		childAdded(added);
	
	resetLayoutCache();
	return ret;
}
Entity* Entity::prepareChildren()
{
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		while (childCountAt(i) < minRequired(i))
			middle(i).spawnPreparedSilent()->parentAdded();
	foreach (int i, AuxilliaryRegistrar::get()->names())
		while (childCountAt(i) < minRequired(i))
			middle(i).spawnPreparedSilent()->parentAdded();
	for (int i = m_cardinalChildren.size(); i < minRequired(Cardinals); ++i)
		back().spawnPreparedSilent()->parentAdded();
	childrenInitialised();
	resetLayoutCache();
	return this;
}
bool Entity::removeInvalidChildren()
{
	bool ret = false;
	foreach (Entity* i, m_namedChildren)
		if (!i->isAllowed())
		{
			ret = true;
			i->deleteAndRefill();
		}
	for (int i = m_cardinalChildren.size() - 1; i >= 0; i--)
		if (!m_cardinalChildren[i]->isAllowed())
		{
			ret = true;
			m_cardinalChildren[i]->deleteAndRefill();
		}
	return ret;
}

void Entity::childMoved(Entity* _e, int _oI)
{
	if (_e->botherNotifying() && _e->familyDependencies() & DependsOnIndex)
		_e->indexChanged(_oI);
	if (botherNotifying() && familyDependencies() & TestOnOrder)
		notifyOfChildMove(_e, _oI);
}

void Entity::move(InsertionPoint const& _newPosition)
{
	M_ASSERT(this);
	InsertionPoint old = over();
	
	if (_newPosition.exists() && _newPosition->isPlaceholder())
		_newPosition->replace(this);	// TODO: handle brood-move/replace.
	else
	{
		silentMove(_newPosition);
		if (old.parent() == m_parent)
		{
			//-2-1 0 1 2 3 4 5 6
			//   x A B C D E F      Start
			// B x A C D E F        (home = 1, end = 5, s = +1) -2(1)		1(2) 2(3) 3(4) 4(5)	i(i+s)
			//     A B C D x E F	(home = 6, end = 4, s = -1) 4(-2)		6(5) 5(4)			i(i+s)
			//     A C D E B F		(home = 1, end = 4, s = +1)  4(1)		1(2) 2(3) 3(4)		i(i+s)
			//     A E B C D F		(home = 4, end = 1, s = -1)  1(4)		4(3) 3(2) 2(1)		i(i+s)

			// Move children between home & end, when i's index has changed from i+sign(end - home) to i.
			// except for us; we handle that specially below.
			int home = old.index() < 0 ? m_parent->m_cardinalChildren.size() - 1 : old.index();
			int end = m_index < 0 ? m_parent->m_cardinalChildren.size() : m_index;
			if (int s = sign(end - home))
				for (int i = home; i != end; i += s)
					childMoved(child(i), i + s);
			childMoved(this, old.index());
		}
		else
		{
			parentSwitchedWithChildRemoved(old);
			if (m_parent)
			{
				m_parent->childAdded(m_index);
				m_parent->resetLayoutCache();
			}
		}
	}
}
Entity* Entity::usurp(Entity* _u)
{
	InsertionPoint you = _u->over();

	over().insertSilent(_u);
	
	// Move children over.
	QList<Entity*> es = m_cardinalChildren + m_namedChildren.values();
	foreach (Entity* e, m_cardinalChildren)
		e->silentMove(_u->back());
	foreach (Entity* e, m_namedChildren.values())
		if (_u->isAllowed(e->m_index, e->kind()))
			e->silentMove(_u->middle(e->m_index));
			
	kill();
	
	// Signal we're off.
	oneFootInTheGrave();
	
	// Tell _r's old context (if it has one) that it has gone, and tell _r that it has a new context.
	_u->parentSwitchedWithChildRemoved(you);
	
	// Notify about the children having been moved.
	foreach (Entity* i, es)
		i->parentSwitched(this);
	_u->childrenInitialised();
	
	// Tell _r's new context that it's here instead of us.
	if (_u->m_parent)
		_u->m_parent->childSwitched(_u, this);
		
	delete this;
	_u->resetLayoutCache();
	return _u;
}
Entity* Entity::replace(Entity* _r)
{
	InsertionPoint you = _r->over();
	
	over().insertSilent(_r);
	kill(_r);

	// Signal we're off.
	oneFootInTheGrave();
	// Tell _r's old context (if it has one) that it has gone, and tell _r that it has a new context.
	_r->parentSwitchedWithChildRemoved(you);
	// Tell _r's new context that it's here.
	if (_r->m_parent)
		_r->m_parent->childSwitched(_r, this);
	delete this;
	return _r;
}
Entity* Entity::insert(Entity* _e, int _preferedIndex)
{
	InsertionPoint you = _e->over();
	
	over().insertSilent(_e);
	InsertionPoint ip = _preferedIndex == NonePrefered ? _e->firstFor(kind()) : _e->middle(_preferedIndex);
	M_ASSERT(ip != Nowhere);
	if (ip.allowedToBeKind(kind()))
	{
		// Everything going according to plan.
		// P -n-> this   BECOMES   P -n-> _e -ip.i-> this
		ip.insertSilent(this);
		_e->childAdded(m_index);
		if (_e->parent())
			_e->parent()->childSwitched(_e, this);
		_e->parentSwitchedWithChildRemoved(you);
		if (_e->parent())
			parentSwitched(_e->parent());
		else
			parentAdded();
	}
	else
	{
		// There's nowhere we can go - the prefered index failed:
		// We get deleted and substituted against a newly created entity to fill the prefered index.
		// NEW created at _prefInd, this killed, NEW replaces this.
		// P -n-> this   BECOMES   P -n-> _e -pi-> NEW
		// P's child switched (at n, was this, now _e)
		// _e's parent's child removed, _e's context switched.
		// this will be dead.
		silentMove(Nowhere);
		int x; _e->validifyChild(_preferedIndex, &x);
		if (_e->parent())
			_e->parent()->childSwitched(_e, this);
		_e->parentSwitchedWithChildRemoved(you);
		parentRemoved(_e->parent());
		oneFootInTheGrave();
		killAndDelete(_e->child(_preferedIndex));
	}
	_e->resetLayoutCache();
	return _e;
} 
bool Entity::tryInsert(Entity* _e, int _preferedIndex)
{
	InsertionPoint you = _e->over();
	
	over().insertSilent(_e);
	InsertionPoint ip = _preferedIndex == NonePrefered ? _e->firstFor(kind()) : _e->middle(_preferedIndex);
	if (!ip.allowedToBeKind(kind()))
		ip = Nowhere;
	ip.insertSilent(this);
	
	_e->parentSwitchedWithChildRemoved(you);
	
	if (_e->parent() && parent())
		parentSwitched(_e->parent());
	else if (_e->parent())
		parentRemoved(_e->parent());
	else if (parent())
		parentAdded();
		
	if (ip != Nowhere)
	{
		_e->childAdded(m_index);
	}
	if (_e->parent())
		_e->parent()->childSwitched(_e, this);
		
	_e->resetLayoutCache();
	return ip != Nowhere;
} 
void Entity::deleteAndRefill(Entity* _e, bool _moveToGrave)
{
	Entity* c = parent();
	InsertionPoint p = over();
	
	if (_e)
	{
		replace(_e);
	}
	else if (isNecessary())
	{
		p.spawnPreparedSilent();
		kill(p.entity());
		oneFootInTheGrave();
		p->parentAdded();
		c->childSwitched(p.entity(), this);
		delete this;
	}
	else
	{
		int ci = m_index;
		kill();
		oneFootInTheGrave();
		c->childRemoved(this, ci);
		c->relayoutLater();
		delete this;
	}
	if (_moveToGrave)
		p.nearestEntity()->setCurrent();
}

void Entity::onChildrenInitialised()
{
	foreach (Entity* c, children())
		onDependencyAdded(c);
}
//****************************************************************
//****************************************************************
//****************************************************************
//****************************************************************
//
// The actual notification code.
//
//****************************************************************
//****************************************************************
//****************************************************************
//****************************************************************
void Entity::childAdded(int _index)
{
	if (_index >= 0 && _index < m_cardinalChildren.size() && familyDependencies() & DependsOnChildren)
	{
		if (botherNotifying())
			dependencyAdded(m_cardinalChildren[_index]);
		if (_index >= 0)
			for (int i = _index + 1; i < cardinalChildCount(); i++)
				childMoved(child(i), i - 1);
	}
	else if (botherNotifying() && m_cardinalChildren.size() && familyDependencies() & DependsOnChildren)
	{
		dependencyAdded(m_cardinalChildren.last());
	}
	if (isInModel())
		// This would be a relayoutLater() call, except we know the child will call that, so we can optimise thus:
		resetLayoutCache();
}
void Entity::childrenInitialised()
{
	if (botherNotifying() && familyDependencies() & DependsOnChildren)
	{
		change(this, EntityChildrenInitialised, 0);
		onChildrenInitialised();
	}
/*	else
		qDebug() << "Not bothering to notify (" << botherNotifying() << " - " << isComplete() << ")";
*/	if (isInModel())
		resetLayoutCache();
}
void Entity::childSwitched(Entity* _ch, Entity* _old)
{
	if (botherNotifying() && _ch && familyDependencies() & DependsOnChildren)
		dependencySwitched(_ch, _old);
}
void Entity::childRemoved(Entity* _ch, int _index)
{
	if (_ch && familyDependencies() & DependsOnChildren)
	{
		if (botherNotifying())
			dependencyRemoved(_ch, _index);
		if (_index >= 0)
			for (int i = _index; i < cardinalChildCount(); i++)
				childMoved(child(i), i + 1);
	}
	if (isInModel())
		relayoutLater();
}
void Entity::parentAdded()
{
	if (botherNotifying() && m_parent && familyDependencies() & DependsOnParent)
		dependencyAdded(m_parent);
	updateAncestralDependencies();
	if (isInModel())
		relayoutLater();
}
void Entity::parentSwitched(Entity* _old)
{
	if (botherNotifying() && familyDependencies() & DependsOnParent)
		dependencySwitched(m_parent, _old);
	updateAncestralDependencies();
	relayoutLater();
}
void Entity::parentRemoved(Entity* _old)
{
	if (botherNotifying() && _old && familyDependencies() & DependsOnParent)
		dependencyRemoved(_old);
	updateAncestralDependencies();
}

}
