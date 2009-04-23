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
#include "SubAddressable.h"

namespace Martta
{

class ModelPtrFace;
class FundamentalOperator;

/**
 * The language root for an entire program.
 *
 * This encompasses everything required to compile a single program. It should include forward declarations of all
 * kinds from all dependencies (including C-style).
 *
 * Aside from that it should also encompass all the implementation of the project in question.
 */
class RootEntity: public QObject, public SubAddressable
{
	Q_OBJECT
	MARTTA_OBJECT(SubAddressable)

public:
	RootEntity();
	~RootEntity();

	static RootEntity*					get() { return s_this; }

	virtual QString						name() const { return QString(); }
	virtual Entity*						context() const { return 0; }
	virtual QString						reference() const { return ""; }
	virtual QString						key() const { return ""; }
	virtual Kinds						allowedKinds(int) const;

	void								checkCull(Entity* _e) { if (!m_cullList.size()) QTimer::singleShot(0, this, SLOT(doCulling())); m_cullList << _e; }

	virtual void						exportDom(QDomElement&) const { M_ASSERT(0); }

	void								archivePtrs() const;
	void								restorePtrs() const;
	void								addModelPtr(ModelPtrFace* _p);
	void								removeModelPtr(ModelPtrFace* _p);
	void								noteDeletion(DeclarationEntity* _e);

	void								setChanged();
	
	enum ArchivalState { Restored, Archiving, Archived, Restoring };
	ArchivalState 						archivalState() const { return m_archivalState; }
	
	void								registerDeclaration(DeclarationEntity* _e) { M_ASSERT(!m_registered.contains(_e->key())); m_registered[_e->key()] = _e; }
	void								unregisterDeclaration(DeclarationEntity* _e) { M_ASSERT(m_registered.contains(_e->key())); m_registered.remove(_e->key()); }
	DeclarationEntity*					findDeclaration(QString const& _key) const { if (m_registered.contains(_key)) return m_registered[_key]; return 0; }
	
public slots:	
	void								ensureSyncedModel();

private slots:
	void								doCulling();
	
signals:
	/// Indicates that some or all of the model has changed and needs reinterpretation.
	void								modelChanged();

private:
	bool								m_changed;
	QHash<QString, DeclarationEntity*>	m_registered;
	QList<ModelPtrFace*>				m_modelPtrs;
	mutable ArchivalState				m_archivalState;

	// List of entities to check and possibly delete at next opportunity when nothing else happening.
	QList<SafePointer<Entity> >			m_cullList;

	static RootEntity*					s_this;
};

}
