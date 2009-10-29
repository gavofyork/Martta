/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "AuxilliaryRegistrar.h"
#include "Concept.h"
#include "Kind.h"

namespace Martta
{

Concept* Kind::spawn() const
{
	return Concept::spawn(name());
}

Concept* Kind::spawnPrepared() const
{
	return Concept::spawn(name())->prepareChildren();
}

Kinds Kinds::withoutInterfaces() const
{
	Kinds ret;
	foreach (Kind k, *this)
		if (!k.isInterface())
			ret << k;
	return ret;
}

Kinds Kinds::onlyInterfaces() const
{
	Kinds ret;
	foreach (Kind k, *this)
		if (k.isInterface())
			ret << k;
	return ret;
}

Kinds Kinds::onlyPlaceholders() const
{
	Kinds ret;
	foreach (Kind k, *this)
		if (!k.isInterface() && k.isPlaceholder())
			ret << k;
	return ret;
}

Kinds Kinds::onlyObjects() const
{
	Kinds ret;
	foreach (Kind k, *this)
		if (!k.isInterface() && !k.isPlaceholder())
			ret << k;
	return ret;
}

}
