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

#include "Entity.h"
#include "CodeScene.h"
#include "EditDelegate.h"

namespace Martta
{

EditDelegateFace::EditDelegateFace(Entity* _e, CodeScene* _s):
	m_subject			(_e),
	m_codeScene			(_s),
	m_immediateCommits	(_e->isPlaceholder())
{
	m_codeScene->rememberMe(this);
}

EditDelegateFace::~EditDelegateFace()
{
	m_codeScene->forgetMe(this);
	if (m_subject)
		m_subject->checkForCullingLater();
}

CodeScene* EditDelegateFace::codeScene() const
{
	return m_codeScene;
}

bool EditDelegateFace::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == DeleteKey)
		return subject()->keyPressed(_e);
	else if (_e->text() == EscapeKey)
		_e->codeScene()->setEditing(0);
	else
		return false;
	return true;
}

}
