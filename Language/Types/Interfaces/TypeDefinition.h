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

#include <msList.h>
#include <msRgb.h>
using namespace MarttaSupport;

#include "TypeConcept.h"
#include "Labelled.h"
#include "Identifiable.h"
#include "WebViewable.h"

#ifndef M_API_Types
#define M_API_Types M_OUTAPI
#endif

namespace Martta
{

class ValueDefiner;
class Declaration;
class Type;
typedef List<Type> Types;

/**
 * Class for permanent types.
 * Types may be named or anonymous.
 * This includes typedefs, unions, structs and enums.
 */
class M_API_Types TypeDefinition: public_interface Identifiable, public_interface Labelled
{
	MARTTA_NOTION(Identifiable)
	MARTTA_ALSO_INHERITS(Labelled, 0)

public:
	virtual String						code() const = 0;
	virtual Rgb							idColour() const { return Rgb(0x444444); }
	virtual Types						assignableTypes() const = 0;
	virtual List<Declaration*>			utilisedInUse() const = 0;
	virtual bool						hasDefaultConstructor() const = 0;
	virtual bool						hasSingleCastOperator(TypeConcept const* _t, bool _const) const;
	virtual inline bool					hasSingleConversionConstructor(TypeConcept const*) const { return false; }
	virtual bool						defineSimilarityTo(TypeConcept const* _t, TypeConcept::Castability _c) const;
	virtual List<ValueDefiner*>			applicableMembers(Concept const* _s, bool _isConst) const;
	virtual inline bool					canStandAlone() const { return true; }
	virtual inline String				defineLabelHtml(String const& _text) const { return String(L"<span class=\"TypeConcept\" style=\"text-shadow: -1px -1px 1px %1\">").arg(idColour().name()) + WebViewable::composeName(_text, StringList("TypeDefinition")) + L"</span>"; }
	virtual void						apresLoad() { Identifiable::apresLoad(); Labelled::apresLoad(); }

	virtual ~TypeDefinition() {}
};

}

namespace MarttaSupport
{

template<>
class NameTrait< ::Martta::TypeDefinition*>
{
public:
	static String name(::Martta::TypeDefinition* _val) { return _val ? _val->nick() : String::null; }
};

}
