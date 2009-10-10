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

#include "ui_MainWindow.h"

class QProcess;
class QDomElement;
class QDomDocument;
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
	void on_actClose_triggered();
	void on_actQuit_triggered();

	void on_actExecute_triggered();
	void on_actNewCProject_triggered();

	void on_actReloadPlugins_triggered();

	void on_actShowDeps_triggered();
	void on_actShowChanges_triggered();
	void on_actListChanges_triggered();
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
	virtual void paintEvent(QPaintEvent* _p);

	void updateSolutionSupportPath();

	void setFilename(QString const& _fn);

	void executeNextStep();
	void loadPlugins();
	void updateLanguage();
	bool confirmLose();

	// Simple load/save functions; doesn't record the filename.
	Project*				loadProject(String const& _filename);
	bool					saveProject(String const& _filename, Project* _p) const;

	// Will save the solution, but not the projects---do that manually.
	// Acts according to m_filename and m_solution.
	// Does nothing is m_filename/m_solution is null.
	// @returns true iff everything went ok.
	bool					saveSolution() const;

	// Loads the solution file @a _filename and any projects that it requires.
	// Maintains correctness of m_projects, m_filename and m_solution accordingly.
	// Doesn't do anything about the GUI though.
	// @returns true iff everything went ok.
	bool					load(String const& _filename);

	// Proper save function---saves everything, prompting for filename if necessary.
	// Maintains correctness of m_projects, m_filename and m_solution accordingly.
	bool					save();

	Entity*					importDom(QDomElement const& _el, Entity* _p, QStringList* _projectstoLoad = 0, QList<Project*>* _projects = 0);
	QDomElement				exportDom(QDomDocument& _doc, Entity const* _e, bool _dump = false) const;

	// Serialises/deserialises the lot.
	QString					serialise() const;
	bool					deserialise(QString const& _s);

	inline QList<Project*>	projects() const { return m_projects.keys(); }
	inline Project*			project() const { return codeView->subject()->tryKind<Project>(); }

	QString					summary(Entity* _e);

	String					m_pluginsPath;

	QList<QLibrary*>		m_libraries;

	QHash<Project*, QString>m_projects;
	QString					m_filename;
	Solution*				m_solution;

	QTimer*					m_updateTimer;

	QProcess*				m_buildAndRun;
	List<StringList>		m_steps;
	QString					m_outputOwed;
};

}
