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
#include "Concept.h"

namespace Martta
{

#if defined(DEBUG)
int s_news = 0;
int s_deletes = 0;

void* Concept::operator new(size_t _size)
{
	s_news++;
	void* ret = malloc(_size);
	AssertNR(ret);
	return ret;
}

void Concept::operator delete(void* p)
{
	s_deletes++;
	free(p);
}
#endif

MARTTA_NOTION_CPP(Concept);

void Concept::loadFinished()
{
	apresLoad();
	foreach (Concept* i, children())
		i->loadFinished();
}

String Concept::indexName(int _i) const
{
	if (_i == UndefinedIndex)
		return "[Undefined]";
	if (_i >= 0)
		return String::number(_i);
//	if (_i < virtualEndOfNamed())
//		return String((char)('A' + _i - INT_MIN));
	return AuxilliaryRegistrar::get()->nameOfArbitrary(_i);
}

String Concept::namedIndexId() const
{
	if (m_index >= 0)
		return String::null;
//	if (m_parent && m_index < m_parent->virtualEndOfNamed())
//		return String::number(m_index - INT_MIN);
	return AuxilliaryRegistrar::get()->nameOfArbitrary(m_index);
}

Position Concept::named(String const& _name)
{
	if (_name.isEmpty())
		return back();
	if (_name[0].isNumber())
		return middle(_name.toInt() + INT_MIN);
	return middle(AuxilliaryRegistrar::get()->arbitraryOfName(_name));
}

List<int> Concept::knownNames() const
{
	List<int> ret = AuxilliaryRegistrar::get()->names();
//	for (int i = INT_MIN; i < virtualEndOfNamed(); ++i)
//		ret << i;
	return ret;
}

Concept::~Concept()
{
	AssertNR(!m_parent);
	clearChildren();
	AssertNR(!m_cardinalChildren.size());
	AssertNR(!m_namedChildren.size());
}

// Model structure management.
bool Concept::hasAncestor(Kind _k) const
{
	for (Concept* e = this ? parent() : 0; e; e = e->parent())
		if (e->isKind(_k))
			return true;
	return false;
}
Concept* Concept::ancestor(Kind _k) const
{
	for (Concept* e = this ? parent() : 0; e; e = e->parent())
		if (e->isKind(_k))
			return e;
	return 0;
}
int Concept::ancestorIndex(Kind _k) const
{
	int ret = index();
	for (Concept* e = this ? parent() : 0; e; ret = e->index(), e = e->parent())
		if (e->isKind(_k))
			return ret;
	return UndefinedIndex;
}
bool Concept::hasSelfAncestor(Kind _k) const
{
	for (Concept* e = const_cast<Concept*>(this); e; e = e->parent())
		if (e->isKind(_k))
			return true;
	return false;
}
Concept* Concept::selfAncestor(Kind _k) const
{
	for (Concept const* e = this; e; e = e->parent())
		if (e->isKind(_k))
			return const_cast<Concept*>(e);
	return 0;
}
bool Concept::hasSelfAncestor(Concept const* _a) const
{
	for (Concept const* i = this; i; i = i->parent())
		if (i == _a)
			return true;
	return false;
}
int Concept::ancestorIndex(Concept const* _a) const
{
	for (Concept const* i = this; i; i = i->parent())
		if (i->parent() == _a)
			return i->index();
	return UndefinedIndex;
}

// Validity/status checking
bool Concept::isValidName(String const& _s)
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
bool Concept::isNecessary() const
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
bool Concept::isComplete() const
{
	List<Concept*> e = cardinalChildren();
	if (e.size() < minRequired(Cardinals))
		return false;
//	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
//		if (childCount(i) < minRequired(i))
//			return false;
	foreach (int i, AuxilliaryRegistrar::get()->names())
		if (childCount(i) < minRequired(i))
			return false;
	foreach (Concept* i, children())
		if (!i->isAllowed())
			return false;
	return true;
}
bool Concept::isValid() const
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
bool Concept::isSuperfluous() const
{
	return (isPlaceholder() && !isNecessary()) || !isAllowed();
}
bool Concept::isAllowed(int _i, Kind _o) const
{
	if (_o.isKind(deniedKinds(_i)))
		return false;
	if (!_o.isKind(allowedKinds(_i)))
		return false;
	return true;
}

// Culling
void Concept::checkForCullingLater()
{
	CullManager::get()->checkCull(this);
}
bool Concept::cull()
{
	Concept* c = parent();
	if (c && !isCurrentOrAncestor() && (!isNecessary() || c->allowedKinds(index()).commonBase() != kind()) && isSuperfluous())
	{
		deleteAndRefill(onDecay());
		return true;
	}
	return false;
}

// These do *not* send notifications. Be careful when using.
void Concept::kill(Concept* _substitute)
{
//	if (isInModel())
//		mDebug() << "Killing" << this;
	if (_substitute)
		rewirePointer(_substitute);
	silentRemove();
}
void Concept::clearChildren()
{
	List<Concept*> tbd;
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
void Concept::debugTree() const
{
	List<Concept const*> ancestors;
	for (Concept const* i = parent(); i; i = i->parent())
		ancestors.prepend(i);
	String indent = "";
	foreach (Concept const* i, ancestors)
	{
		mDebug() << indent << i;
		indent += "    ";
	}
	debugTree(indent);
}

void Concept::debugTree(String const& _i) const
{
	mDebug() << _i << this;
	for (Hash<int, Concept*>::ConstIterator i = m_namedChildren.begin(); i != m_namedChildren.end(); ++i)
//		if (i.key() < virtualEndOfNamed())
//			i.value()->debugTree(_i + "|   [" + String::number(i.key() - INT_MIN) + "] ");
//		else
			i.value()->debugTree(_i + "|   [" + AuxilliaryRegistrar::get()->nameOfArbitrary(i.key()) + "] ");
	foreach (Concept* i, m_cardinalChildren)
		i->debugTree(_i + "|   ");
}

// Navigation in CodeScene(s)
bool Concept::isCurrent() const
{
	foreach (CodeScene* i, CodeScene::all())
		if (i->isCurrent(this))
			return true;
	return false;
}
bool Concept::isCurrentOrAncestor() const
{
	foreach (CodeScene* i, CodeScene::all())
		if (i->current()->hasSelfAncestor(this))
			return true;
	return false;
}

// Editting in CodeScene(s)
bool Concept::isEditing() const
{
	foreach (CodeScene* i, CodeScene::all())
		if (i->isEditing(this))
			return true;
	return false;
}
void Concept::clearEditing()
{
	foreach (CodeScene* i, CodeScene::all())
		i->setEditing(0);
}

// Keypress/UI event handlers.
List<Concept*> Concept::savedChildren() const
{
	List<Concept*> ret;
	foreach (int i, knownNames())
		if (Concept* c = child(i))
			ret.append(c);
	return ret + cardinalChildren();
}

void Concept::keyPressEventStarter(KeyEvent* _e, bool _abortive)
{
	Concept* f = _e->focus();
	foreach (Concept* e, f->children())
		if (f->usurpsChild(e))
		{
			_e->codeScene()->handleNote("Child-May-Usurp:<br/>");
			_e->setFocus(e);
			keyPressEventStarter(_e, true);
			_e->setFocus(f);
			if (_e->isAccepted())
				return;
		}

	if (_e->codeScene()->isEditing(_e->focus()) && _e->codeScene()->editDelegate() && _e->codeScene()->editDelegate()->keyPressed(_e))
	{
		_e->codeScene()->handleNote("Edit.<br/>");
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

	SafePointer<Concept, true> fe = _e->focus();
	if (_e->codeScene()->editDelegate())
	{
		_e->codeScene()->handleNote("(Out-of-edit)<br/>");
		_e->codeScene()->setEditing(0);
	}
	if (!fe)
		fe = _e->codeScene()->current();

	while (fe)
	{
		_e->codeScene()->handleNote(L"Routing: <b>" + *fe + L"</b>:<br/>");
		if (_e->codeScene()->manageKeyPress(*_e, fe))
		{
			_e->codeScene()->handleNote("Page-Internal.<br/>");
			_e->accept();
			return;
		}
		if (fe && fe->keyPressed(_e))
		{
			_e->codeScene()->handleNote("Standard.<br/>");
			_e->accept();
			return;
		}
		if (fe && fe->attemptInsert(_e))
		{
			_e->codeScene()->handleNote("Insert.<br/>");
			_e->accept();
			return;
		}

		_e->codeScene()->handleNote("To-parent...<br/>");
		if (fe && fe->parent())
		{
			_e->setIsFocused(false);
			_e->setFocalIndex(fe->index());
			fe = fe->parent();
		}
		else
			fe = 0;

		if (_abortive)
		{
			_e->codeScene()->handleNote("Abortive.");
			return;
		}
	}
}

bool Concept::keyPressed(KeyEvent const* _e)
{
	Position p = over();
	if (_e->codeScene()->isCurrent(this) && (_e->text() == DeleteKey && _e->modifiers() == ShiftModifier) && !isFixed())
	{
		_e->codeScene()->rememberCurrent();
		deleteAndRefill(0);	// NOTE: Was true; changed to false to avoid erroneous currents being set. May need a rethink.
		_e->codeScene()->restoreCurrent();
	}
	else if (_e->codeScene()->isCurrent(this) && _e->text() == DeleteKey && !isFixed())
	{
		_e->codeScene()->rememberCurrent();
		if (properCount() == 1 && over().allowedToBeKind(proper(0)->kind()))
			deleteAndRefill(proper(0));	// SEE ABOVE.
		else
			deleteAndRefill(0);	// SEE ABOVE.
		_e->codeScene()->restoreCurrent();
	}
	else if (_e->codeScene()->isCurrent(this) && _e->text() == TabKey && !_e->codeScene()->isEditing(this) && !isFixed())
		_e->codeScene()->setEditing(this);
	else
		return false;
	return true;
}
bool Concept::attemptInsert(KeyEvent const* _e)
{
	if (parent())
		foreach (Kind i, parent()->allowedKinds(index()))
			if (AuxilliaryRegistrar::get()->auxilliary(i.name())->dispatchKeyPress(over(), _e))
				return true;
	return false;
}
bool Concept::attemptAppend(KeyEvent const* _e)
{
	foreach (int i, knownNames() << cardinalChildCount())
		foreach (Kind k, allowedKinds(i))
			if (AuxilliaryRegistrar::get()->auxilliary(k.name())->dispatchKeyPress(middle(i), _e))
				return true;
	return false;
}

// Context/position changing.
void Concept::silentMove(Position const& _newPosition)
{
	if (_newPosition == over() || (!m_parent && !_newPosition.parent()))
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
void Concept::removeFromBrood(int _index, Concept* _e)
{
	AssertNR(_index < m_cardinalChildren.size());
	if (_index >= 0)
	{
		Concept* t = m_cardinalChildren.takeAt(_index);
		(void)(t);
		AssertNR(t == _e);	// confirm that we're taken the right one. if this fails, the parent has something else in the position that the child thinks it occupies.
		for (int i = _index; i < m_cardinalChildren.size(); i++)
			m_cardinalChildren[i]->m_index = i;
	}
	else
		for (Hash<int, Concept*>::Iterator i = m_namedChildren.find(_index); i != m_namedChildren.end() && i.key() == _index; ++i)
			if (i.value() == _e)
			{
				m_namedChildren.erase(i);
				break;
			}
}
void Concept::removeAllFromBrood(int _index)
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
void Concept::insertIntoBrood(int _index, Concept* _e)
{
	if (_index == UndefinedIndex)
		_index = m_cardinalChildren.size();
	AssertNR(_index <= m_cardinalChildren.size());
	if (_index < 0)
	{
		_e->m_index = _index;
		m_namedChildren.insertMulti(_index, _e);
/*		if (m_namedChildren.count(_index) > 3)
			Assert(false, "Strange");*/
	}
	else
	{
		m_cardinalChildren.insert(_index, _e);
		for (int i = _index; i < m_cardinalChildren.size(); i++)
			m_cardinalChildren[i]->m_index = i;
	}
}
void Concept::moveWithinBrood(int _old, int _new)
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

void Concept::changed(int _aspects)
{
	if (!isInModel())
		return;
	SafePointer<Concept> safeThis = this;
	if (Dependee::changed(_aspects) && safeThis)
		if (!isEditing())
			checkForCullingLater();
}

Position Concept::firstFor(Kind const& _k)
{
	int pi = definePreferedFor(_k);
	if (pi != NonePrefered)
		return middle(pi);
//	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
//		if (middle(i).allowedToBeKind(_k) && (middle(i).exists() && (middle(i)->isPlaceholder() || childCount(i) < minRequired(i))))
//			return middle(i);
	foreach (int i, AuxilliaryRegistrar::get()->names())
		if (middle(i).allowedToBeKind(_k) && ((middle(i).exists() && middle(i)->isPlaceholder()) || childCount(i) < minRequired(i)))
			return middle(i);
	if (back().allowedToBeKind(_k) && cardinalChildCount() < minRequired(Cardinals))
		return back();
	foreach (Concept* i, cardinalChildren())
		if (i->isPlaceholder() && i->over().allowedToBeKind(_k))
			return i->over();
//	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
//		if (middle(i).allowedToBeKind(_k))
//			return middle(i);
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

bool Concept::validifyChild(int _i, int* _added)
{
	bool ret = false;

	foreach (Concept* e, m_namedChildren.values(_i))
		if (!e->isAllowed())
		{
			e->inLimbo();
			e->killAndDelete();
		}
	while (childCount(_i) < minRequired(_i))
	{
		middle(_i).spawnPreparedSilent()->parentAdded();
		*_added = (*_added == INT_MAX - 1) ? _i : INT_MAX;
		ret = true;
	}
	return ret;
}
bool Concept::validifyChildren()
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
//	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
//		ret = ret || validifyChild(i, &added);
	foreach (int i, AuxilliaryRegistrar::get()->names())
		ret = ret || validifyChild(i, &added);

	if (added == INT_MAX)
		childrenInitialised();
	else if (added < INT_MAX - 1)
		childAdded(added);

	return ret;
}
Concept* Concept::prepareChildren()
{
//	for (int i = INT_MIN; i < virtualEndOfNamed(); i++)
//		while (childCount(i) < minRequired(i))
//			middle(i).spawnPreparedSilent()->parentAdded();
	foreach (int i, AuxilliaryRegistrar::get()->names())
	{
//		mDebug() << this;
//		mDebug() << i << minRequired(i) << childCount(i);
		while (childCount(i) < minRequired(i))
			middle(i).spawnPreparedSilent()->parentAdded();
	}
	for (int i = m_cardinalChildren.size(); i < minRequired(Cardinals); ++i)
		back().spawnPreparedSilent()->parentAdded();

	childrenInitialised();
	return this;
}
bool Concept::removeInvalidChildren()
{
	bool ret = false;
	foreach (Concept* i, m_namedChildren)
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

Concept* Concept::evaluate(String const& _exp, int* _readUpto)
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
	Concept* ret = af ? af->create() : new Concept;

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
			Concept* e = evaluate(_exp.mid(eon + 1), &eq);
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

void Concept::put(Position const& _newPosition)
{
	AssertNR(this);
	if (_newPosition.exists() && _newPosition->isPlaceholder())
		_newPosition->replace(this);	// TODO: handle in-brood replace.
	else
		move(_newPosition);
}

void Concept::shove(Position const& _newPosition)
{
	AssertNR(this);
	if (_newPosition.exists() && _newPosition->index() < 0)
		_newPosition->replace(this);
	else
		put(_newPosition);
}

void Concept::move(Position const& _newPosition)
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
	}
	else
	{
		parentSwitchedWithChildRemoved(old);
		if (m_parent)
			m_parent->childAdded(m_index);
	}
}
Concept* Concept::usurp(Concept* _u)
{
	Position you = _u->over();

	over().insertSilent(_u);

	// Move children over.
	List<Concept*> es = m_cardinalChildren + m_namedChildren.values();
	foreach (Concept* e, m_cardinalChildren)
		e->silentMove(_u->back());
	foreach (Concept* e, m_namedChildren.values())
		e->silentMove(_u->middle(e->m_index));

	kill(_u);

	// Signal we're off.
	inLimbo(_u);

	// Tell _r's old context (if it has one) that it has gone, and tell _r that it has a new context.
	_u->parentSwitchedWithChildRemoved(you);

	// Notify about the children having been moved.
	foreach (Concept* i, es)
		i->parentSwitched(this);
	_u->childrenInitialised();

	// Tell _r's new context that it's here instead of us.
	if (_u->m_parent)
		_u->m_parent->childSwitched(_u, this);

	delete this;
	return _u;
}
Concept* Concept::replace(Concept* _r)
{
	Position you = _r->over();

	over().insertSilent(_r);
	kill(_r);

	// Signal we're off.
	inLimbo(_r);
	// Tell _r's old context (if it has one) that it has gone, and tell _r that it has a new context.
	_r->parentSwitchedWithChildRemoved(you);
	// Tell _r's new context that it's here.
	if (_r->m_parent)
		_r->m_parent->childSwitched(_r, this);
	delete this;
	return _r;
}
Concept* Concept::insert(Concept* _e, int _preferedIndex)
{
	Position you = _e->over();

	over().insertSilent(_e);
	Position ip = _preferedIndex == NonePrefered ? _e->firstFor(kind()) : _e->middle(_preferedIndex);
	AssertNR(ip != Nowhere);
	if (ip.allowedToBeKind(kind()))
	{
		// Everything going according to plan.
		// P -n-> this   BECOMES   P -n-> _e -ip.i-> this
		if (ip.exists() && (ip->isPlaceholder() || ip->index() < 0))
			ip->killAndDelete(this);
		silentMove(ip);

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
		inLimbo(_e->child(_preferedIndex));
		killAndDelete(_e->child(_preferedIndex));
	}
	return _e;
}
bool Concept::tryInsert(Concept* _e, int _preferedIndex)
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

	return ip != Nowhere;
}
void Concept::inLimbo(Dependee* _replacement)
{
	foreach (Concept* e, children())
		e->inLimbo();
	oneFootInTheGrave(_replacement);
}

void Concept::deleteAndRefill(Concept* _e)
{
	SafePointer<Concept> c = parent();
	Position p = over();

	if (_e)
	{
		replace(_e);
	}
	else if (isNecessary())
	{
		p.spawnPreparedSilent();
		kill(p.concept());
		inLimbo(p.concept());
		p->parentAdded();
		c->childSwitched(p.concept(), this);
		delete this;
	}
	else
	{
		int ci = m_index;
		kill();
		inLimbo();
		c->childRemoved(this, ci);
		delete this;
	}
}

void Concept::notifyOfStrobe(Concept* _strobeCreation)
{
	if (_strobeCreation && m_parent)
		parentSwitched(_strobeCreation);
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
	}
}

}
