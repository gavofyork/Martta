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

#include <QtXml>

#include "Class.h"
#include "Compound.h"
#include "LambdaNamer.h"
#include "AccessLabel.h"
#include "CodeScene.h"
#include "VirtualOverload.h"
#include "CompletionDelegate.h"

namespace Martta
{

MARTTA_OBJECT_CPP(VirtualOverload);

Type VirtualOverload::returns() const
{
	return m_base.isUsable() ? m_base->returns() : Type();
}
	
QString VirtualOverload::memberLambdaDefineLayout(ViewKeys& _viewKeys) const
{
	return ("yminor;'VIRTUAL';Mo;>name;ycode;'" + (m_base ? m_base->asKind<LambdaNamer>()->basicCode(LambdaNamer::InsideScope) : QString("[]")) + "';Mo" + QString(_viewKeys["expanded"].toBool() ? body()->entities().size() ? ";n;i;%1" : ";%1" : "")).arg(fromLocal(0));
}

Kinds VirtualOverload::memberAllowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<Compound>();
	return Kinds();
}

bool VirtualOverload::keyPressed(EntityKeyEvent const* _e)
{
	if (QRegExp("[a-z]").exactMatch(_e->text()))
	{
		_e->reinterpretLater();
		setEditing(_e->codeScene());
	}
	else
		return Super::keyPressed(_e);
	return true;
}

void VirtualOverload::exportDom(QDomElement& _element) const
{
	Super::exportDom(_element);
	_element.setAttribute("base", m_base->key());
}

void VirtualOverload::importDom(QDomElement const& _element)
{
	Super::importDom(_element);
	m_base = locateEntity<VirtualMethod>(_element.attribute("base"));
}

QList<VirtualMethod*> VirtualOverload::possibilities() const
{
	QList<VirtualMethod*> ret;
	foreach (VirtualMethod* i, contextAs<Class>()->membersOf<VirtualMethod>())
		if (i->context() != context())
			ret << i;
	return ret;
}

QString VirtualOverload::defineEditLayout(ViewKeys& _viewKeys, VirtualMethod*) const
{
	// having the margin here is horrible, but it'll do for now
	return "m24,0,0,0;^;ycode;'virtual';Mo;>name;ynormal;%1;Mo" + QString(_viewKeys["expanded"].toBool() ? (body()->entities().size() ? ";n;i;" : ";") + QString::number(fromLocal(0)) : "");
}

EditDelegateFace* VirtualOverload::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<VirtualOverload, VirtualMethod*>(this, _s);
}

}
