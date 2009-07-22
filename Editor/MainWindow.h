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

#include <QMainWindow>

#include <Project.h>
#include <Solution.h>
#include <CullManager.h>
#undef inline

#include "ui_MainWindow.h"

class QProcess;
class QLibrary;

namespace Martta
{

class CullActor: public QObject, public DelayedActor
{
	Q_OBJECT

public:
	CullActor(QObject* _parent): QObject(_parent), m_primed(false) {}

	virtual void prime();

private slots:
	void doCulling();

private:
	bool m_primed;
};

class CodeScene;
class Function;
class Entity;



class MainWindow: public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* _p = 0, Qt::WindowFlags _f = Qt::Window);
	~MainWindow();

private slots:
	void on_actNew_triggered();
	void on_actOpen_triggered();
	void on_actSave_triggered();
	void on_actQuit_triggered();

	void on_actExecute_triggered();
	void on_actNewCProject_triggered();

	void on_actShowDeps_triggered();
	void on_actShowChanges_triggered();
	void on_actClearChanges_triggered();
	void on_actShowFirstChange_triggered();
	void on_actRemoveFirstChange_triggered();
	void on_actCastability_triggered();

	void on_actAboutMartta_triggered();
	void on_actAboutQt_triggered();

	void on_programIn_returnPressed();

	void entityFocused(Entity* _e);
	void delayedUpdate();

	void resetSubject();

	void stepFinished();
	void programReadyOut();
	void programReadyError();

	void updateProgramCode();

private:
	void updateSolutionSupportPath();

	void executeNextStep();
	void loadPlugins();
	void updateLanguage();
	bool confirmLose();

	inline List<Project*>	projects() const { return m_solution ? m_solution->self()->cardinalChildrenAs<Project>() : List<Project*>(); }
	inline Project*			project() const { return codeView->subject()->tryKind<Project>(); }

	QList<QLibrary*>		m_libraries;

	Solution*				m_solution;

	QTimer*					m_updateTimer;

	QProcess*				m_buildAndRun;
	List<StringList>		m_steps;
	QString					m_outputOwed;
};

}
