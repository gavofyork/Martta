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

#include "SafePointer.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

class CodeScene;
class KeyEvent;

class M_API_Concept EditDelegateFace
{
public:
	EditDelegateFace(Concept* _e, CodeScene* _s, bool _immediateCommits = false);
	virtual ~EditDelegateFace();

	CodeScene*					codeScene() const;

	virtual String				real() const { return String::null; }
	virtual String				unreal() const { return String::null; }
	virtual String				comment() const { return String::null; }
	virtual void*				current() const { return 0; }
	Concept*						concept() const { return m_subject; }

	void						initialised() { /*m_immediateCommits = !isValid();*/ }
	void						lazyCommit() { if (m_immediateCommits) tryCommit(); }
	void						tryCommit() { if (isValid()) commit(); }

	virtual bool				keyPressed(KeyEvent const*);
	/// Called only once, and only when this will be destroyed but the subject will live.
	virtual void				leavingEditIntact() {}

protected:
	virtual bool				isValid() const { return true; }
	virtual void				commit() {}

private:
	SafePointer<Concept, true>	m_subject;
	CodeScene*					m_codeScene;
	bool						m_immediateCommits;
};

}
