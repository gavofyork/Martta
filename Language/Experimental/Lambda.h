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
#include "AssignedVariable.h"
#include "BuiltinType.h"
#include "Typed.h"
#include "TopLevel.h"
#include "TextLabel.h"
#include "ReferencedType.h"

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
	virtual String						defineHtml() const { return mark() + preHtml() + toHtml(child(Variable)); }
	virtual bool						usurpsChild(Concept const* _c) const { return _c == child(Variable); }
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
	virtual String						defineHtml() const { return tagOf("minor symbol", "[") + toHtml(list_cast<Concept*>(cardinalChildrenOf<ClosureDefault>()), tagOf(L"minor symbol", L", "), L"", true) + toHtml(filterConceptsInv<ClosureDefault>(cardinalChildren()), tagOf(L"minor symbol", L", ")) + tagOf(L"minor symbol", L"]"); }
	virtual int							minRequired(int) const { return 0; }
	virtual Kinds						allowedKinds(int _i) const { return _i >= 0 ? Kind::of<ClosureEntry>() : Kinds(); }
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencySwitched(Concept* _e, Concept*) { if (_e->isKind<ClosureDefault>()) foreach (ClosureDefault* c, cardinalChildrenOf<ClosureDefault>()) if (c != _e) c->deleteAndRefill(); }
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

	virtual bool						isInValidState() const { return isWellDefined() && Super::isInValidState(); }
	virtual Type						returns() const;

protected:
	virtual Type						type() const;
	virtual String						defineHtml() const;
	virtual int							minRequired(int _i) const;
	virtual Kinds						allowedKinds(int _i) const;
	virtual Types						allowedTypes(int _i) const { return LambdaNamer::allowedTypes(_i); }
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual void						compose();
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyChanged(int, Concept*) { changed(Logically); }
	virtual void						onDependencyRemoved(Concept*, int) { changed(Logically); }
};

class M_CLASS M_API_Types LambdaType: public_super InvocableType
{
	MARTTA_PROPER(InvocableType)

public:
	LambdaType(Lambda* _l = 0): m_subject(0) { setDependency(m_subject, _l); }

	virtual bool						isUltimatelyNull() const { return false; }
	virtual Type						returnType() const { return m_subject ? m_subject->returns() : Super::returnType(); }
	virtual Type						argumentType(int _i) const { return m_subject ? m_subject->argumentType(_i) : Super::argumentType(_i); }
	virtual int							minimumArgCount() const { return m_subject ? m_subject->argumentCount() : Super::minimumArgCount(); }
	virtual bool						hasArgumentAt(int _i) const { return m_subject ? _i < m_subject->argumentCount() : Super::hasArgumentAt(_i); }
	virtual bool						isWellDefined() const { return m_subject && m_subject->isInValidState(); }
	virtual bool						canStandAlone() const { return true; }

private:
//	virtual Types						assignableTypes() const;
	virtual String						code(String const& _middle) const { return String(L"\u03BB(%1)").arg((long)&*m_subject) + _middle; }
	virtual TypeConcept*				newClone() const { return new LambdaType(m_subject); }

private:
	ModelPtr<Lambda> m_subject;
};

class M_CLASS M_API_Experimental AutoType: public_super TypeConcept, public_super_interface Declarable
{
	MARTTA_INITIALISED_PROPER(TypeConcept)
	MARTTA_ALSO_INHERITS(Declarable, 0)

public:
	static void							initialiseClass();
	static void							finaliseClass();

	virtual String						defineHtml() const { return mark() + tagOf("TypeConcept", typeHtml("auto")); }
	virtual String						code(String const& _middle) const { return L"???" + _middle; }

protected:
	virtual TypeConcept*				newClone() const { return new AutoType; }
	virtual Type						declaredType(Type const& _t) const;
};

class M_CLASS M_API_Experimental CommentedOut: public_super Statement
{
	MARTTA_INITIALISED_PROPER(Statement)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<CommentedOut>(_p, _e, L"#"); }
	virtual bool						requiresSemicolon() const { return false; }
	virtual bool						isInValidState() const { return true; }
	virtual bool						isSuperfluous() const { return !cardinalChildCount(); }
	virtual Kinds						allowedKinds(int _i) const { return (_i >= 0) ? Kind::of<Statement>() : Kinds(); }
	virtual String						code() const { return String::null; }
	virtual String						defineHtml() const { return mark() + tagOf("Comment", tagOf(L"minor symbol", L"// ") + toHtml(cardinalChildren()), "div style=\"border: 0; padding: 4px;\""); }
};

class M_CLASS M_API_Experimental TypeArgument: public_super TypeDefinition, public_interface WebViewable
{
	MARTTA_PROPER(TypeDefinition)
	MARTTA_ALSO_INHERITS(WebViewable, 0)

public:
	MARTTA_NAMED(Default)

	// TYPE FORWARDING...
	virtual Rgb							idColour() const { return defaultType()->idColour(); }
	virtual Types						assignableTypes() const { return defaultType()->assignableTypes(); }
	virtual List<Declaration*>			utilisedInUse() const { return defaultType()->utilised(); }
	virtual bool						hasDefaultConstructor() const { return defaultType()->hasDefaultConstructor(); }
	virtual bool						hasSingleCastOperator(TypeConcept const* _t, bool _const) const { return false; }	// TODO: FIX.
	virtual bool						hasSingleConversionConstructor(TypeConcept const* _t) const { return false; }		// TODO: FIX.
	virtual bool						isCallable(Type* _t = 0, bool _isConst = false) const { return defaultType()->isCallable(_t, _isConst); }
	virtual bool						defineSimilarityTo(TypeConcept const* _t, TypeConcept::Castability _c) const { return defaultType()->defineSimilarityTo(_t, _c); }
	virtual List<ValueDefiner*>			applicableMembers(Concept const* _s, bool _isConst) const { return defaultType()->applicableMembers(_s, _isConst); }
	virtual bool						canStandAlone() const { return defaultType()->canStandAlone(); }

	TypeConcept*						defaultType() const { return childAs<TypeConcept>(Default); }

	virtual int							minRequired(int _i) const { return (_i == Identity || _i == Default) ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const { return (_i == Identity) ? Kind::of<TextLabel>() : (_i == Default) ? Kind::of<TypeConcept>() : Super::allowedKinds(_i); }
//	virtual bool						usurpsChild(Concept const* _c) const { return _c == child(Identity); }
	virtual String						code() const { return L"typename " + childAs<IdLabel>(Identity)->code(); }
	virtual String						defineHtml() const { return mark() + tagOf(L"minor", L"typename ") + toHtml(child(Identity)) + L" = " + toHtml(child(Default)); }
};

class M_CLASS M_API_Experimental Template: public_super TopLevelType
{
	MARTTA_PROPER(TopLevelType)

public:
	MARTTA_NAMED(Enclosure)

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<Template>(_p, _e, L"T"); }
	virtual bool						isInValidState() const { return true; }
	virtual int							minRequired(int _i) const { return (_i == Cardinals) ? 1 : (_i == Enclosure) ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const { return (_i >= 0) ? Kind::of<TypeArgument>() : (_i == Enclosure) ? Kind::of<TopLevelType>() : Super::allowedKinds(_i); }
	virtual String						interfaceCode() const;
	virtual String						implementationCode() const;
	virtual String						defineHtml() const { return mark() + tagOf(L"minor", L"TEMPLATE<") + toHtml(cardinalChildren(), L",") + tagOf(L"minor", L">") + tagOf(L"", toHtml(child(Enclosure)), L"div"); }
	virtual List<Declaration*>			utilised() const { return childAs<Declaration>(Enclosure)->utilised(); }
};

}
