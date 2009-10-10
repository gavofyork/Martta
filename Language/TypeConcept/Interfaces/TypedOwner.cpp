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
#include "TypeNamer.h"
#include "TypedOwner.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(TypedOwner);

Types TypedOwner::allowedTypes(int) const
{
	return Types();
}

Types TypedOwner::deniedTypes(int) const
{
	return Types();
}

Type TypedOwner::effectiveType(int _i) const
{
	if (!self()->childIs<TypeNamer>(_i))
		return Type();
	Type p(self()->childAs<TypeNamer>(_i)->apparentType());
	foreach (Type t, allowedTypes(_i))
		if (p.isSimilarTo(t, TypeConcept::Convertible))
//		{
//			mDebug() << p->code() << " is convertible to " << t->code() << " t is ultnull? " << t->isUltimatelyNull();
//			t->debugTree();
			return t->isUltimatelyNull() ? p : t;
//		}
	return Type();
}

Type TypedOwner::canonicalType(int _i) const
{
	if (!self()->childIs<TypeNamer>(_i))
		return Type();
	return self()->childAs<TypeNamer>(_i)->type();
}

bool TypedOwner::isChildInValidState(int _i) const
{
//	child(_i)->debugTree();
	// Nothing to say about it as it's not typed. If it's supposed to be typed, then the allowed kinds system should pick up the error.
	if (!self()->childIs<TypeNamer>(_i))
		return true;
	foreach (Type i, deniedTypes(_i))
		if (self()->childAs<TypeNamer>(_i)->apparentType() == i)
			return false;
	foreach (Type i, allowedTypes(_i))
		if (!self()->childIs<TypeNamer>(_i))
		{
			self()->debugTree();
			AssertNR(false);
		}
		else
			if (self()->childAs<TypeNamer>(_i)->apparentType().isSimilarTo(i, TypeConcept::Convertible))
				return true;
			else
			{
				mDebug() << self()->childAs<TypeNamer>(_i)->apparentType()->code() << " != " << i->code();
			}
	return false;
}

String TypedOwner::informationHtml() const
{
	int num = max(self()->minRequired(), self()->cardinalChildCount());
	List<int> ai;
	foreach (int i, self()->knownNames())
		if (allowedTypes(i).count())
			ai.append(i);
	for (int i = 0; i < num; ++i)
		if (allowedTypes(i).count())
			ai.append(i);
	if (ai.count() || allowedTypes(num).count())
	{
		Pairs p(L"Typed Children");
		foreach (int i, ai)
		{
			Pairs q(self()->indexName(i) + L": " + escape(effectiveType(i)->name()));
			if (canonicalType(i) != effectiveType(i))
				q << "Canonical type" << escape(canonicalType(i)->name());
			q << "Allowed types" << escape(compileTypes(allowedTypes(i)));
			if (deniedTypes(i).count())
				q << "Denied types" << escape(compileTypes(deniedTypes(i)));
			p << q;
		}
		if (allowedTypes(num).count())
			p << L"..." << escape(compileTypes(allowedTypes(num)));
		return p;
	}
	return String::null;
}

}
