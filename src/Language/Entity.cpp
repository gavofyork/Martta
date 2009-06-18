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

#include "CommonGraphics.h"
#include "DecorationContext.h"
#include "CodeScene.h"
#include "TypeDefinition.h"
#include "RootEntity.h"
#include "EditDelegate.h"
#include "Entity.h"

namespace Martta
{

#if POOL_ALLOCATOR
QMap<size_t, boost::pool<>* >* Entity::s_pools = 0;
#endif
	
int s_news = 0;
int s_deletes = 0;
	
MARTTA_CPP_BASIC(Entity);
AuxilliaryFace const* Entity::staticAuxilliary() { if (!s_auxilliary_Entity) s_auxilliary_Entity = new Auxilliary<Entity>("Martta::Entity"); return s_auxilliary_Entity; }

QList<ChangeEntry> s_changes;
void change(Entity* _s, ChangeOperation _op, Entity* _o)
{
	static const QStringList ops = QStringList() << "EntityChanged" << "ChildrenInitialised" << "DependencyAdded" << "DependencyRemoved" << "DependencyChanged" << "DependencySwitched" << "ChildMoved" << "ContextIndexChanged";
	s_changes << ChangeEntry(_s, _op, _o);
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

// Identification, search & location.
Identifiable* Entity::findEntity(QString const& _key) const
{
//	qDebug() << "findEntity: " << _key;
	if (_key.isEmpty())
		return const_cast<RootEntity*>(rootEntity());
	if (DeclarationEntity* r = rootEntity()->findDeclaration(_key))
		return r;
	if (_key.startsWith("::"))
		return rootEntity()->findEntity(_key.mid(2));
	if (isKind<Identifiable>())
		if (Identifiable* e = asKind<Identifiable>()->lookupChild(_key.section("::", 0, 0)))
			return _key.contains("::") ? e->self()->findEntity(_key.section("::", 1)) : e;
	return 0;
}
QList<DeclarationEntity*> Entity::spacesInScope() const
{
	return childrenOf<DeclarationEntity>();
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
		if (parent()->cardinalChildCount() <= parent()->minimumRequired())
			return true;
		for (int i = index() + 1; i < parent()->cardinalChildCount(); i++)
			if (!parent()->child(i)->isKind(parent()->allowedKinds(i - 1)))
				return true;
	}
	else
		if (parent()->childCountAt(m_index) - 1 < parent()->minimumRequiredNamed(m_index))
			return true;
	return false;
}
bool Entity::isComplete() const
{
	QList<Entity*> e = cardinalChildren();
	if (e.size() < minimumRequired())
		return false;
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		if (childCountAt(i) < minimumRequiredNamed(i))
			return false;
	foreach (int i, AuxilliaryRegistrar::get()->names())
		if (childCountAt(i) < minimumRequiredNamed(i))
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
		qInformation() << this << "invalid because it's not allowed here by context." << parent();
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
	remove();
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
				e->move(middle(i.toElement().attribute("contextindex").toInt()));
			else if (i.toElement().hasAttribute("childname"))
				e->move(middle(AuxilliaryRegistrar::get()->arbitraryOfName(i.toElement().attribute("childname"))));
			else
				e->move(back());
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
	foreach (Entity* e, f->m_cardinalChildren)
		if (f->usurpsChild(e))
		{
			_e->setFocus(e);
			keyPressEventStarter(_e, true);
			_e->setFocus(f);
			if (_e->isAccepted())
				return;
		}
	
	// GAA FALSE
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
			_e->codeScene()->setCurrent(p.childType());
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
			_e->codeScene()->setCurrent(p.childType());
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
void Entity::moveVirtually(InsertionPoint const& _newPosition)
{
	if (_newPosition.parent() == m_parent)
		return;

	if (m_parent)
		m_parent->removeFromBrood(m_index, this);
	
	m_parent = _newPosition.parent();
	
	if (m_parent)
		m_parent->insertIntoBrood(_newPosition.index(), this);	//m_index set by this, too.
	else
		m_index = UndefinedIndex;
}
void Entity::commitVirtualMove(InsertionPoint const& _oldPosition)
{
	if (_oldPosition.parent() == m_parent) return;

	RootEntity* oldRoot = m_rootEntity;
	RootEntity* newRoot = m_parent ? m_parent->m_rootEntity : 0;
	
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
	if (familyDependencies() & DependsOnContext)
		ret << parent();
	return ret.values();
}
QList<Entity*> Entity::dependents() const
{
	if (!isInModel())
		return QList<Entity*>();
	QList<Entity*> ret = m_dependents;
	foreach (Entity* e, children())
		if (e->familyDependencies() & DependsOnContext)
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
void Entity::changed()
{
	if (!isInModel() || m_notifiedOfChange)
		return;
	m_notifiedOfChange = true;
	m_rootEntity->setChanged();
	if (!isEditing())
		checkForCullingLater();
	
	change(this, EntityChanged);
	if (onChanged())
		foreach (Entity* e, dependents())
		{
			change(e, DependencyChanged, this);
			e->onDependencyChanged(this);
		}
	m_notifiedOfChange = false;
}
void Entity::notifyOfChange(Entity* _dependent)
{
	if (!isInModel() || !isAllowed())
		return;
		
	change(_dependent, DependencyChanged, this);
	_dependent->onDependencyChanged(this);
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
	for (QHash<int, Entity*>::Iterator j = m_namedChildren.find(_i); j != m_namedChildren.end() && j.key() == _i; ++j)
		if (!j.value()->isAllowed())
		{
			j = m_namedChildren.erase(j);
			ret = true;
		}
	while (childCountAt(_i) < minimumRequiredNamed(_i))
	{
		middle(_i).spawnPreparedSilent()->contextAdded();
		*_added = (*_added == INT_MAX - 1) ? _i : INT_MAX;
		ret = true;
	}
	return ret;
}
bool Entity::validifyChildren()
{
	bool ret = false;
	int added = INT_MAX - 1;
	for (int i = 0; i < qMax(minimumRequired(), m_cardinalChildren.size()); i++)
	{
		if (i >= m_cardinalChildren.size())
		{
			back().spawnPreparedSilent()->contextAdded();
			added = (added == INT_MAX - 1) ? i : INT_MAX;
		}
		else if (!m_cardinalChildren[i]->isAllowed() && i < minimumRequired())
			m_cardinalChildren[i]->deleteAndRefill();
		else if (!m_cardinalChildren[i]->isAllowed())
			m_cardinalChildren[i]->killAndDelete();
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
	return ret;
}
Entity* Entity::prepareChildren()
{
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		while (childCountAt(i) < minimumRequiredNamed(i))
			middle(i).spawnPreparedSilent()->contextAdded();
	foreach (int i, AuxilliaryRegistrar::get()->names())
		while (childCountAt(i) < minimumRequiredNamed(i))
			middle(i).spawnPreparedSilent()->contextAdded();
	for (int i = m_cardinalChildren.size(); i < minimumRequired(); ++i)
		back().spawnPreparedSilent()->contextAdded();
	childrenInitialised();
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
Entity* Entity::usurp(Entity* _u)
{
	InsertionPoint you = _u->over();

	over().insertSilent(_u);
	
	// Move children over.
	QList<Entity*> es = m_cardinalChildren + m_namedChildren.values();
	foreach (Entity* e, m_cardinalChildren)
		e->move(_u->back());
	foreach (Entity* e, m_namedChildren.values())
		e->move(_u->middle(e->m_index));
		
	kill();
	
	// Tell _r's old context (if it has one) that it has gone, and tell _r that it has a new context.
	_u->contextSwitchedWithChildRemoved(you);
	
	// Notify about the children having been moved.
	foreach (Entity* i, es)
		i->contextSwitched(this);
	_u->childrenInitialised();
	
	// Tell _r's new context that it's here instead of us.
	if (_u->m_parent)
		_u->m_parent->childSwitched(_u, this);
		
	delete this;
	return _u;
}
Entity* Entity::replace(Entity* _r)
{
	InsertionPoint you = _r->over();
	
	over().insertSilent(_r);
	kill(_r);
	
	// Tell _r's old context (if it has one) that it has gone, and tell _r that it has a new context.
	_r->contextSwitchedWithChildRemoved(you);
	// Tell _r's new context that it's here.
	if (_r->m_parent)
		_r->m_parent->childSwitched(_r, this);
	delete this;
	return _r;
}
Entity* Entity::insert(Entity* _e)
{
	InsertionPoint you = _e->over();
	
	over().insertSilent(_e);
	_e->front().insertSilent(this);
	
	_e->contextSwitchedWithChildRemoved(you);
	contextSwitched(_e->parent());
	_e->childAdded(0);
	if (_e->parent())
		_e->parent()->childSwitched(_e, _e->parent());
	return _e;
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
		kill(p.childType());
		p->contextAdded(c);
		c->childSwitched(p.childType(), this);
		delete this;
	}
	else
	{
		int ci = m_index;
		kill();
		c->childRemoved(this, ci);
		delete this;
	}
	if (_moveToGrave)
		p.nearestEntity()->setCurrent();
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
		for (int i = _index + 1; i < cardinalChildCount(); i++)
		{
			if (child(i)->botherNotifying() && child(i)->familyDependencies() & DependsOnContextIndex)
				child(i)->contextIndexChanged(i - 1);
			if (botherNotifying() && familyDependencies() & TestOnOrder)
				childMoved(child(i), i - 1);
		}
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
		for (int i = _index; i < cardinalChildCount(); i++)
		{
			if (child(i)->botherNotifying() && child(i)->familyDependencies() & DependsOnContextIndex)
				child(i)->contextIndexChanged(i + 1);
			if (botherNotifying() && familyDependencies() & TestOnOrder)
				childMoved(child(i), i + 1);
		}
	}
	if (isInModel())
		relayoutLater();
}
void Entity::contextAdded(Entity* _con)
{
	if (botherNotifying() && _con && familyDependencies() & DependsOnContext)
		dependencyAdded(_con);
	updateAncestralDependencies();
	if (isInModel())
		relayoutLater();
}
void Entity::contextSwitched(Entity* _old)
{
	if (m_parent && !_old)
		contextAdded();
	else if (!m_parent && _old)
		contextRemoved(_old);
	else if (_old != m_parent)
	{
		if (botherNotifying() && familyDependencies() & DependsOnContext)
			dependencySwitched(m_parent, _old);
		updateAncestralDependencies();
		relayoutLater();
	}
}
void Entity::contextRemoved(Entity* _old)
{
	if (botherNotifying() && _old && familyDependencies() & DependsOnContext)
		dependencyRemoved(_old);
	updateAncestralDependencies();
}

}
