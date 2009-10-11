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
 * the License.Projec
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#pragma once

#include <QMainWindow>

#include <Module.h>
#include <Program.h>
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
class Concept;

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
	void on_actNewCModule_triggered();

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

	void entityFocused(Concept* _e);
	void delayedUpdate();

	void resetSubject();

	void stepFinished();
	void programReadyOut();
	void programReadyError();

	void updateProgramCode();

private:
	virtual void paintEvent(QPaintEvent* _p);

	void updateProgramSupportPath();

	void setFilename(QString const& _fn);

	void executeNextStep();
	void loadPlugins();
	void updateLanguage();
	bool confirmLose();

	// Simple load/save functions; doesn't record the filename.
	Module*				loadModule(String const& _filename);
	bool					saveModule(String const& _filename, Module* _p) const;

	// Will save the solution, but not the projects---do that manually.
	// Acts according to m_filename and m_solution.
	// Does nothing is m_filename/m_solution is null.
	// @returns true iff everything went ok.
	bool					saveProgram() const;

	// Loads the solution file @a _filename and any projects that it requires.
	// Maintains correctness of m_projects, m_filename and m_solution accordingly.
	// Doesn't do anything about the GUI though.
	// @returns true iff everything went ok.
	bool					load(String const& _filename);

	// Proper save function---saves everything, prompting for filename if necessary.
	// Maintains correctness of m_projects, m_filename and m_solution accordingly.
	bool					save();

	Concept*					importDom(QDomElement const& _el, Concept* _p, QStringList* _projectstoLoad = 0, QList<Module*>* _projects = 0);
	QDomElement				exportDom(QDomDocument& _doc, Concept const* _e, bool _dump = false) const;

	// Serialises/deserialises the lot.
	QString					serialise() const;
	bool					deserialise(QString const& _s);

	inline QList<Module*>	projects() const { return m_projects.keys(); }
	inline Module*			project() const { return codeView->subject()->tryKind<Module>(); }

	QString					summary(Concept* _e);

	String					m_pluginsPath;

	QList<QLibrary*>		m_libraries;

	QHash<Module*, QString>m_projects;
	QString					m_filename;
	Program*				m_solution;

	QTimer*					m_updateTimer;

	QProcess*				m_buildAndRun;
	List<StringList>		m_steps;
	QString					m_outputOwed;
};

}
