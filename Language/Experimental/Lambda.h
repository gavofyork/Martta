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

#pragma once

#include "Composite.h"
#include "LambdaNamer.h"
#include "Typed.h"

#ifndef M_API_Experimental
#define M_API_Experimental M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Experimental ClosureEntry: public_super Concept, public_interface NameEntryPoint, public_interface ScopePreserver
{
	MARTTA_PLACEHOLDER(Concept)
	MARTTA_ALSO_INHERITS(NameEntryPoint, 0)
	MARTTA_ALSO_INHERITS(ScopePreserver, 1)

public:
	void								committed(Named* _n, CodeScene* _cs) { NameEntryPoint::committed(_n, _cs); }

protected:
	virtual EditDelegateFace*			newDelegate(CodeScene* _s) { return NameEntryPoint::newDelegate<ClosureEntry>(_s); }
	virtual bool						keyPressed(KeyEvent const* _e)
	{
		if (NameEntryPoint::keyPressed(_e))
			return true;
		return Super::keyPressed(_e);
	}
};

class M_CLASS M_API_Experimental ClosureDefault: public_super ClosureEntry
{
	MARTTA_PLACEHOLDER(ClosureEntry)
};

class M_CLASS M_API_Experimental ClosureDefaultValue: public_super ClosureDefault
{
	MARTTA_PROPER(ClosureDefault)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<ClosureDefaultValue>(_p, _e, "@="); }

protected:
	virtual String						defineHtml() const { return mark() + tagOf("symbol", "="); }
};

class M_CLASS M_API_Experimental ClosureDefaultReference: public_super ClosureDefault
{
	MARTTA_PROPER(ClosureDefault)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<ClosureDefaultReference>(_p, _e, "@&"); }

protected:
	virtual String						defineHtml() const { return mark() + tagOf("symbol", "&"); }
};

// Refers to an existant variable.
class M_CLASS M_API_Experimental ClosureExplicit: public_super ClosureEntry
{
	MARTTA_PROPER(ClosureEntry)

public:
	MARTTA_NAMED(Variable)

	ClosureExplicit(ValueDefiner* _v = 0);

protected:
	virtual int							minRequired(int _i) const { return _i == Variable ? 1 : 0; }
	virtual Kinds						allowedKinds(int _i) const;
	virtual String						preHtml() const { return L""; }
	virtual String						defineHtml() const { return preHtml() + toHtml(child(Variable)); }
	virtual bool						isSuperfluous() const { return !childAs<ReferencedValue>(Variable)->subject(); }
};

class M_CLASS M_API_Experimental ClosureExplicitValue: public_super ClosureExplicit
{
	MARTTA_PROPER(ClosureExplicit)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<ClosureExplicitValue>(_p, _e, "="); }

protected:
	virtual String						preHtml() const { return tagOf(L"symbol", L"="); }
	virtual Concept*					onDecay() const { return new ClosureDefaultValue; }
};

class M_CLASS M_API_Experimental ClosureExplicitReference: public_super ClosureExplicit
{
	MARTTA_PROPER(ClosureExplicit)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<ClosureExplicitReference>(_p, _e, "&"); }

protected:
	virtual String						preHtml() const { return tagOf(L"symbol", L"&"); }
	virtual Concept*					onDecay() const { return new ClosureDefaultReference; }
};

class M_CLASS M_API_Experimental Closure: public_super Concept, public_interface WebViewable, public_interface ScopePreserver
{
	MARTTA_PROPER(Concept)
	MARTTA_ALSO_INHERITS(WebViewable, 0)
	MARTTA_ALSO_INHERITS(ScopePreserver, 1)

public:
	Concept*							firstChild()
	{
		if (childCount() == 0)
		{
			ClosureEntry* v = new ClosureEntry;
			self()->back().place(v);
			v->prepareChildren();
		}
		return child(0);
	}

protected:
	virtual String						defineHtml() const { return mark() + tagOf("minor symbol", "[") + toHtml(self()->cardinalChildren(), tagOf(L"minor symbol", L", ")) + tagOf(L"minor symbol", L"]"); }
	virtual int							minRequired(int) const { return 0; }
	virtual Kinds						allowedKinds(int _i) const { return _i >= 0 ? Kind::of<ClosureEntry>() : Kinds(); }
	virtual bool						keyPressed(KeyEvent const* _e)
	{
		if (_e->text() == L"," && _e->focalIndex() >= 0)
		{
			ClosureEntry* v = new ClosureEntry;
			((_e->focalIndex() < 0 || _e->focalIndex() == UndefinedIndex) ? self()->back() : self()->middle(_e->focalIndex() + (_e->isInserting() ? 0 : 1))).place(v);
			v->prepareChildren();
			_e->codeScene()->navigateInto(v);
		}
		else
			return Super::keyPressed(_e);
		return true;
	}
};

class M_CLASS M_API_Experimental Lambda: public_super Typed, public_interface LambdaNamer, public_interface Composite
{
	MARTTA_PROPER(Typed)
	MARTTA_ALSO_INHERITS(LambdaNamer, 0)
	MARTTA_ALSO_INHERITS(Composite, 1)
	MARTTA_NAMED(ClosureSet)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual Type						type() const;
	virtual String						defineHtml() const;
	virtual int							minRequired(int _i) const;
	virtual Kinds						allowedKinds(int _i) const;
	virtual Types						allowedTypes(int _i) const { return LambdaNamer::allowedTypes(_i); }
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual void						compose();
};

}
