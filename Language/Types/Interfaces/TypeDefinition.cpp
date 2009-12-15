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

#include "Type.h"
#include "ReferencedType.h"
#include "WebStylist.h"
#include "TypeDefinition.h"

namespace Martta
{

MARTTA_NOTION_CPP(TypeDefinition);

String TypeDefinition::defineLabelHtml(String const& _text) const
{
	if (WebStylist::current()->property("CSS", "Simple").toBool())
		return String(L"<span class=\"TypeConcept\">") + WebViewable::composeName(_text, StringList("TypeDefinition")) + L"</span>";
	return String(L"<span class=\"TypeConcept\" style=\"text-shadow: -1px -1px 1px %1\">").arg(idColour().name()) + WebViewable::composeName(_text, StringList("TypeDefinition")) + L"</span>";
}

bool TypeDefinition::hasSingleCastOperator(TypeConcept const* _t, bool) const
{
	return Type(*_t) == Type(const_cast<TypeDefinition*>(this));
}

bool TypeDefinition::defineSimilarityTo(TypeConcept const* _t, TypeConcept::Castability _c) const
{
	if (ReferencedType const* e = _t->tryKind<ReferencedType>())
		if (e->subject() == this)
			return true;
	return _c == TypeConcept::Convertible && hasSingleCastOperator(_t, false);
}

List<ValueDefiner*> TypeDefinition::applicableMembers(Concept const*, bool) const
{
	return List<ValueDefiner*>();
}

}
