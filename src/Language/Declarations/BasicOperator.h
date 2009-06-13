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

#include "Operator.h"
#include "LambdaNamer.h"
#include "TopLevel.h"

namespace Martta
{

class BasicOperator: public TopLevel, public_interface LambdaNamer
{
	MARTTA_OBJECT(TopLevel)
	MARTTA_INHERITS(LambdaNamer, 0)

public:
	virtual Operator							id() const;

protected:	
	virtual Kinds								allowedKinds(int _i) const;
	virtual int									minimumRequired() const;
	virtual int									argumentCount() const;
	virtual Type								argumentType(int _i) const;
	
	virtual QString								interfaceCode() const { return LambdaNamer::interfaceCode(); }
	virtual QString								implementationCode() const { return LambdaNamer::implementationCode(); }

	virtual QString								defineLayout(ViewKeys& _v) const { return "^;" + LambdaNamer::defineLayout(_v); }
	virtual bool								keyPressed(EntityKeyEvent const* _e) { return LambdaNamer::keyPressed(_e) ? true : Super::keyPressed(_e); }
	virtual int									familyDependencies() const { return DependsOnChildren; }
	virtual void								onDependencyChanged(Entity*) { changed(); }
	virtual void								onDependencyRemoved(Entity*) { changed(); }
	virtual Entity*								isExpander() const;
		
private:
	/// Safe to call within allowedKinds/minimumRequired (i.e. to determine isComplete()).
	virtual bool								isBinary() const;
};

}
