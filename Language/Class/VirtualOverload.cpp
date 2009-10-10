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

#include "Class.h"
#include "Compound.h"
#include "LambdaNamer.h"
#include "AccessLabel.h"
#include "CodeScene.h"
#include "VirtualOverload.h"
#include "CompletionDelegate.h"

namespace Martta
{

MARTTA_PROPER_CPP(VirtualOverride);

Type VirtualOverride::returns() const
{
	return m_base.isUsable() ? m_base->returns() : Type();
}

Kinds VirtualOverride::allowedKinds(int _i) const
{
	if (_i == Identity || _i == Constness || _i == Returned)
		return Kinds();
	return Super::allowedKinds(_i);
}

bool VirtualOverride::keyPressed(KeyEvent const* _e)
{
	if (_e->text().length() == 1 && _e->text()[0].isLower())
	{
		_e->codeScene()->setEditing(this);
		if (_e->codeScene()->isEditing(this))
			_e->reinterpretLater();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

List<VirtualMethod*> VirtualOverride::possibilities() const
{
	List<VirtualMethod*> ret;
	foreach (VirtualMethod* i, parentAs<Class>()->membersOf<VirtualMethod>())
		if (i->parent() != parent())
			ret << i;
		else if (i == this && m_base)
			ret << m_base;
	return ret;
}

static bool s_VirtualOverride_inEdit = false;
static String s_VirtualOverride_inEditReal = String::null;
static String s_VirtualOverride_inEditUnreal = String::null;
static String s_VirtualOverride_inEditComment = String::null;
static VirtualMethod* s_VirtualOverride_inEditMethod = 0;

String VirtualOverride::defineNameHtml() const
{
	if (s_VirtualOverride_inEdit)
	{
		String cap = s_VirtualOverride_inEditReal + L"<span class=\"unreal\">" + s_VirtualOverride_inEditUnreal + L"</span>";
		if (s_VirtualOverride_inEditMethod)
			return stripId(s_VirtualOverride_inEditMethod->defineNameHtml()).replace(s_VirtualOverride_inEditReal + s_VirtualOverride_inEditUnreal, cap);
		return cap + tagOf(L"minor", s_VirtualOverride_inEditComment);
	}
	else
		return m_base ? stripId(m_base->defineNameHtml()) : String::null;
}

String VirtualOverride::defineReturnHtml() const
{
	if (s_VirtualOverride_inEdit)
		return s_VirtualOverride_inEditMethod ? stripId(s_VirtualOverride_inEditMethod->defineReturnHtml()) : String::null;
	return m_base ? stripId(m_base->defineReturnHtml()) : String::null;
}

String VirtualOverride::defineArgListHtml() const
{
	if (s_VirtualOverride_inEdit)
		return s_VirtualOverride_inEditMethod ? stripId(s_VirtualOverride_inEditMethod->defineArgListHtml()) : String::null;
	return m_base ? stripId(m_base->defineArgListHtml()) : String::null;
}

String VirtualOverride::defineEditHtml(CodeScene* _s) const
{
	s_VirtualOverride_inEdit = true;
	s_VirtualOverride_inEditReal = _s->editDelegate(this)->real();
	s_VirtualOverride_inEditUnreal = _s->editDelegate(this)->unreal();
	s_VirtualOverride_inEditComment = _s->editDelegate(this)->comment();
	s_VirtualOverride_inEditMethod = static_cast<CompletionDelegate<VirtualOverride, VirtualMethod*>*>(_s->editDelegate(this))->selection();
	String ret = defineHtml();
	s_VirtualOverride_inEdit = false;
	s_VirtualOverride_inEditReal = String::null;
	s_VirtualOverride_inEditUnreal = String::null;
	s_VirtualOverride_inEditComment = String::null;
	s_VirtualOverride_inEditMethod = 0;
	return ret;
}

EditDelegateFace* VirtualOverride::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<VirtualOverride, VirtualMethod*>(this, _s);
}

}
