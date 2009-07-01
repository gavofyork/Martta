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

#include <QTimer>

#include "Operator.h"
#include "Identifiable.h"
#include "ModelPtr.h"
#include "Declaration.h"

namespace Martta
{

class ModelPtrFace;

/**
 * The language root for an entire program.
 *
 * This encompasses everything required to compile a single program. It should include forward declarations of all
 * kinds from all dependencies (including C-style).
 *
 * Aside from that it should also encompass all the implementation of the project in question.
 */
class BasicRoot: public QObject, public Declaration
{
	Q_OBJECT
	MARTTA_OBJECT(Declaration)

public:
	BasicRoot();

	static BasicRoot*					get() { return s_this; }

	virtual QString						name() const { return QString(); }
	virtual Entity*						parent() const { return 0; }
	virtual QString						reference() const { return ""; }
	virtual QString						key() const { return ""; }
	virtual Kinds						allowedKinds(int) const;

	void								checkCull(Entity* _e) { if (!m_cullList.size()) QTimer::singleShot(0, this, SLOT(doCulling())); m_cullList << _e; }

	virtual void						exportDom(QDomElement&) const { M_ASSERT(0); }

	void								setChanged();

public slots:	
	void								ensureSyncedModel();

private slots:
	void								doCulling();
	
signals:
	/// Indicates that some or all of the model has changed and needs reinterpretation.
	void								modelChanged();

private:
	bool								m_changed;

	// List of entities to check and possibly delete at next opportunity when nothing else happening.
	QList<SafePointer<Entity> >			m_cullList;

	static BasicRoot*					s_this;
};

}
