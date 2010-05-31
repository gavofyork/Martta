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

#include <QtGui>
#include <QtXml>
#include <QWebFrame>
#include <QSvgGenerator>

#ifdef Q_WS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <ChangeMan.h>
#include <WebInformer.h>

#include "Timer.h"
#include "CodeView.h"
#include "MainWindow.h"

namespace Martta
{

void CullActor::doCulling()
{
//	TIME_FUNCTION;
	CullManager::get()->doCulling();
	m_primed = false;
}

void CullActor::prime()
{
	if (!m_primed)
	{
		QTimer::singleShot(0, this, SLOT(doCulling()));
		m_primed = true;
	}
}

class RealDataFinder: public DataFinder
{
public:
	RealDataFinder(String _s): m_pluginsPath(_s) {}
	virtual String				fullPathOf(String const& _f)
	{
		return m_pluginsPath + "/Data/" + _f;
	}
	String m_pluginsPath;
};

MainWindow::MainWindow(QWidget* _p, Qt::WindowFlags _f):
	QMainWindow			(_p, _f),
	m_solution			(0),
	m_updateTimer		(0),
	m_buildAndRun		(0)
{
	mInfo() << qs(qApp->applicationDirPath());
#if M_LINUX
	if (qApp->arguments().count() > 1)
		m_pluginsPath = qs(qApp->applicationDirPath() + "/" + qApp->arguments()[1]);
	else
		m_pluginsPath = qs(qApp->applicationDirPath() + "/../share/martta/plugins");
#else
	m_pluginsPath = qs(qApp->applicationDirPath() + "/../plugins");
#endif
	qDebug() << qApp->applicationDirPath() << (M_LINUX) << qApp->arguments().count() << qs(m_pluginsPath);
	qApp->addLibraryPath(qs(m_pluginsPath));
	DataFinder::set(new RealDataFinder(m_pluginsPath));

	setupUi(this);

#ifdef Q_WS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
#endif
//#ifndef Q_WS_WIN
//	setAttribute(Qt::WA_TranslucentBackground, true);
//#endif
	centralWidget()->setAttribute(Qt::WA_OpaquePaintEvent, true);

	QSettings s;
	for (int i = 0; i < s.value("mainwindow/codeviews").toInt(); i++)
		(i ? makeCodeView(s.value(QString("mainwindow/codeview%1/title").arg(i)).toString()) : codeView)->setProperties(qhv(s.value(QString("mainwindow/codeview%1/properties").arg(i)).toHash()));
	restoreState(s.value("mainwindow/state").toByteArray());
	restoreGeometry(s.value("mainwindow/geometry").toByteArray());

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	CullManager::get()->setDelayedActor(new CullActor(this));
	connect(codeView, SIGNAL(currentChanged(Concept*)), SLOT(entityFocused(Concept*)));

	loadPlugins();
	if (QFile::exists(s.value("mainwindow/lastproject").toString()))
		load(qs(s.value("mainwindow/lastproject").toString()));
	else
		setFilename(QString::null);

	resetSubject();
	ChangeMan::get()->changesDone().clear();
	ChangeMan::get()->setChanged();

	on_actShowDeps_triggered();
	on_actCastability_triggered();
	on_actShowChanges_triggered();
	on_actShowFirstChange_triggered();
}

MainWindow::~MainWindow()
{
	CullManager::get()->setDelayedActor(0);

	QSettings s;
	s.setValue("mainwindow/codeviews", findChildren<CodeView*>().count());
	int i = 1;
	foreach (CodeView* cv, findChildren<CodeView*>())
	{
		if (cv != codeView)
			s.setValue(QString("mainwindow/codeview%1/title").arg(i), static_cast<QDockWidget*>(cv->parent())->windowTitle());
		s.setValue(QString("mainwindow/codeview%1/properties").arg(cv == codeView ? 0 : i++), qhv(cv->properties()));
	}
	s.setValue("mainwindow/state", saveState());
	s.setValue("mainwindow/geometry", saveGeometry());
	s.setValue("mainwindow/lastproject", m_filename);

	desetSubject();
	delete m_solution;
	m_solution = 0;
	delete m_updateTimer;
	m_updateTimer = 0;

#if defined(DEBUG)
//	mInfo() << "Type count:" << TypeConcept::s_typeCount;
#endif
}

void MainWindow::closeEvent(QCloseEvent*)
{
	confirmLose();
}

void MainWindow::paintEvent(QPaintEvent* _ev)
{
	QMainWindow::paintEvent(_ev);
}

struct UpdateProgress
{
	QProgressDialog* m_p;
	int m_o;
	int m_t;
	inline void operator()(int _i, int _s) const { m_p->setValue(m_o + m_t * _i / _s); }
} u;

void MainWindow::loadPlugins()
{
	setEnabled(false);

	QStringList deps;
	QFileInfoList list = QDir(qs(m_pluginsPath)).entryInfoList();
	for (int i = 0; i < list.size(); ++i)
		if (list.at(i).canonicalFilePath().endsWith(".dep"))
			deps += list.at(i).canonicalFilePath();

	QProgressDialog progress("Initialising language...", "Abort", 0, deps.size() * 2 + 3, this);
	progress.setWindowModality(Qt::WindowModal);

	AuxilliaryRegistrar::get()->finaliseClasses();
	while (m_libraries.size())
	{
		m_libraries.last()->unload();
		delete m_libraries.takeLast();
	}

	progress.setValue(1);
	progress.setLabelText("Loading dependencies...");

	QHash<QString, QSet<QString> > depTree;
	foreach (QString s, deps)
	{
		QFile f(s);
		f.open(QIODevice::ReadOnly);
		QStringList deps = QString(f.readAll()).simplified().split(" ");
		depTree[deps[0]] = QSet<QString>::fromList(deps.mid(1));
	}

	progress.setValue(2);

	QString oldCur = QDir::currentPath();
	QDir::setCurrent(qs(m_pluginsPath));
	pluginsLoaded->clear();
	QSet<QString> loaded;
	int lastSize = -1;
	while (depTree.size() != lastSize)
	{
		lastSize = depTree.size();
		QSet<QString> curLoad = loaded;
		foreach (QString s, depTree.keys())
			if ((depTree[s] - curLoad).isEmpty())
			{
				progress.setLabelText("Loading & linking " + s + "...");
				m_libraries.append(new QLibrary(s));
				if (m_libraries.last()->load())
					new QTreeWidgetItem(pluginsLoaded, QStringList() << s << QStringList(depTree[s].toList()).join(", "));
				else
				{
					QString errorStr = m_libraries.last()->errorString();
					QMessageBox::warning(this, "Module Error", "Error loading " + s + ": " + m_libraries.last()->errorString(), QMessageBox::Ok);
				}
				loaded += s;
				depTree.remove(s);
				progress.setValue(progress.value() + 1);
			}
	}
	QDir::setCurrent(oldCur);

	if (depTree.size())
	{
		QString unk = "Unknown dependencies:";
		foreach (QString s, depTree.keys())
			unk += "\n" + s + ": " + QStringList(depTree[s].toList()).join(", ");
		QMessageBox::warning(this, "Load errors", unk, QMessageBox::Ok);
	}

	progress.setLabelText("Caching data...");
	AuxilliaryRegistrar::get()->jigCache();
	progress.setValue(progress.value() + 1);

	progress.setLabelText("Priming for use...");
	u.m_p = &progress;
	u.m_o = progress.value();
	u.m_t = deps.size();
	AuxilliaryRegistrar::get()->initialiseClasses<UpdateProgress>(&u);

	updateLanguage();
	setEnabled(true);
}

void MainWindow::on_actCopyCodeViewContents_triggered()
{
	foreach (CodeView* cv, findChildren<CodeView*>())
		if (cv->hasFocus())
			QApplication::clipboard()->setText(cv->toHtml());
}

void MainWindow::on_actSaveCodeViewToSvg_triggered()
{
	foreach (CodeView* cv, findChildren<CodeView*>())
		if (cv->hasFocus())
		{
			QString fn = QFileDialog::getSaveFileName(this, "Save Code View to SVG", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation), "*.svg;SVG documents");
			if (!fn.isEmpty())
			{
				QSvgGenerator g;
				g.setFileName(fn);
				cv->renderTo(&g);
			}
		}
}

void MainWindow::on_actConfigureCodeView_triggered()
{
	foreach (CodeView* cv, findChildren<CodeView*>())
		if (cv->hasFocus())
			cv->onConfigure();
}

void MainWindow::on_actRemoveCodeView_triggered()
{
	foreach (CodeView* cv, findChildren<CodeView*>())
		if (cv->hasFocus() && cv != codeView)
			delete cv->parentWidget();
}

CodeView* MainWindow::makeCodeView(QString const& _name)
{
	QDockWidget* dw = new QDockWidget(_name, this);
	CodeView* cv = new CodeView(dw);
	dw->setWidget(cv);
	cv->setSubject(codeView->subject());
	addDockWidget(Qt::RightDockWidgetArea, dw);
	return cv;
}

void MainWindow::on_actNewCodeView_triggered()
{
	bool ok = false;
	QString name = QInputDialog::getText(this, "Create New Code View", "Please enter name for new code view", QLineEdit::Normal, "New Code View", &ok);
	if (ok)
		makeCodeView(name);
}

void MainWindow::desetSubject()
{
	foreach (CodeView* cv, findChildren<CodeView*>())
		cv->setSubject(0);
}

void MainWindow::on_actReloadPlugins_triggered()
{
	QString s = serialise();

	desetSubject();
	delete m_solution;
	m_solution = 0;
	m_projects.clear();

	loadPlugins();
	deserialise(s);

	resetSubject();
	ChangeMan::get()->setChanged();
}

void MainWindow::on_actQuit_triggered()
{
	qApp->quit();
}

void MainWindow::on_actClose_triggered()
{
	if (!confirmLose())
		return;

	desetSubject();

	delete m_solution;
	m_solution = 0;
	m_projects.clear();
	setFilename(QString::null);

	resetSubject();
}

void MainWindow::on_actNew_triggered()
{
	if (!confirmLose())
		return;

	desetSubject();

	m_projects.clear();
	delete m_solution;
	m_solution = 0;

	// For now just assume the default:
	Kinds solutions = Kind::of<Program>().deriveds();
	solutions = solutions.onlyObjects();
	if (solutions.size())
	{
		m_solution = solutions[0].spawnPrepared()->asKind<Program>();
		m_solution->initialiseNew();
		foreach (Module* p, m_solution->self()->superChildrenOf<Module>())
			m_projects.insert(p, QString::null);
		updateProgramSupportPath();
	}

	setFilename(QString::null);
	resetSubject();
}

void MainWindow::on_actExecute_triggered()
{
	if (!project())
		return;

	actExecute->setEnabled(false);

	// Assume current project is executable for now.
	codeView->setEditing(0);
	if (m_buildAndRun)
	{
		// TODO: make sure you want to terminate current!
		disconnect(m_buildAndRun, SIGNAL(finished(int, QProcess::ExitStatus)));
		delete m_buildAndRun;
		m_buildAndRun = 0;
	}

	m_buildAndRun = new QProcess;
	m_steps = project()->steps();
	connect(m_buildAndRun, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(stepFinished()));
	executeNextStep();
}

void MainWindow::stepFinished()
{
	StringList step = m_steps.takeFirst();
	if (m_steps.isEmpty())
	{
		// Program finished.
		statusBar()->showMessage(QString("Program terminated with exit code %1").arg(m_buildAndRun->exitCode()));
		actExecute->setEnabled(true);
		programIn->setEnabled(false);
		m_buildAndRun->deleteLater();
		m_buildAndRun = 0;
		m_outputOwed = "";
	}
	else if (m_buildAndRun->exitStatus() || m_buildAndRun->exitCode())
	{
		String error = m_buildAndRun->readAllStandardError().data();
		if(error.isEmpty()) error = m_buildAndRun->readAllStandardOutput().data();

		QMessageBox::critical(this, "Build failed", tr("Could not build executable. Build step '%1' returned with: '%2'").arg(qs(step.join(L' '))).arg(qs(error)));
		m_buildAndRun->deleteLater();
		m_buildAndRun = 0;
		actExecute->setEnabled(true);
	}
	else
		executeNextStep();
}

void MainWindow::executeNextStep()
{
	if (m_steps.size())
	{
		if (m_steps.size() == 1)
		{
			// Build finished
			actExecute->setEnabled(true);
			programOut->setText("");
			programIn->setEnabled(true);

			connect(m_buildAndRun, SIGNAL(readyReadStandardOutput()), SLOT(programReadyOut()));
			connect(m_buildAndRun, SIGNAL(readyReadStandardError()), SLOT(programReadyError()));
		}
		m_buildAndRun->start(qs(m_steps.first()[0]), qs(m_steps.first().mid(1)), QIODevice::ReadOnly);
	}
}

void MainWindow::programReadyOut()
{
	bool track = programOut->verticalScrollBar()->sliderPosition() == programOut->verticalScrollBar()->maximum();
	QString n = m_outputOwed + QString::fromAscii(m_buildAndRun->readAllStandardOutput());
	m_outputOwed = "";
	if (n.endsWith("\n")) { n.chop(1); m_outputOwed = "\n"; }
	programOut->setPlainText(programOut->toPlainText() + n);
	if (track) programOut->verticalScrollBar()->setSliderPosition(programOut->verticalScrollBar()->maximum());
}

void MainWindow::programReadyError()
{
	bool track = programOut->verticalScrollBar()->sliderPosition() == programOut->verticalScrollBar()->maximum();
	QString n = m_outputOwed + QString::fromAscii(m_buildAndRun->readAllStandardError());
	m_outputOwed = "";
	if (n.endsWith("\n")) { n.chop(1); m_outputOwed = "\n"; }
	programOut->setPlainText(programOut->toPlainText() + n);
	if (track) programOut->verticalScrollBar()->setSliderPosition(programOut->verticalScrollBar()->maximum());
}

void MainWindow::on_programIn_returnPressed()
{
	m_buildAndRun->write((programIn->text() + "\n").toAscii());
	programIn->setText("");
}

void MainWindow::on_actOpen_triggered()
{
	if (!confirmLose())
		return;

	QString f = QFileDialog::getOpenFileName(this, "Open Program", QDir::homePath(), "*.xml");
	if (f.isEmpty())
		return;

	desetSubject();
	load(qs(f));
	resetSubject();
}

void MainWindow::on_actSave_triggered()
{
	save();
}

void MainWindow::setFilename(QString const& _fn)
{
	m_filename = _fn;
	if (m_solution)
		if (m_filename.isEmpty())
			setWindowTitle(tr("Martta - <New Program>"));
		else
			setWindowTitle(tr("Martta - %1").arg(m_filename));
	else
		setWindowTitle("Martta");
}

bool MainWindow::save()
{
	if (m_filename.isEmpty())
	{
		QString fn = QFileDialog::getSaveFileName(this, "Save Program", QDir::homePath(), "*.xml");
		if (fn.isEmpty())
			return false;
		if (!fn.endsWith(".xml"))
			fn.append(".xml");
		setFilename(fn);
	}

	foreach (Module* p, projects())
		if (!m_projects[p].isEmpty())
			if (!saveModule(qs(m_projects[p]), p))
				return false;
	return saveProgram();
}

bool MainWindow::load(String const& _filename)
{
	QFile f(qs(_filename));
	if (!f.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(0, tr("Load Program"), tr("File does not exist! (name is %1)").arg(f.fileName()));
		return 0;
	}

	QString errorStr;
	int errorLine;
	int errorColumn;

	QDomDocument doc;
	if (!doc.setContent(&f, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::critical(0, tr("Load Program"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
		return false;
	}
	if (doc.doctype().name() != "MarttaProgram")
	{
		QMessageBox::critical(0, tr("Load Program"), tr("Invalid solution file format (%1)").arg(doc.doctype().name()));
		return false;
	}

	QStringList projectFiles;
	QList<Module*> projects;
	Concept* e = importDom(doc.documentElement(), 0, &projectFiles, &projects);	// Expects to be able to write into projects.
	if (!e->isKind<Program>())
	{
		QMessageBox::critical(0, tr("Load Program"), tr("Invalid file contents (unknown Program entity %1---missing a plugin?)").arg(qs(e->kind().name())));
		e->killAndDelete();
		return false;
	}

	m_projects.clear();
	foreach (Module* p, projects)
		m_projects.insert(p, QString::null);
	delete m_solution;
	m_solution = e->asKind<Program>();
	setFilename(qs(_filename));

	List<Module*> loadedModules;
	foreach (QString f, projectFiles)
	{
		Module* p = loadModule(qs(f));
		if (p)
		{
			m_projects[p] = f;
			loadedModules << p;
		}
		else
			QMessageBox::warning(0, tr("Load Program"), tr("Module file not found (%1)").arg(f));
	}


	m_solution->initWithModules(loadedModules);
	updateProgramSupportPath();

	return true;
}

bool MainWindow::saveProgram() const
{
	if (!m_solution || m_filename.isEmpty())
		return false;

	QDomDocument doc("MarttaProgram");
	doc.appendChild(exportDom(doc, m_solution->self()));
	QFile f(m_filename);
	if (!f.open(QFile::WriteOnly))
		return false;
	QTextStream fs(&f);
	doc.save(fs, 4);
	return true;
}

bool MainWindow::saveModule(String const& _file, Module* _p) const
{
	QDomDocument doc("MarttaModule");
	doc.appendChild(exportDom(doc, _p->self()));
	QFile f(qs(_file));
	if (!f.open(QFile::WriteOnly))
		return false;
	QTextStream fs(&f);
	doc.save(fs, 4);
	return true;
}

Module* MainWindow::loadModule(String const& _filename)
{
	QFile f(qs(_filename));
	if (!f.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(0, tr("Load Module"), tr("File does not exist! (name is %1)").arg(f.fileName()));
		return 0;
	}

	QString errorStr;
	int errorLine;
	int errorColumn;

	QDomDocument doc;
	if (!doc.setContent(&f, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::critical(0, tr("Load Module"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
		return 0;
	}
	if (doc.doctype().name() != "MarttaModule")
	{
		QMessageBox::critical(0, tr("Load Module"), tr("Invalid file format (%1)").arg(doc.doctype().name()));
		return 0;
	}
	return importDom(doc.documentElement(), 0)->asKind<Module>();
}

bool MainWindow::confirmLose()
{
	if (m_solution)
		save();
	return true;
}

QString MainWindow::serialise() const
{
	QDomDocument doc("MarttaProgram");
	doc.appendChild(exportDom(doc, m_solution->self(), true));
	QString ret;
	QTextStream fs(&ret);
	doc.save(fs, 4);
	return ret;
}

bool MainWindow::deserialise(QString const& _s)
{
	QDomDocument doc;
	AssertNR(doc.setContent(_s, true, 0, 0, 0));
	AssertNR(doc.doctype().name() == "MarttaProgram");

	QStringList projectFiles;
	QList<Module*> projects;
	Concept* e = importDom(doc.documentElement(), 0, &projectFiles, &projects);	// Expects to be able to write into m_projects.
	AssertNR(projectFiles.isEmpty());
	if (!e->isKind<Program>())
	{
		QTemporaryFile f;
		f.setAutoRemove(false);
		f.open();
		f.write(_s.toUtf8());
		f.close();
		QMessageBox::critical(0, tr("Load Program"), tr("Document now invalid (unknown Program entity %1---missing a plugin?). Program contents dumped to %2.").arg(qs(e->kind().name())).arg(f.fileName()));
		e->killAndDelete();
		return false;
	}

	m_projects.clear();
	foreach (Module* p, projects)
		m_projects.insert(p, QString::null);
	delete m_solution;
	m_solution = e->asKind<Program>();

	m_solution->initWithModules();
	updateProgramSupportPath();

	return true;
}

void MainWindow::updateProgramSupportPath()
{
#ifdef Q_WS_MAC
	CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
	m_solution->setSupportPath(String(CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding())) + L"/Support/");
	CFRelease(appUrlRef);
	CFRelease(macPath);
#endif
#ifdef Q_WS_WIN
	m_solution->setSupportPath(qs(QCoreApplication::applicationDirPath() + "/../Support/"));
#endif
#ifdef Q_WS_X11
	if (qApp->arguments().count() > 1)
		m_solution->setSupportPath(qs(QCoreApplication::applicationDirPath() + "/../support/"));
	else
		m_solution->setSupportPath(qs(QCoreApplication::applicationDirPath() + "/../share/martta/support/"));
#endif
}

QDomElement MainWindow::exportDom(QDomDocument& _doc, Concept const* _e, bool _dump) const
{
	QDomElement n = _doc.createElement(qs(_e->kind().name()).replace("::", ":"));
	Hash<String, String> h;
	_e->properties(h);
	for (Hash<String, String>::Iterator i = h.begin(); i != h.end(); ++i)
		n.setAttribute(qs(i.key()), qs(i.value()));
	String iId = _e->namedIndexId();
	if (!iId.isEmpty())
		n.setAttribute("index", qs(iId));
	foreach (Concept* e, _e->savedChildren())
		if (e->isKind<Module>() && !m_projects.value(e->asKind<Module>()).isEmpty() && !_dump)
		{
			QDomElement pe = _doc.createElement("project");
			pe.setAttribute("filename", m_projects.value(e->asKind<Module>()));
			n.appendChild(pe);
		}
		else
		{
			n.appendChild(exportDom(_doc, e));
		}
	return n;
}

Concept* MainWindow::importDom(QDomElement const& _el, Concept* _p, QStringList* _unloadedModules, QList<Module*>* _projects)
{
	Concept* e = Concept::spawn(qs(_el.nodeName().replace(":", "::")));
	Assert(e, "Spawn doesn't know anything about this kind, yet it did when exported.");

	if (e->isKind<Module>() && _projects)
		*_projects << e->asKind<Module>();

	Hash<String, String> h;
	String index;
	for (uint i = 0; i < _el.attributes().length(); i++)
		if (_el.attributes().item(i).nodeName() == "index")
			index = qs(_el.attributes().item(i).nodeValue());
		else
			h.insert(qs(_el.attributes().item(i).nodeName()), qs(_el.attributes().item(i).nodeValue()));

	if (_p)
		e->move(_p->named(index));

	e->setProperties(h);
	for (QDomNode i = _el.firstChild(); !i.isNull(); i = i.nextSibling())
		if(i.isElement() && i.toElement().tagName() == "project" && _unloadedModules)
			*_unloadedModules << i.namedItem("filename").nodeValue();
		else if (i.isElement())
			importDom(i.toElement(), e, _unloadedModules, _projects);
	return e;
}

void MainWindow::on_actNewCModule_triggered()
{
}

void MainWindow::on_actAboutMartta_triggered()
{
	QMessageBox::about(this, "About Martta", "<center><font size=+4>Martta</font><br/><font size=-2><b>Technology Preview M1</b></font></center><font size=-2><p>Copyright (c) quid pro code Ltd. (UK)</p></font><font size=-1>Martta, the C++-based Extensible Quasi-Graphical Meta-Language.<br>This program is released according to the GNU GPL and is therefore Free Software: For details on how this program and derivatives thereof may be distributed, please see COPYING or visit <a href=\"http://quidprocode.co.uk/martta/\">quidprocode.co.uk/Martta</a>.</font>");
}

void MainWindow::on_actAboutQt_triggered()
{
	QMessageBox::aboutQt(this);
}

void MainWindow::on_actShowDeps_triggered()
{
	codeView->setShowDependencyInfo(actShowDeps->isChecked());
}

void MainWindow::on_actCastability_triggered()
{
#if defined(DEBUG)
//	TypeConcept::s_debugCastability = actCastability->isChecked();
#endif
}

void MainWindow::on_actListChanges_triggered()
{
	delayedUpdate();
}

void MainWindow::on_actShowChanges_triggered()
{
	codeView->setShowChanges(actShowChanges->isChecked());
}

void MainWindow::on_actShowFirstChange_triggered()
{
	codeView->setShowOneChange(actShowFirstChange->isChecked());
}

void MainWindow::on_actRemoveFirstChange_triggered()
{
	if (ChangeMan::get()->changesDone().size())
		ChangeMan::get()->changesDone().removeFirst();
	codeView->update();
}

void MainWindow::on_actClearChanges_triggered()
{
	ChangeMan::get()->changesDone().clear();
	codeView->update();
	delayedUpdate();
}

void MainWindow::resetSubject()
{
	foreach (CodeView* cv, findChildren<CodeView*>())
		if (projects().size())
			cv->setSubject(projects()[0]->self());
		else
			cv->setSubject(0);
	AssertNR(!codeView->current() || codeView->current()->root() == m_solution->self());
	entityFocused(codeView->current());
}

template<class T> void addToLanguage(Kind const& _k, T* _p)
{
	QString ifsS;
	foreach (Kind k, _k.interfaces())
		if (_k.immediateInterfaces().contains(k))
			ifsS += "*" + qs(k.name()).remove("Martta::") + "* ";
		else
			ifsS += qs(k.name()).remove("Martta::") + " ";
	if (ifsS.endsWith(", "))
		ifsS.chop(2);
	QTreeWidgetItem* i = new QTreeWidgetItem(_p, QStringList() << qs(_k.name()).remove("Martta::") << ifsS);
	foreach (Kind k, _k.immediateDeriveds())
		addToLanguage(k, i);
}

void MainWindow::updateLanguage()
{
	language->clear();
	addToLanguage(Kind::of<Concept>(), language);
	language->expandAll();

/*	QFile f("/home/gav/language.dot");
	f.open(QIODevice::WriteOnly);
	QTextStream fout(&f);
	fout << "digraph G\n{\nbgcolor=\"transparent\";\nedge [fontname=\"FreeSans\",fontsize=\"2\",labelfontname=\"FreeSans\",labelfontsize=\"2\"];\nnode [fontname=\"FreeSans\",fontsize=\"20\",shape=record];\nrankdir=LR;" << endl;
	Kinds all = AuxilliaryRegistrar::get()->auxilliaries();
	foreach (Kind k, all)
	{
		fout	<< qs(k.name()).replace("Martta::", "") << " [label=\"" << qs(k.name().replace("Martta::", "")) << "\",height=0,width=0,color=\""
				<< (k.isInterface() ? "red" : k.isPlaceholder() ? "blue" : "black") << "\"];" << endl;
		foreach (Kind b, k.immediateInterfaces())
		{
			QString colour = b.isInterface() ? "red" : "black";
			QString style = b == k.super() ? "solid" : "dotted";
			fout << qs(b.name().replace("Martta::", "")) << " -> " << qs(k.name().replace("Martta::", "")) + " [dir=back,color=\"" << colour << "\",style=\"" << style << "\"];" << endl;
		}
	}
	fout << "}";*/
}

static void addChild(QTreeWidgetItem* _p, Concept const* _c)
{
	QTreeWidgetItem* t = new QTreeWidgetItem(_p, QStringList() << _c->indexName().toCString() << _c->kind().name().toCString());
	foreach (Concept* e, _c->children())
		addChild(t, e);
}

void MainWindow::entityFocused(Concept* _e)
{
	if (codeView->current() != _e)
		codeView->setCurrent(_e);

	if (!m_updateTimer)
	{
		m_updateTimer = new QTimer(this);
		connect(m_updateTimer, SIGNAL(timeout()), SLOT(delayedUpdate()));
	}
	m_updateTimer->setSingleShot(true);
	m_updateTimer->start(500);
}

void MainWindow::updateProgramCode()
{
	if (Module* p = codeView->subject()->tryKind<Module>())
		programCode->setPlainText(qs(p->finalCode()));
}

bool hasSuperChild(Concept* _p, Concept* _e)
{
	if (_p == _e)
		return true;
	foreach (Concept* i, _p->children())
		if (hasSuperChild(i, _e))
			return true;
	return false;
}

QString MainWindow::summary(Concept* _e)
{
	if (_e && hasSuperChild(m_solution->self(), _e))
		return qs(_e->summary());
	return "NULL";
}

void MainWindow::delayedUpdate()
{
	m_updateTimer->deleteLater();
	m_updateTimer = 0;
	Concept* e = codeView->current();
	QString t;
	if (e && e->parent())
//		TIME_STATEMENT("update")
		{
			t = "<script language=\"javascript\">function toggle(_id, _t){var a = document.getElementById(_id); var hid = a.style.display == 'none'; _t.innerHTML = hid ? _id : (_id + '...'); a.style.display = hid ? null : 'none';}</script>"
				"<style type=\"text/css\">body{margin:2px;padding:0;font-size:12px;}h1{text-shadow:-1px -1px 2px #aaa;font-size:110%;text-align:left;padding:0 0 0 0.5em;margin:0;border-bottom:1px #888 solid;font-weight:bold;color:black;}.sh{color:black;font-weight:bold}h2{margin:0.5em 0 0.1em 0;cursor:pointer;text-decoration:underline;font-size:100%;color:black;font-weight:bold;} table{font-size:100%;padding:0;margin:0;border-spacing:0}th{font-size:100%;font-weight:normal;color:black;text-align:left;padding:0 1em 0 0;margin:0 0;white-space:nowrap} td{font-size:100%;color:#666;padding:0;margin:0;spacing:0;white-space:nowrap} tr{margin:0;padding:0}</style>";
			t += qs(WebInformer::basicInformationHtml(e));
			if (WebInformer* we = e->tryKind<WebInformer>())
				t += qs(we->informationHtml());
			for (Concept* p = e->parent(); p; p = p->parent())
				if (WebInformer* we = p->tryKind<WebInformer>())
					t += qs(we->superChildInformationHtml(e));
		}
	else
		t += "Nothing selected.";
	entityInfo->setHtml(t);

	if (ChangeMan::get()->hasChanged())
	{
		updateProgramCode();
		ChangeMan::get()->resetChanged();
	}

	changes->clear();
	if (actListChanges->isChecked())
		foreach (ChangeMan::Entry i, ChangeMan::get()->changesDone())
		{
			switch (i.m_op)
			{
			case ChangeMan::DependencyChanged:
				new QTreeWidgetItem(changes, QStringList() << summary(i.m_depender) + ": DepChanged" << summary(i.m_object1) + QString(", Aspect: %1").arg(i.m_aspect));
			break;
			case ChangeMan::DependencySwitched:
				new QTreeWidgetItem(changes, QStringList() << summary(i.m_depender) + ": DepSwitched" << "Coming: " + summary(i.m_object1) + ", Going: " + summary(i.m_object2));
			break;
			case ChangeMan::DependencyAdded:
				new QTreeWidgetItem(changes, QStringList() << summary(i.m_depender) + ": DepAdded" << "Coming: " + summary(i.m_object1));
			break;
			case ChangeMan::ConceptChildrenInitialised:
				new QTreeWidgetItem(changes, QStringList() << summary(i.m_depender) + ": ChildrenInit'd");
			break;
			case ChangeMan::DependencyRemoved:
				new QTreeWidgetItem(changes, QStringList() << summary(i.m_depender) + ": DepRemoved" << "Going: " + summary(i.m_object1));
			break;
			case ChangeMan::ChildMoved:
				new QTreeWidgetItem(changes, QStringList() << summary(i.m_depender) + ": ChildMoved" << summary(i.m_object1) + QString(", From: #%1").arg(i.m_index));
			break;
			case ChangeMan::IndexChanged:
				new QTreeWidgetItem(changes, QStringList() << summary(i.m_depender) + ": IndexChanged" << QString("#%1").arg(i.m_index));
			break;
			}
		}
}

}
