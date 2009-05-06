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
#include "FundamentalOperator.h"
#include "EditDelegate.h"
#include "Entity.h"

namespace Martta
{

#if POOL_ALLOCATOR
QMap<size_t, boost::pool<>* >* Entity::s_pools = 0;
#endif
	
int s_news = 0;
int s_deletes = 0;
	
MARTTA_OBJECT_CPP(Entity);
			
QList<ChangeEntry> s_changes;
void change(Entity* _s, ChangeOperation _op, Entity* _o)
{
	static const QStringList ops = QStringList() << "EntityChanged" << "DependencyAdded" << "DependencyRemoved" << "DependencyChanged" << "DependencySwitched" << "ContextIndexChanged";
	s_changes << ChangeEntry(_s, _op, _o);
}

Entity::~Entity()
{
	M_ASSERT(!m_context);
	foreach (Entity* i, m_children)
		i->killAndDelete();
	M_ASSERT(!m_children.size());
}

bool Entity::isComplete() const
{
	QList<Entity*> e = entities();
	if (e.size() < minimumRequired())
		return false;
	foreach (Entity* i, e)
		if (!i->isAllowed())
			return false;
	return true;
}

bool Entity::hasAncestor(Kind _k) const
{
	for (Entity* e = context(); e; e = e->context())
		if (e->isKind(_k))
			return true;
	return false;
}

Entity* Entity::ancestor(Kind _k) const
{
	for (Entity* e = context(); e; e = e->context())
		if (e->isKind(_k))
			return e;
	return 0;
}

int Entity::ancestorIndex(Entity* _a) const
{
	for (Entity const* i = this; i; i = i->context())
		if (i->context() == _a)
			return i->contextIndex();
	return -1;
}

DeclarationEntity* Entity::findEntity(QString const& _key) const
{
	if (_key.isEmpty())
		return const_cast<RootEntity*>(rootEntity());
	if (DeclarationEntity* r = rootEntity()->findDeclaration(_key))
		return r;
	if (_key.startsWith("::"))
		return rootEntity()->findEntity(_key.mid(2));
	if (!isKind<DeclarationEntity>())
		return 0;
	if (DeclarationEntity* e = asKind<DeclarationEntity>()->lookupChild(_key.section("::", 0, 0)))
		return _key.contains("::") ? e->findEntity(_key.section("::", 1)) : e;
	return 0;
}

bool Entity::isNecessary() const
{
	if (!context())
		return false;
	if (context()->entities().size() <= context()->minimumRequired())
		return true;
	for (int i = contextIndex() + 1; i < context()->entities().size(); i++)
		if (!context()->entity(i)->isKind(context()->allowedKinds(i - 1)))
			return true;
	return false;
}

bool Entity::isValidName(QString const& _n)
{
	return QRegExp("[a-zA-Z][a-zA-Z0-9_]*").exactMatch(_n);
}

QList<Entity*> Entity::dependencies() const
{
	if (!isInModel())
		return QList<Entity*>();
	QSet<Entity*> ret = m_dependencies;
	foreach (Entity* e, m_children)
		if (familyDependencies() & DependsOnChildren)
			ret << e;
	if (familyDependencies() & DependsOnContext)
		ret << context();
	return ret.values();
}

QList<Entity*> Entity::dependents() const
{
	if (!isInModel())
		return QList<Entity*>();
	QList<Entity*> ret = m_dependents;
	foreach (Entity* e, m_children)
		if (e->familyDependencies() & DependsOnContext)
			ret << e;
	if (context() && (context()->familyDependencies() & DependsOnChildren))
		ret << context();
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

bool Entity::isValid() const
{
	if (!isInModel())
		return true;
	
	if (!isAllowed())
	{
		qInformation() << this << "invalid because it's not allowed here by context." << context();
		return false;
	}
	if (!isInValidState())
	{
		qInformation() << this << "invalid because it's in an invalid entity state.";
		return false;
	}
	if (context())
		if (!context()->isChildInValidState(contextIndex()))
		{
			qInformation() << this << "invalid because its context considers it in an invalid state.";
			return false;
		}
	return true;
}

void Entity::setContext(Entity* _newContext)
{
	if (_newContext == m_context) return;

//	M_ASSERT(!_e || _e->rootEntity());

	if (rootEntity() && (!_newContext || rootEntity() != _newContext->rootEntity()))
		foreach (CodeScene* i, CodeScene::all())
			i->leaving(this);
	
	InsertionPoint p = over();
	
	if ((_newContext ? _newContext->rootEntity() : 0) != rootEntity())
		onLeaveScene(_newContext ? _newContext->rootEntity() : 0, rootEntity());

	if (m_context)
	{
		int oi = contextIndex();
		m_context->m_children.removeAll(this);
		for (int i = oi; i < siblingCount(); i++)
			sibling(i)->m_contextIndex = i;
	}
	
	m_context = _newContext;
	
	if (m_context)
	{
		m_context->m_children.append(this);
		m_contextIndex = siblingCount() - 1;
		m_rootEntity = m_context->rootEntity();
	}
	else
	{
		m_contextIndex = -1;
		m_rootEntity = 0;
	}

	foreach (Entity* e, m_children)
		e->checkRoot();
}

void Entity::setContextTentatively(Entity* _newContext)
{
	if (_newContext == m_context) return;

	if (m_context)
	{
		int oi = contextIndex();
		m_context->m_children.removeAll(this);
		for (int i = oi; i < siblingCount(); i++)
			sibling(i)->m_contextIndex = i;
	}
	
	m_context = _newContext;
	
	if (m_context)
	{
		m_context->m_children.append(this);
		m_contextIndex = siblingCount() - 1;
	}
	else
	{
		m_contextIndex = -1;
	}
}

void Entity::commitTentativeSetContext(InsertionPoint const& _oldPosition)
{
	Entity* oldContext = _oldPosition.context();
	Entity* newContext = m_context;
	
	if (oldContext == newContext) return;

	m_rootEntity = m_context ? m_context->rootEntity() : 0;

	if (oldContext->rootEntity() && (!newContext || oldContext->rootEntity() != newContext->rootEntity()))
		foreach (CodeScene* i, CodeScene::all())
			i->leaving(this, _oldPosition);
	if ((newContext ? newContext->rootEntity() : 0) != oldContext->rootEntity())
		onLeaveScene(newContext ? newContext->rootEntity() : 0, oldContext ? oldContext->rootEntity() : 0);

	foreach (Entity* e, m_children)
		e->checkRoot();
}

void Entity::undoTentativeSetContext(InsertionPoint const& _oldPosition)
{
	if (_oldPosition.context() == m_context) return;
	
	if (m_context)
	{
		int oi = contextIndex();
		m_context->m_children.removeAll(this);
		for (int i = oi; i < siblingCount(); i++)
			sibling(i)->m_contextIndex = i;
	}

	m_context = _oldPosition.context();
	
	if (m_context)
	{
		int oi = _oldPosition.index();
		m_context->m_children.insert(oi, this);
		for (int i = oi; i < siblingCount(); i++)
			sibling(i)->m_contextIndex = i;
	}
	else
	{
		m_contextIndex = -1;
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
				dependencySwitched(na);
			
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
	if (!isInModel() || !isAllowed() || m_notifiedOfChange)
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

void Entity::moveToPosition(int _index)
{
	M_ASSERT(m_context);
	
	int oi = contextIndex();
	
	if (_index < 0 || _index > siblingCount())
		_index = siblingCount() - 1;
	
	m_context->m_children.removeAll(this);
	m_context->m_children.insert(_index, this);
	
	for (int i = qMin(_index, oi); i <= qMax(_index, oi); i++)
		sibling(i)->m_contextIndex = i;
}

bool Entity::isSuperfluous() const
{
	return isPlaceholder() && !isNecessary() || !isAllowed();
}

void Entity::checkForCullingLater()
{
	if (rootEntity())
		rootEntity()->checkCull(this);
}

bool Entity::cull()
{
	Entity* c = context();
	if (c && !isCurrent() && isSuperfluous())
	{
		deleteAndRefill();
		c->relayoutLater();
		return true;
	}
	return false;
}

void Entity::checkRoot()
{
	onLeaveScene(context()->rootEntity(), rootEntity());
	m_rootEntity = context()->rootEntity();
	foreach (Entity* e, entities())
		e->checkRoot();
}

QList<DeclarationEntity*> Entity::spacesInScope() const
{
	return entitiesOf<DeclarationEntity>();
}

void Entity::clearEntities()
{
	while (entities().size())
		entities().last()->kill();
}

bool Entity::removeInvalidChildren()
{
	bool ret = false;
	for (int i = m_children.size() - 1; i >= 0; i--)
		if (!m_children[i]->isAllowed())
		{
			ret = true;
			m_children[i]->deleteAndRefill();
		}
	return ret;
}

bool Entity::validifyChildren()
{
	bool ret = false;
	for (int i = 0; i < qMax(minimumRequired(), m_children.size()); i++)
	{
		if (i >= m_children.size())
			back().spawnPrepared();
		else if (!m_children[i]->isAllowed() && i < minimumRequired())
			m_children[i]->deleteAndRefill();
		else if (!m_children[i]->isAllowed())
			m_children[i]->killAndDelete();
		else
			continue;
		ret = true;
	}
	onChildrenPrepared();
	return ret;
}

Entity* Entity::prepareChildren()
{
	for (int i = m_children.size(); i < minimumRequired(); ++i)
		back().spawnPrepared();
	onChildrenPrepared();
	return this;
}

Entity* Entity::usurp(Entity* _u)
{
	InsertionPoint you = _u->over();
	
	over().insertSilent(_u);
	QList<Entity*> es = entities();
	foreach (Entity* e, es)
		e->setContext(_u);
	kill();
	
	_u->contextSwitchedWithChildRemoved(you);
	foreach (Entity* i, es)
	{
		i->contextSwitched(this);
		_u->childAdded(i);
	}
	_u->context()->childSwitched(_u);
	delete this;
	return _u;
}

Entity* Entity::insert(Entity* _e)
{
	InsertionPoint you = _e->over();
	
	over().insertSilent(_e);
	_e->front().insertSilent(this);
	
	_e->contextSwitchedWithChildRemoved(you);
	contextSwitched(_e->context());
	_e->childAdded(0);
	if (_e->context())
		_e->context()->childSwitched(_e);
	return _e;
} 

Entity* Entity::replace(Entity* _r)
{
	InsertionPoint you = _r->over();
	
	over().insertSilent(_r);
	kill(_r);
	
	// Tell _r's old context (if it has one) that it has gone, and tell _r that it has a new context.
	_r->contextSwitchedWithChildRemoved(you);
	// Tell _r's new context that it's here.
	if (_r->m_context)
		_r->m_context->childSwitched(_r);
	delete this;
	return _r;
}

void Entity::kill(Entity* _substitute)
{
//	if (isInModel())
//		qDebug() << "Killing" << this;
	if (_substitute)
		rewirePointer(_substitute);
	setContext(0);
	foreach (Entity* i, m_dependencies)
		removeDependency(i);
	clearDependents();
}

bool Entity::attemptAppend(EntityKeyEvent const* _e)
{
	foreach (Kind i, allowedKinds(entities().size()))
		if (AuxilliaryRegistrar::get()->auxilliary(i.name())->dispatchKeyPress(back(), _e))
			return true;
	return false;
}

void Entity::decorate(DecorationContext const& _c) const
{
	if (!isValid())
	{
		_c->setPen(QPen(QColor(255, 128, 128), 2, Qt::DotLine, Qt::RoundCap));
		_c->drawLine(QPointF(_c().left(), _c().bottom() - 2.f), QPointF(_c().right() - 2.f, _c().bottom() - 2.f));
	}
}

bool Entity::attemptInsert(EntityKeyEvent const* _e)
{
	if (context())
		foreach (Kind i, context()->allowedKinds(contextIndex()))
			if (AuxilliaryRegistrar::get()->auxilliary(i.name())->dispatchKeyPress(over(), _e))
				return true;
	return false;
}

void Entity::debugTree() const
{
	QList<Entity const*> ancestors;
	for (Entity const* i = context(); i; i = i->context())
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
	foreach (Entity* i, m_children)
		i->debugTree(_i + "|   ");
}

bool Entity::isAllowed(int _i, Kind _o) const
{
	if (_o.isKind(deniedKinds(_i)))
		return false;
	if (!_o.isKind(allowedKinds(_i)))
		return false;
	return true;
}

void Entity::deleteAndRefill(Entity* _e, bool _moveToGrave)
{
	Entity* c = context();
	InsertionPoint p = over();
	
	if (_e)
	{
		replace(_e);
	}
	else if (isNecessary())
	{
		p.spawnPreparedSilent();
		killAndDelete(p.entity());
		p->contextAdded(c);
		c->childSwitched(p.entity());
	}
	else
	{
		int ci = m_contextIndex;
		kill();
		c->childRemoved(this, ci);
		delete this;
	}
//	if (_moveToGrave)
//		p.nearestEntity()->setCurrent();
}

void Entity::exportDom(QDomElement& _element) const
{
	foreach (Entity* e, entities())
	{
		QDomElement n = _element.ownerDocument().createElement("entity");
		n.setAttribute("kind", e->kind().name());
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
			e->setContext(this);
			if (e)
				e->importDom(i.toElement());
			else
				qCritical() << "Unknown element of kind" << i.toElement().attribute("kind");
		}
}

Entity* Entity::spawn(QString const& _kind)
{
	AuxilliaryFace const* f = AuxilliaryRegistrar::get()->auxilliary(_kind);
	M_ASSERT(f);
	return f->create();
}

void Entity::clearEditing()
{
	foreach (CodeScene* i, CodeScene::all())
		i->setEditing(0);
}

bool Entity::isEditing() const
{
	foreach (CodeScene* i, CodeScene::all())
		if (isEditing(i))
			return true;
	return false;
}

bool Entity::isCurrent() const
{
	foreach (CodeScene* i, CodeScene::all())
		if (isCurrent(i))
			return true;
	return false;
}

bool Entity::isEditing(CodeScene* _s) const
{
	return _s->editEntity() == this;
}

bool Entity::isCurrent(CodeScene* _s) const
{
	return _s->current() == this;
}

void Entity::setEditing(CodeScene* _s)
{
	_s->setEditing(this);
}

EditDelegateFace* Entity::editDelegate(CodeScene* _s)
{
	return _s->editDelegate();
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
	QList<Entity*> s = entities();
	while (s.size())
	{
		Entity* e = s.takeFirst();
		if (e->isPlaceholder())
		{
			e->setCurrent();
			return;
		}
		else
			s = e->entities() + s;
	}
	setCurrent();
}

void Entity::childAdded(int _index)
{
	if (_index >= 0 && _index < m_children.size() && familyDependencies() & DependsOnChildren)
	{
		dependencySwitched(entity(_index));
		for (int i = _index + 1; i < entityCount(); i++)
			if (entity(i)->familyDependencies() & DependsOnContextIndex)
				entity(i)->contextIndexChanged(i - 1);
	}
	else if (_index < 0 && m_children.size() && familyDependencies() & DependsOnChildren)
	{
		dependencyAdded(m_children.last());
	}
	if (isInModel())
		// This would be a relayoutLater() call, except we know the child will call that, so we can optimise thus:
		resetLayoutCache();
}

void Entity::childSwitched(Entity* _ch)
{
	if (_ch && familyDependencies() & DependsOnChildren)
		dependencySwitched(_ch);
}

void Entity::childRemoved(Entity* _ch, int _index)
{
	if (_ch && familyDependencies() & DependsOnChildren)
	{
		dependencyRemoved(_ch, _index);
		for (int i = _index; i < entityCount(); i++)
			if (entity(i)->familyDependencies() & DependsOnContextIndex)
				entity(i)->contextIndexChanged(i + 1);
	}
	if (isInModel())
	{
		relayoutLater();
	}
}

void Entity::contextAdded(Entity* _con)
{
	if (_con && familyDependencies() & DependsOnContext)
		dependencyAdded(_con);
	updateAncestralDependencies();
	if (isInModel())
		relayoutLater();
}

void Entity::contextSwitched(Entity* _old)
{
	if (m_context && !_old)
		contextAdded();
	else if (!m_context && _old)
		contextRemoved(_old);
	else if (_old != m_context)
	{
		if (familyDependencies() & DependsOnContext)
			dependencySwitched(m_context);
		updateAncestralDependencies();
		relayoutLater();
	}
}

void Entity::contextRemoved(Entity* _old)
{
	if (_old && familyDependencies() & DependsOnContext)
		dependencyRemoved(_old);
	updateAncestralDependencies();
}

void Entity::relayout(CodeScene* _s)
{
	_s->relayout(this);
}

void Entity::resetLayoutCache()
{
	foreach (CodeScene* i, CodeScene::all())
		i->resetLayoutCache(this);
}

void Entity::relayoutLater(CodeScene* _s)
{
	_s->relayoutLater(this);
}

void Entity::repaint(CodeScene* _s)
{
	if (_s->isInScene(this))
		_s->repaint(this);
	else if (context())
		context()->repaint(_s);
}

void Entity::repaint()
{
	foreach (CodeScene* i, CodeScene::all())
		repaint(i);
}

void Entity::relayout()
{
	foreach (CodeScene* i, CodeScene::all())
		relayout(i);
}

void Entity::relayoutLater()
{
	foreach (CodeScene* i, CodeScene::all())
		relayoutLater(i);
}

void Entity::activateEvent(CodeScene* _s)
{
	if (!activated(_s) && context())
		context()->activateEvent(_s);
}

void Entity::keyPressEventStarter(EntityKeyEvent* _e)
{
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
	
		if (fe && fe->context())
		{
			_e->setIsFocused(false);
			_e->setFocalIndex(fe->contextIndex());
			fe = fe->context();
		}
		else
			fe = 0;
	}
}

void Entity::keyPressEvent(EntityKeyEvent* _e)
{
	if (keyPressed(_e) || attemptInsert(_e))
	{
		_e->accept();
		return;
	}
	
	if (context())
	{
		_e->setIsFocused(false);
		_e->setFocalIndex(contextIndex());
		context()->keyPressEvent(_e);
	}
}

bool Entity::keyPressed(EntityKeyEvent const* _e)
{
	InsertionPoint p = over();
	if (_e->codeScene()->isCurrent(this) && (_e->key() == Qt::Key_Delete && _e->modifiers() == Qt::ShiftModifier || _e->key() == Qt::Key_Backspace && isEditing(_e->codeScene())))
	{
//		p.context()->debugTree();
//		qDebug() << p.index();
//		debugTree();
		deleteAndRefill(0, true);
//		p.context()->debugTree();
//		qDebug() << p.index();
		if (p.entity())
			_e->codeScene()->setCurrent(p.entity());
	}
	else if (_e->codeScene()->isCurrent(this) && _e->key() == Qt::Key_Delete)
	{
//		p.context()->debugTree();
//		qDebug() << p.index();
//		debugTree();
		if (nonPlaceholderCount() == 1 && isAllowed(nonPlaceholder(0)->kind()))
			deleteAndRefill(nonPlaceholder(0), true);
		else
			deleteAndRefill(0, true);
//		p.context()->debugTree();
//		qDebug() << p.index();
		if (p.entity())
			_e->codeScene()->setCurrent(p.entity());
	}
	else if (_e->codeScene()->isCurrent(this) && (_e->key() == Qt::Key_Escape) && isEditing(_e->codeScene()))
		_e->codeScene()->setEditing(0);
	else if (_e->codeScene()->isCurrent(this) && _e->key() == Qt::Key_Tab && !isEditing(_e->codeScene()))
		_e->codeScene()->setEditing(this);
	else
		return false;
	return true;
}

}
