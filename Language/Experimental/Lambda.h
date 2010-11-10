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
#include "Class.h"

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
			back().place(v);
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
			((_e->focalIndex() < 0 || _e->focalIndex() == UndefinedIndex) ? back() : middle(_e->focalIndex() + (_e->isInserting() ? 0 : 1))).place(v);
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
	virtual Rgb							idColour() const { return defaultType() ? defaultType()->idColour() : Super::idColour(); }
	virtual Types						assignableTypes() const { return defaultType() ? defaultType()->assignableTypes() : Types(); }
	virtual List<Declaration*>			utilisedInUse() const { return defaultType() ? defaultType()->utilised() : List<Declaration*>(); }
	virtual bool						hasDefaultConstructor() const { return defaultType() && defaultType()->hasDefaultConstructor(); }
	virtual bool						hasSingleCastOperator(TypeConcept const* _t, bool _const) const { return false; }	// TODO: FIX.
	virtual bool						hasSingleConversionConstructor(TypeConcept const* _t) const { return false; }		// TODO: FIX.
	virtual bool						isCallable(Type* _t = 0, bool _isConst = false) const { return defaultType() && defaultType()->isCallable(_t, _isConst); }
	virtual bool						defineSimilarityTo(TypeConcept const* _t, TypeConcept::Castability _c) const { return defaultType() && defaultType()->defineSimilarityTo(_t, _c); }
	virtual List<ValueDefiner*>			applicableMembers(Concept const* _s, bool _isConst) const { return defaultType() ? defaultType()->applicableMembers(_s, _isConst) : List<ValueDefiner*>(); }
	virtual bool						canStandAlone() const { return !defaultType() || defaultType()->canStandAlone(); }

	virtual Identifiable*				addressableContext() const { return 0; }

	TypeConcept*						defaultType() const { return tryChild<TypeConcept>(Default); }

	virtual int							minRequired(int _i) const { return (_i == Identity || _i == Default) ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const { return (_i == Identity) ? Kind::of<TextLabel>() : (_i == Default) ? Kind::of<TypeConcept>() : Super::allowedKinds(_i); }
	virtual String						code() const { return L"typename " + childAs<IdLabel>(Identity)->code(); }
	virtual String						defineHtml() const { return mark() + tagOf(L"minor", L"typename ") + toHtml(child(Identity)) + L" = " + toHtml(child(Default)); }
};

class M_CLASS M_API_Experimental TemplateParameters: public_super Concept, public_super WebViewable
{
	MARTTA_PROPER(Concept)
	MARTTA_ALSO_INHERITS(WebViewable, 0)

public:
	String								preamble() const;
	String								refPostamble() const;
	virtual int							minRequired(int _i) const { return (_i == Cardinals) ? 1 : 0; }
	virtual Kinds						allowedKinds(int _i) const { return (_i >= 0) ? Kinds() << Kind::of<TypeArgument>() /*<< Kind::of<Argument>()*/ : Kinds(); }
	virtual String						defineHtml() const { return tagOf(L"minor", L"<") + toHtml(cardinalChildren(), L",") + tagOf(L"minor", L">"); }
};

class M_CLASS M_API_Experimental Template: public_super_interface WebViewable
{
	MARTTA_NOTION(WebViewable)

public:
	MARTTA_NAMED(Parameters)

	Concept*							instantiation(List<Concept*> const& _params) const;
	template<class T> T*				instantiationAs(List<Concept*> const& _params) const { return instantiation(_params)->asKind<T>(); }

	TypeArgument*						argument(int _i) const { return child(Parameters)->tryChild<TypeArgument>(_i); }
	int									argumentCount() const { return child(Parameters)->cardinalChildCount(); }

	virtual int							minRequired(int _i) const { return (_i == Parameters) ? 1 : 0; }
	virtual Kinds						allowedKinds(int _i) const { return (_i == Instantiations) ? Kind::of<Concept>() : (_i == Parameters) ? Kind::of<TemplateParameters>() : Kinds(); }
	virtual String						preamble() const { return childAs<TemplateParameters>(Parameters)->preamble(); }
	virtual String						refPostamble() const { return childAs<TemplateParameters>(Parameters)->refPostamble(); }
	virtual String						templateHtml(String const& _enclosed) const { return tagOf(L"", tagOf(L"minor", L"TEMPLATE") + toHtml(child(Parameters)), L"div") + _enclosed; }

private:
	MARTTA_NAMED(Instantiations)
	// TODO: Eventually be able to usefully use Concept* as key.
	mutable Hash<List<Type>, Concept*> m_instantiations;
};

class M_CLASS M_API_Experimental TemplatedTypeDefinition: public_super_interface Template, public_interface TypeDefinition
{
	MARTTA_NOTION(Template)

public:
	virtual String						rawReference() const { return reference(); }
};

class M_CLASS M_API_Experimental TemplateClass: public_super Class, public_interface TemplatedTypeDefinition
{
	MARTTA_PROPER(Class)
	MARTTA_ALSO_INHERITS(TemplatedTypeDefinition, 0)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<TemplateClass>(_p, _e, L"T"); }

	Class*								instantiation(List<Concept*> _params) const { return instantiationAs<Class>(_params); }
	virtual String						rawReference() const { return Super::reference(); }

	virtual bool						isInValidState() const { return Super::isInValidState(); }
	virtual int							minRequired(int _i) const { return max(Template::minRequired(_i), Super::minRequired(_i)); }
	virtual Kinds						allowedKinds(int _i) const { return Template::allowedKinds(_i) + Super::allowedKinds(_i); }
	virtual String						interfaceCode() const { return Template::preamble() + Super::interfaceCode(); }
	virtual String						implementationCode() const { return memberedImplementationCode(Template::preamble()); }
	virtual String						defineHtml() const { return templateHtml(Super::defineHtml()); }
	virtual List<Declaration*>			utilised() const { return Super::utilised(); }
	virtual String						reference() const { return Super::reference() + refPostamble(); }
};

class M_CLASS M_API_Experimental TemplatedReferencedType: public_super ReferencedType
{
	MARTTA_PROPER(ReferencedType)

	friend class TypeDefinition;
	friend class ReferencedTypeSet;

public:
	TemplatedReferencedType(TemplatedTypeDefinition* _subject = 0): ReferencedType(0) { set(_subject); }

	TemplatedTypeDefinition*			subject() const { return m_subject->tryKind<TemplatedTypeDefinition>(); }
	TemplatedTypeDefinition*			get() const { return m_subject->tryKind<TemplatedTypeDefinition>(); }
	void								set(TemplatedTypeDefinition* _subject) { setDependency(m_subject, _subject); }

	List<TemplatedTypeDefinition*>		possibilities() const;

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

	Type								asCallableType(bool _isConst = false) const { Type t; if (m_subject) instantiation()->isCallable(&t, _isConst); return t; }

	TypeDefinition*						instantiation() const
	{
		return get() ? get()->instantiationAs<TypeDefinition>(cardinalChildren()) : 0;
//		return get();
/*		if (m_isDirty && child(Instantiation))
			child(Instantiation)->killAndDelete();
		if (!child(Instantiation) && get())
		{
			const_cast<TemplatedReferencedType*>(this)->middle(Instantiation).insertSilent(get()->instantiation(cardinalChildren()));
			m_isDirty = false;
		}
		return tryChild<TypeDefinition>(Instantiation);*/
	}

	virtual int							minRequired(int _i) const { return _i == Cardinals ? get() ? get()->argumentCount() : 0 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const { return _i == Cardinals && (!get() || _i < get()->argumentCount()) ? Kind::of<TypeConcept>() : Super::allowedKinds(_i); }

protected:
	virtual bool						hasDefaultConstructor() const { return instantiation() ? instantiation()->hasDefaultConstructor() : false; }
	virtual Types						assignableTypes() const { return instantiation() ? instantiation()->assignableTypes() : Types(); }
	virtual List<ValueDefiner*>			applicableMembers(Concept const* _s = 0, bool _isConst = false) const { return instantiation() ? instantiation()->applicableMembers(_s, _isConst) : List<ValueDefiner*>(); }
	virtual bool						canStandAlone() const { return instantiation() && instantiation()->canStandAlone(); }
	virtual bool						isCastableTo(TypeConcept const* _t, bool _const) { return instantiation() && instantiation()->hasSingleCastOperator(_t, _const); }
	virtual bool						isCallable(Type* _t = 0, bool _c = false) const { return instantiation() && instantiation()->isCallable(_t, _c); }
	virtual TypeConcept*				newClone() const { return new TemplatedReferencedType(get()); }

	virtual bool						contentsEquivalentTo(TypeConcept const* _t) const { if (TemplatedReferencedType const* e = _t->tryKind<TemplatedReferencedType>()) if (e->m_subject == m_subject) { /*TODO: check children's type equivalence*/ return true; } return false; }
	virtual bool						defineSimilarityTo(TypeConcept const* _t, Castability _c) const { return (instantiation() && instantiation()->defineSimilarityTo(_t, _c)) || Super::defineSimilarityTo(_t, _c); }
	virtual bool						defineSimilarityFrom(TypeConcept const* _f, Castability _c) const { return (instantiation() && _c == Convertible && instantiation()->hasSingleConversionConstructor(_f)) || Super::defineSimilarityFrom(_f, _c); }

	virtual bool						isNull() const { return !m_subject.isUsable(); }

	String								templatePostamble() const;

	virtual String						code(String const& _middle) const { return (get() ? get()->rawReference() : "") + templatePostamble() + _middle; }
	virtual List<Declaration*>			utilised() const { return instantiation() ? instantiation()->utilisedInUse() : Super::utilised(); }	// TODO: define for other types.

	virtual Rgb							idColour() const { return instantiation() ? instantiation()->idColour() : Rgb(0x444444); }
	virtual String						defineHtml() const;
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual String						defineEditHtml(CodeScene* _cs) const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
//	virtual bool						isSuperfluous() const;

	virtual void						apresLoad() { m_isDirty = true; Super::apresLoad(); }
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"subject"] = m_subject.key(); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_subject.restoreFrom(_p[L"subject"]); }

	// TODO: mark dirty when changed.
	virtual int							familyDependencies() const { return DependsOnChildren; }
	virtual void						onDependencyRemoved(Concept* _s, int) { validifyChildren(); changed(); }
	virtual void						onDependencySwitched(Concept* _s, Concept* _o) { validifyChildren(); changed(); }
	virtual void						onDependencyChanged(int, Concept* _s) { validifyChildren(); changed(); }

	mutable bool m_isDirty;
};

}
