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

#include "CodeScene.h"
#include "Const.h"
#include "Reference.h"
#include "TextLabel.h"
#include "Variable.h"
#include "OperatorLabel.h"
#include "ConstLabel.h"
#include "AccessLabel.h"
#include "Class.h"
#include "ArtificialAssignmentOperator.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ArtificialAssignmentOperator);	

Type ArtificialAssignmentOperator::argumentType(int) const
{
	return Type(ancestor<Class>()).topWith(Const()).topWith(Reference());
}

Type ArtificialAssignmentOperator::returns() const
{
	return Type(ancestor<Class>()).topWith(Reference());
}

/*bool ArtificialAssignmentOperator::activated(CodeScene* _s)
{
	Callable* c = new MethodOperator;
	c->prepareChildren();
	c->middle(0)->replace(new OperatorLabel(Operator::Equals));
	returns().placeCopy(c->middle(2));
	c->middle(3)->replace(new AccessLabel(Public));
	c->middle(4)->replace(new ConstLabel(false));
	Variable* v = new Variable;
	v->back().insertSilent(new TextLabel("source"));
	argumentType(0).insertSilentCopy(v->back());
	c->back().insertSilent(v);
	replace(c);
	_s->setViewKey(c, "expanded", true);
	_s->setCurrent(c->body()->entity(0));
	return true;
}*/

}

