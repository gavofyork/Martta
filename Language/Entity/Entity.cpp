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

#include <cstdlib>

#include <msSupport.h>
using namespace MarttaSupport;

#include "CullManager.h"
#include "CodeScene.h"
#include "EditDelegate.h"
#include "SpecialKeys.h"
#include "WebStylistRegistrar.h"
#include "Entity.h"

namespace Martta
{

int s_news = 0;
int s_deletes = 0;

void* Entity::operator new(size_t _size)
{
	s_news++;
	void* ret = malloc(_size);
	return ret;
}

void Entity::operator delete(void* p)
{
	s_deletes++;
	free(p);
}

MARTTA_PLACEHOLDER_CPP(Entity);

void Entity::initialiseClass()
{
	WebStylistRegistrar::get()->registerCss(staticKind.auxilliary(),
		// TODO Move these to Stylist
		"body { color: #666; font-size: 12px; font-family: Lucida Grande; background-color: white; }"
		".keyword { font-weight: bold; }"
		".unreal { color: #888; }"
		".minor { color: #888; font-size: 8px; }"
		".symbol { font-weight: bold; }"
		".block { position: relative; margin-left: 20px; }"
		".deblock { position: relative; margin-left: -20px; }"

		".TypeEntity { font-weight: bold; color: #000; }"
		".Referenced { font-weight: normal; color: #000; }"
		".Namespace-label { color: #000; font-weight: bold; }"
		".Class-label { color: #000; font-weight: bold; text-shadow: -1px -1px 0px #f77; }"
		".Label { font-weight: normal; }"
		".ConstLabel-constblock { -webkit-border-image: -webkit-gradient(linear, left top, right top, from(#fff), to(#bdf), color-stop(0.5, #bdf)) 100% 100% 100% 100% repeat repeat; border-width: 0 0 0 12px; padding-left: 8px; }"
		".ConstLabel-nonconstblock { margin-left: 20px; }"
		".AccessLabel-publicblock { margin-left: -8px; border-width: 0 0 0 4px; border-color: #efefbf; border-style: solid; padding-left: 4px; }"
		".AccessLabel-protectedblock { margin-left: -8px; border-width: 0 0 0 4px; border-color: #ffdfbf; border-style: solid; padding-left: 4px; }"
		".AccessLabel-privateblock { margin-left: -8px; border-width: 0 0 0 4px; border-color: #ffbfbf; border-style: solid; padding-left: 4px; }"
		".AccessLabel-public { font-weight: bold; text-shadow: -1px -1px 0px #dedebb; }"
		".AccessLabel-protected { font-weight: bold; text-shadow: -1px -1px 0px #ffddbb; }"
		".AccessLabel-private { font-weight: bold; text-shadow: -1px -1px 0px #ffbbbb; }"
		".Literal { font-size: 13px; color: black; }"
		".TypeEntity { color: black; font-size: 13px; font-weight: bold; text-shadow: -1px -1px 0px #888; }"
		".SimpleType { text-shadow: -1px -1px 0px #fb0; }"
		".FunctionType { text-shadow: -1px -1px 0px #84f; }"
		".HashType { text-shadow: -1px -1px 0px #8f4; }"
		".Operation { font-size: 12px; color: #888; background-color: rgba(0, 0, 0, 0.03); padding: 0 2px 0 2px; }"
		".Statement { background-color: transparent; padding: 0; }"
	);
}

void Entity::finaliseClass()
{
	WebStylistRegistrar::get()->unregisterCss(staticKind.auxilliary());
}

static Hash<Entity const*, String> s_htmlCache;

void addToHtmlCache(Entity const* _e, String const& _s) { s_htmlCache.insert(_e, _s); }
void clearHtmlCache() { s_htmlCache.clear(); }

String refinedHtml(Entity const* _e)
{
	return _e->defineHtml().replace(L"<^>", L"<span id=\"this\"></span>");
}

String htmlEscape(String const& _s)
{
	return String(_s)
		.replace(L"&", L"&amp;")
		.replace(L"\\", L"&#92;")
		.replace(L"<", L"&lt;")
		.replace(L">", L"&gt;")
		.replace(L"'", L"&apos;")
		.replace(L"\"", L"&quot;");
}

String toHtml(Entity const* _e, String const& _tag)
{
	if (!_e)
		return String::null;
	if (s_htmlCache.contains(_e))
		return s_htmlCache[_e];
	return String("<%1 entity=\"true\" ondblclick=\"if (CodeView.attemptEdit(%2)) event.stopPropagation();\" id=\"%2\">%3</%4>").arg(_tag).arg((int)_e).arg(refinedHtml(_e)).arg(_tag.section(L' ', 0, 0));
}

String toHtml(List<Entity const*> const& _es, String const& _delimiter, String const& _tag)
{
	String ret;
	bool first = true;
	foreach (Entity const* e, _es)
	{
		if (first)
			first = false;
		else
			ret += _delimiter;
		ret += toHtml(e, _tag);
	}
	return ret;
}

String Entity::defineHtml() const
{
	String ret = L"<span id=\"this\" class=\"keyword\">" + kind().name() + L"</span><span class=\"minor\">";
	Hash<String, String> p;
	properties(p);
	foreach (String s, p.keys())
		ret += L"[" + s + L"=" + p[s] + L"]";
	ret += L"</span><span>{";
	foreach (Entity* e, children())
		 ret += L" " + toHtml(e) + L"@" + e->indexName();
	ret += L" }</span>";
	return ret;
}

String Entity::indexName(int _i) const
{
	if (_i == UndefinedIndex)
		return "[Undefined]";
	if (_i >= 0)
		return String::number(_i);
	if (_i < virtualEndOfNamed())
		return String((char)('A' + _i - INT_MIN));
	return AuxilliaryRegistrar::get()->nameOfArbitrary(_i);
}

String Entity::namedIndexId() const
{
	if (m_index >= 0)
		return String::null;
	if (m_parent && m_index < m_parent->virtualEndOfNamed())
		return String::number(m_index - INT_MIN);
	return AuxilliaryRegistrar::get()->nameOfArbitrary(m_index);
}

Position Entity::named(String const& _name)
{
	if (_name.isEmpty())
		return back();
	if (_name[0].isNumber())
		return middle(_name.toInt() + INT_MIN);
	return middle(AuxilliaryRegistrar::get()->arbitraryOfName(_name));
}

List<int> Entity::knownNames() const
{
	List<int> ret = AuxilliaryRegistrar::get()->names();
	for (int i = INT_MIN; i < virtualEndOfNamed(); ++i)
		ret << i;
	return ret;
}

Entity::~Entity()
{
	AssertNR(!m_parent);
	clearEntities();
	AssertNR(!m_cardinalChildren.size());
	AssertNR(!m_namedChildren.size());
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
bool Entity::isValidName(String const& _s)
{
	if (_s.isEmpty())
		return false;
	if (!_s[0].isLetter())
		return false;
	for (int i = 1; i < _s.length(); ++i)
		if (!_s[i].isAlphaNumeric() && _s[i] != L'_')
			return false;
	return true;
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
		if (parent()->childCount(m_index) - 1 < parent()->minRequired(m_index))
			return true;
	return false;
}
bool Entity::isComplete() const
{
	List<Entity*> e = cardinalChildren();
	if (e.size() < minRequired(Cardinals))
		return false;
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		if (childCount(i) < minRequired(i))
			return false;
	foreach (int i, AuxilliaryRegistrar::get()->names())
		if (childCount(i) < minRequired(i))
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
		mInfo() << this << "invalid because it's not allowed here by parent." << parent();
		return false;
	}
	if (!isInValidState())
	{
		mInfo() << this << "invalid because it's in an invalid entity state.";
		return false;
	}
	if (parent())
		if (!parent()->isChildInValidState(index()))
		{
			mInfo() << this << "invalid because its context considers it in an invalid state.";
			return false;
		}
	return true;
}
bool Entity::isSuperfluous() const
{
	return (isPlaceholder() && !isNecessary()) || !isAllowed();
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
	CullManager::get()->checkCull(this);
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
//		mDebug() << "Killing" << this;
	if (_substitute)
		rewirePointer(_substitute);
	silentRemove();
}
void Entity::clearEntities()
{
	List<Entity*> tbd;
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
	List<Entity const*> ancestors;
	for (Entity const* i = parent(); i; i = i->parent())
		ancestors.prepend(i);
	String indent = "";
	foreach (Entity const* i, ancestors)
	{
		mDebug() << indent << i;
		indent += "    ";
	}
	debugTree(indent);
}

void Entity::debugTree(String const& _i) const
{
	mDebug() << _i << this;
	for (Hash<int, Entity*>::ConstIterator i = m_namedChildren.begin(); i != m_namedChildren.end(); ++i)
		if (i.key() < virtualEndOfNamed())
			i.value()->debugTree(_i + "|   [" + String::number(i.key() - INT_MIN) + "] ");
		else
			i.value()->debugTree(_i + "|   [" + AuxilliaryRegistrar::get()->nameOfArbitrary(i.key()) + "] ");
	foreach (Entity* i, m_cardinalChildren)
		i->debugTree(_i + "|   ");
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
	List<Entity*> s = children();
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
	_s->repaint(this);
}
void Entity::repaint()
{
	foreach (CodeScene* i, CodeScene::all())
		repaint(i);
}
String Entity::defineLayout(ViewKeys const&) const
{
	return "^;ycode;'[]'";
}
/*void Entity::decorate(DecorationContext const& _c) const
{
	if (!isValid())
	{
		_c->setPen(QPen(QColor(255, 128, 128), 2, Qt::DotLine, Qt::RoundCap));
		_c->drawLine(QPointF(_c().left(), _c().bottom() - 2.f), QPointF(_c().right() - 2.f, _c().bottom() - 2.f));
	}
}*/

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
		_s->setViewKey(this, "expanded", !_s->viewKeys(this)["expanded"].toBool());
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
void Entity::keyPressEventStarter(KeyEvent* _e, bool _abortive)
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
		if ((fe && fe->keyPressed(_e)) || (fe && fe->attemptInsert(_e)) || (fe && _e->codeScene()->manageKeyPress(*_e, fe)))
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

bool Entity::keyPressed(KeyEvent const* _e)
{
	Position p = over();
	if (_e->codeScene()->isCurrent(this) && ((_e->text() == L"\x7f" && _e->modifiers() == ShiftModifier) || (_e->text() == L"\b" && isEditing(_e->codeScene()))) && !isFixed())
	{
//		p.parent()->debugTree();
//		mDebug() << p.index();
//		debugTree();
		deleteAndRefill(0, false);	// NOTE: Was true; changed to false to avoid erroneous currents being set. May need a rethink.
//		p.parent()->debugTree();
//		mDebug() << p.index();
		if (p.exists())
			_e->codeScene()->setCurrent(p.entity());
	}
	else if (_e->codeScene()->isCurrent(this) && _e->text() == L"\x7f" && !isFixed())
	{
//		p.parent()->debugTree();
//		mDebug() << p.index();
//		debugTree();
		if (nonPlaceholderCount() == 1 && isAllowed(nonPlaceholder(0)->kind()))
			deleteAndRefill(nonPlaceholder(0), false);	// SEE ABOVE.
		else
			deleteAndRefill(0, false);	// SEE ABOVE.
//		p.parent()->debugTree();
//		mDebug() << p.index();
		if (p.exists())
			_e->codeScene()->setCurrent(p.entity());
	}
	else if (_e->codeScene()->isCurrent(this) && _e->text() == L"\x1b" && isEditing(_e->codeScene()))
		_e->codeScene()->setEditing(0);
	else if (_e->codeScene()->isCurrent(this) && _e->text() == L"\t" && !isEditing(_e->codeScene()) && !isFixed())
		_e->codeScene()->setEditing(this);
	else if (_e->text() == L"\t")
		activated(_e->codeScene());
/*	else if (_e->text() == "{" && !_e->codeScene()->viewKeys(this)["expanded"].toBool() && isExpander())
	{
		_e->codeScene()->setViewKey(this, "expanded", true);
		relayout(_e->codeScene());
		isExpander()->setCurrent();
	}
	else if (_e->text() == "}" && _e->codeScene()->viewKeys(this)["expanded"].toBool() && isExpander())
	{
		_e->codeScene()->setViewKey(this, "expanded", false);
		relayout(_e->codeScene());
		setCurrent();
	}*/
	else
		return false;
	return true;
}
bool Entity::attemptInsert(KeyEvent const* _e)
{
	if (parent())
		foreach (Kind i, parent()->allowedKinds(index()))
			if (AuxilliaryRegistrar::get()->auxilliary(i.name())->dispatchKeyPress(over(), _e))
				return true;
	return false;
}
bool Entity::attemptAppend(KeyEvent const* _e)
{
	foreach (int i, knownNames() << cardinalChildCount())
		foreach (Kind k, allowedKinds(i))
			if (AuxilliaryRegistrar::get()->auxilliary(k.name())->dispatchKeyPress(middle(i), _e))
				return true;
	return false;
}

// Context/position changing.
void Entity::prepareMove(Position const& _newPosition)
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
void Entity::commitMove(Position const& _oldPosition)
{
	if (_oldPosition.parent() == m_parent)
		return;

	Entity* oldRoot = _oldPosition.parent() ? _oldPosition.parent()->root() : 0;
	Entity* newRoot = root();

	// TODO: Must be a better way of doing this?
	// Tell scene we're leaving if we had a non-null root entity and it's different.
	if (oldRoot && newRoot != oldRoot)
		foreach (CodeScene* i, CodeScene::all())
			i->leaving(this, _oldPosition);
}
void Entity::removeFromBrood(int _index, Entity* _e)
{
	AssertNR(_index < m_cardinalChildren.size());
	if (_index >= 0)
	{
		Entity* t = m_cardinalChildren.takeAt(_index);
		(void)(t);
		AssertNR(t == _e);	// confirm that we're taken the right one. if this fails, the parent has something else in the position that the child thinks it occupies.
		for (int i = _index; i < m_cardinalChildren.size(); i++)
			m_cardinalChildren[i]->m_index = i;
	}
	else
		for (Hash<int, Entity*>::Iterator i = m_namedChildren.find(_index); i != m_namedChildren.end() && i.key() == _index; ++i)
			if (i.value() == _e)
			{
				m_namedChildren.erase(i);
				break;
			}
}
void Entity::removeAllFromBrood(int _index)
{
	AssertNR(_index < m_cardinalChildren.size());
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
	AssertNR(_index <= m_cardinalChildren.size());
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
	AssertNR(_new <= m_cardinalChildren.size());
	AssertNR(child(_old));
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

void Entity::changed(int _aspects)
{
	if (!isInModel())
		return;
	if (Dependee::changed(_aspects))
	{
		if (!isEditing())
			checkForCullingLater();
		if (_aspects & Visually)
			relayoutLater();
	}
}

Position Entity::firstFor(Kind const& _k)
{
	int pi = definePreferedFor(_k);
	if (pi != NonePrefered)
		return middle(pi);
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		if (middle(i).allowedToBeKind(_k) && (middle(i).exists() && (middle(i)->isPlaceholder() || childCount(i) < minRequired(i))))
			return middle(i);
	foreach (int i, AuxilliaryRegistrar::get()->names())
		if (middle(i).allowedToBeKind(_k) && ((middle(i).exists() && middle(i)->isPlaceholder()) || childCount(i) < minRequired(i)))
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

	foreach (Entity* e, m_namedChildren.values(_i))
		if (!e->isAllowed())
		{
			e->oneFootInTheGrave();
			e->killAndDelete();
		}
	while (childCount(_i) < minRequired(_i))
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
	for (int i = 0; i < max(minRequired(Cardinals), m_cardinalChildren.size()); i++)
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

	if (added != INT_MAX - 1)
		resetLayoutCache();
	return ret;
}
Entity* Entity::prepareChildren()
{
	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
		while (childCount(i) < minRequired(i))
			middle(i).spawnPreparedSilent()->parentAdded();
	foreach (int i, AuxilliaryRegistrar::get()->names())
		while (childCount(i) < minRequired(i))
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

Entity* Entity::evaluate(String const& _exp, int* _readUpto)
{
	uint pe;
	int eon = _exp.length();
	if ((pe = _exp.indexOf(L'{')) < (uint)eon)
		eon = pe;
	if ((pe = _exp.indexOf(L'[')) < (uint)eon)
		eon = pe;
	if ((pe = _exp.indexOf(L'}')) < (uint)eon)
		eon = pe;
	String name = _exp.left(eon);
	AuxilliaryFace const* af = AuxilliaryRegistrar::get()->auxilliary(name);
	if (!af)
		af = AuxilliaryRegistrar::get()->auxilliary("Martta::" + name);
	Entity* ret = af ? af->create() : new Entity;

	Hash<String, String> ps;
	while (eon < _exp.length() && _exp[eon] != L'}' && _exp[eon] != L'@')
		if (_exp[eon] == L'[')
		{
			int eq = _exp.indexOf(L'=', eon);
			int end = _exp.indexOf(L']', eq);
			ps.insert(_exp.mid(eon + 1, eq - (eon + 1)), _exp.mid(eq + 1, end - (eq + 1)));
			eon = end + 1;
		}
		else if (_exp[eon] == L'{')
		{
			int eq;
			Entity* e = evaluate(_exp.mid(eon + 1), &eq);
			AssertNR(e);
			eon += eq + 1;
			if (_exp[eon] == L'@')
			{
				String at = _exp.mid(eon + 1, _exp.indexOf(L'}', eon + 1) - (eon + 1));
				ret->named(at).place(e);
				eon += at.length() + 2;
			}
			else if (_exp[eon] == L'}')
			{
				Position p = ret->firstFor(e->kind());
				if (p == Nowhere)
					delete e;
				else
					p.place(e);
				eon++;
			}
		}
	ret->validifyChildren();
	ret->setProperties(ps);
	if (_readUpto)
		*_readUpto = eon;
	return ret;
}

void Entity::put(Position const& _newPosition)
{
	AssertNR(this);
	if (_newPosition.exists() && _newPosition->isPlaceholder())
		_newPosition->replace(this);	// TODO: handle brood-move/replace.
	else
		move(_newPosition);
}

void Entity::move(Position const& _newPosition)
{
	AssertNR(this);
	Position old = over();

	silentMove(_newPosition);
	if (old.parent() == m_parent && m_parent)
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
				m_parent->childMoved(m_parent->m_cardinalChildren[i], i + s);
		m_parent->childMoved(this, old.index());
		m_parent->relayoutLater();
	}
	else
	{
		parentSwitchedWithChildRemoved(old);
		if (m_parent)
		{
			m_parent->childAdded(m_index);
			m_parent->resetLayoutCache();
			m_parent->relayoutLater();
		}
	}
}
Entity* Entity::usurp(Entity* _u)
{
	Position you = _u->over();

	over().insertSilent(_u);

	// Move children over.
	List<Entity*> es = m_cardinalChildren + m_namedChildren.values();
	foreach (Entity* e, m_cardinalChildren)
		e->silentMove(_u->back());
	foreach (Entity* e, m_namedChildren.values())
		e->silentMove(_u->middle(e->m_index));

	kill(_u);

	// Signal we're off.
	oneFootInTheGrave(_u);

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
	Position you = _r->over();

	over().insertSilent(_r);
	kill(_r);

	// Signal we're off.
	oneFootInTheGrave(_r);
	// Tell _r's old context (if it has one) that it has gone, and tell _r that it has a new context.
	_r->parentSwitchedWithChildRemoved(you);
	// Tell _r's new context that it's here.
	if (_r->m_parent)
	{
		_r->m_parent->childSwitched(_r, this);
		_r->m_parent->relayoutLater();
	}
	delete this;
	return _r;
}
Entity* Entity::insert(Entity* _e, int _preferedIndex)
{
	Position you = _e->over();

	over().insertSilent(_e);
	Position ip = _preferedIndex == NonePrefered ? _e->firstFor(kind()) : _e->middle(_preferedIndex);
	AssertNR(ip != Nowhere);
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
		oneFootInTheGrave(_e->child(_preferedIndex));
		killAndDelete(_e->child(_preferedIndex));
	}
	(_e->parent() ? _e->parent() : _e)->relayoutLater();
	return _e;
}
bool Entity::tryInsert(Entity* _e, int _preferedIndex)
{
	Position you = _e->over();

	over().insertSilent(_e);
	Position ip = _preferedIndex == NonePrefered ? _e->firstFor(kind()) : _e->middle(_preferedIndex);
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
	SafePointer<Entity> c = parent();
	Position p = over();

	if (_e)
	{
		replace(_e);
	}
	else if (isNecessary())
	{
		p.spawnPreparedSilent();
		kill(p.entity());
		oneFootInTheGrave(p.entity());
		p->parentAdded();
		c->childSwitched(p.entity(), this);
		if (c)
			c->relayoutLater();
		delete this;
	}
	else
	{
		int ci = m_index;
		kill();
		oneFootInTheGrave();
		c->childRemoved(this, ci);
		if (c)
			c->relayoutLater();
		delete this;
	}
	if (_moveToGrave)
		p.nearestEntity()->setCurrent();
}

void Entity::notifyOfStrobe(Entity* _strobeCreation)
{
	if (_strobeCreation && m_parent)
	{
		parentSwitched(_strobeCreation);
		relayoutLater();
	}
	else if (_strobeCreation)
		parentRemoved(_strobeCreation);
	else if (m_parent)
		parentAdded();

	if (m_parent)
	{
		if (_strobeCreation)
			m_parent->childSwitched(this, _strobeCreation);
		else
			m_parent->childAdded(m_index);
		m_parent->resetLayoutCache();
	}
}

}
