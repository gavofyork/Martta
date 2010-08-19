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

namespace Martta
{

class ClosureEntry: public_super Concept, public_interface NameEntryPoint
{
	MARTTA_PLACEHOLDER(Concept)
	MARTTA_ALSO_INHERITS(NameEntryPoint, 0)

protected:
	virtual bool						keyPressed(KeyEvent const* _e)
	{
		if (NameEntryPoint::keyPressed(_e))
			return true;
		return Super::keyPressed(_e);
	}

};

// Note these two can morph into ClosureExplicit...
class ClosureDefaultValue: public_super ClosureEntry
{
	MARTTA_PROPER(ClosureEntry)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<ClosureDefaultValue>(_p, _e, "="); }

protected:
	virtual String						defineHtml() const { return mark() + tagOf("symbol", "="); }
};

class ClosureDefaultReference: public_super ClosureEntry
{
	MARTTA_PROPER(ClosureEntry)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<ClosureDefaultReference>(_p, _e, "&"); }

protected:
	virtual String						defineHtml() const { return mark() + tagOf("symbol", "&"); }
};
#if 0
// Refers to an existent variable.
class ClosureExplicit: public_super ClosureEntry
{
	MARTTA_PLACEHOLDER(ClosureEntry)
};

class ClosureExplicitValue: public_super ClosureExplicit
{
	MARTTA_PROPER(ClosureExplicit)
};

class ClosureExplicitReference: public_super ClosureExplicit
{
	MARTTA_PROPER(ClosureExplicit)
};
#endif
class Closure: public_super Concept, public_interface WebViewable
{
	MARTTA_PROPER(Concept)
	MARTTA_ALSO_INHERITS(WebViewable, 0)

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
	virtual String						defineHtml() const { return mark() + tagOf("symbol", "[") + toHtml(self()->cardinalChildren(), L"<span class=\"symbol\">, </span>") + tagOf("symbol", "]"); }
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

class Lambda: public_super Typed, public_interface LambdaNamer, public_interface Composite
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
