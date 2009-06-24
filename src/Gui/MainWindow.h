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

#ifndef MARTTAMAINWINDOW_H
#define MARTTAMAINWINDOW_H

#include <QMainWindow>

#include "ui_MainWindow.h"
#include "ModelPtr.h"

class QProcess;

namespace Martta
{

extern int g_typeCount;

class CodeScene;
class Project;
class Function;
class Entity;

class MainWindow: public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* _p = 0, Qt::WindowFlags _f = Qt::Window);
	~MainWindow();

	Project*				project() const { return m_project; }

	bool confirmLose();

private slots:
	void on_actNewCProject_triggered();
	void on_actNewProject_triggered();
	void on_actOpen_triggered();
	void on_actSave_triggered();
	void on_actShowDeps_triggered();
	void on_actShowChanges_triggered();
	void on_actClearChanges_triggered();
	void on_actShowFirstChange_triggered();
	void on_actRemoveFirstChange_triggered();
	void on_actQuit_triggered();
	void on_actAboutMartta_triggered();
	void on_actAboutQt_triggered();
	void showDependenciesMenu(QPoint const&_p);
	void classSelected(QModelIndex const& _i);
	void on_actExecute_triggered();
	void on_actCastability_triggered();
	
	void on_programIn_returnPressed();

	void entityFocused(Entity* _e);
	void delayedUpdate();

	void projectRenamed();
	void updateCode(Function*);
	void saveCode();
	void resetSubject();

	void programReadyOut();
	void programReadyError();
	void programFinished(int _exitCode);

	void updateProgramCode();
	
	void updateLanguage();

private:
//	Workspace				m_workspace;
	Project*				m_project;
	ModelPtr<Function>		m_codeEditFunction;
	QProcess*				m_program;
	QString					m_outputOwed;
	CodeScene*				m_codeScene;
	QTimer*					m_updateTimer;
};

}

#endif
