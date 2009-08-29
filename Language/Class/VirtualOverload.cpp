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

MARTTA_OBJECT_CPP(VirtualOverload);

Type VirtualOverload::returns() const
{
	return m_base.isUsable() ? m_base->returns() : Type();
}

Kinds VirtualOverload::allowedKinds(int _i) const
{
	if (_i == Identity || _i == Constness || _i == Returned)
		return Kinds();
	return Super::allowedKinds(_i);
}

bool VirtualOverload::keyPressed(KeyEvent const* _e)
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

List<VirtualMethod*> VirtualOverload::possibilities() const
{
	List<VirtualMethod*> ret;
	foreach (VirtualMethod* i, parentAs<Class>()->membersOf<VirtualMethod>())
		if (i->parent() != parent())
			ret << i;
		else if (i == this && m_base)
			ret << m_base;
	return ret;
}

static bool s_VirtualOverload_inEdit = false;
static String s_VirtualOverload_inEditReal = String::null;
static String s_VirtualOverload_inEditUnreal = String::null;
static String s_VirtualOverload_inEditComment = String::null;
static VirtualMethod* s_VirtualOverload_inEditMethod = 0;

String VirtualOverload::defineNameHtml() const
{
	if (s_VirtualOverload_inEdit)
	{
		String cap = s_VirtualOverload_inEditReal + L"<span class=\"unreal\">" + s_VirtualOverload_inEditUnreal + L"</span>";
		if (s_VirtualOverload_inEditMethod)
			return stripId(s_VirtualOverload_inEditMethod->defineNameHtml()).replace(s_VirtualOverload_inEditReal + s_VirtualOverload_inEditUnreal, cap);
		return cap + tagOf(L"minor", s_VirtualOverload_inEditComment);
	}
	else
		return m_base ? stripId(m_base->defineNameHtml()) : String::null;
}

String VirtualOverload::defineReturnHtml() const
{
	if (s_VirtualOverload_inEdit)
		return s_VirtualOverload_inEditMethod ? stripId(s_VirtualOverload_inEditMethod->defineReturnHtml()) : String::null;
	return m_base ? stripId(m_base->defineReturnHtml()) : String::null;
}

String VirtualOverload::defineArgListHtml() const
{
	if (s_VirtualOverload_inEdit)
		return s_VirtualOverload_inEditMethod ? stripId(s_VirtualOverload_inEditMethod->defineArgListHtml()) : String::null;
	return m_base ? stripId(m_base->defineArgListHtml()) : String::null;
}

String VirtualOverload::defineEditHtml(CodeScene* _s) const
{
	s_VirtualOverload_inEdit = true;
	s_VirtualOverload_inEditReal = _s->editDelegate(this)->real();
	s_VirtualOverload_inEditUnreal = _s->editDelegate(this)->unreal();
	s_VirtualOverload_inEditComment = _s->editDelegate(this)->comment();
	s_VirtualOverload_inEditMethod = static_cast<CompletionDelegate<VirtualOverload, VirtualMethod*>*>(_s->editDelegate(this))->selection();
	String ret = defineHtml();
	s_VirtualOverload_inEdit = false;
	s_VirtualOverload_inEditReal = String::null;
	s_VirtualOverload_inEditUnreal = String::null;
	s_VirtualOverload_inEditComment = String::null;
	s_VirtualOverload_inEditMethod = 0;
	return ret;
}

EditDelegateFace* VirtualOverload::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<VirtualOverload, VirtualMethod*>(this, _s);
}

}
