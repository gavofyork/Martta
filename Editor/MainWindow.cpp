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

#ifdef Q_WS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <ChangeMan.h>

#include "Timer.h"
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
	virtual String				fullPathOf(String const& _f)
	{
		return MARTTA_PLUGINS_PATH "/Data/" + _f;
	}
};

MainWindow::MainWindow(QWidget* _p, Qt::WindowFlags _f):
	QMainWindow			(_p, _f),
	m_solution			(0),
	m_updateTimer		(0),
	m_buildAndRun		(0)
{
	qApp->addLibraryPath(MARTTA_PLUGINS_PATH);
	DataFinder::set(new RealDataFinder);

	setupUi(this);

#ifdef Q_WS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
#endif

	QSettings s;
	restoreState(s.value("mainwindow/state").toByteArray());
	restoreGeometry(s.value("mainwindow/geometry").toByteArray());

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	CullManager::get()->setDelayedActor(new CullActor(this));
	connect(codeView, SIGNAL(currentChanged(Entity*)), SLOT(entityFocused(Entity*)));

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

	confirmLose();
	QSettings s;
	s.setValue("mainwindow/state", saveState());
	s.setValue("mainwindow/geometry", saveGeometry());
	s.setValue("mainwindow/lastproject", m_filename);

	codeView->setSubject(0);
	delete m_solution;
	m_solution = 0;
	delete m_updateTimer;
	m_updateTimer = 0;

#if defined(DEBUG)
//	mInfo() << "Type count:" << TypeEntity::s_typeCount;
#endif
}

void MainWindow::loadPlugins()
{
	AuxilliaryRegistrar::get()->finaliseClasses();
	while (m_libraries.size())
	{
		m_libraries.last()->unload();
		delete m_libraries.takeLast();
	}

	QFileInfoList list = QDir(MARTTA_PLUGINS_PATH).entryInfoList();
	qDebug() << "Searching for plugins in" << MARTTA_PLUGINS_PATH;

	QHash<QString, QSet<QString> > depTree;
	for (int i = 0; i < list.size(); ++i)
		if (list.at(i).canonicalFilePath().endsWith(".dep"))
		{
			QFile f(list.at(i).canonicalFilePath());
			f.open(QIODevice::ReadOnly);
			QStringList deps = QString(f.readAll()).simplified().split(" ");
			depTree[deps[0]] = QSet<QString>::fromList(deps.mid(1));
		}

	foreach (QString s, depTree.keys())
		mDebug() << "Found plugin" << qs(s) << "with deps" << qs(QStringList(depTree[s].toList()).join(" "));

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
				m_libraries.append(new QLibrary(s));
				if (m_libraries.last()->load())
				{
					qDebug() << "Loaded " << s;
					loaded += s;
					new QTreeWidgetItem(pluginsLoaded, QStringList() << s << QStringList(depTree[s].toList()).join(", "));
				}
				else
					Assert(false, "Error loading " + qs(s) + ": " + qs(m_libraries.last()->errorString()));
				depTree.remove(s);
			}
	}

	qDebug() << "Unknown dependencies:";
	foreach (QString s, depTree.keys())
		qDebug() << s << QStringList(depTree[s].toList()).join(", ");

	AuxilliaryRegistrar::get()->jigCache();
	AuxilliaryRegistrar::get()->initialiseClasses();

	updateLanguage();
}

void MainWindow::on_actReloadPlugins_triggered()
{
	QString s = serialise();

	codeView->setSubject(0);
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

	codeView->setSubject(0);

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

	codeView->setSubject(0);

	m_projects.clear();
	delete m_solution;
	m_solution = 0;

	// For now just assume the default:
	Kinds solutions = Kind::of<Solution>().deriveds();
	solutions = solutions.onlyObjects();
	if (solutions.size())
	{
		m_solution = solutions[0].spawnPrepared()->asKind<Solution>();
		m_solution->initialiseNew();
		foreach (Project* p, m_solution->self()->superChildrenOf<Project>())
			m_projects.insert(p, QString::null);
		updateSolutionSupportPath();
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
		QMessageBox::critical(this, "Build failed", tr("Could not build executable. Build step '%1' returned with: '%2'").arg(qs(step.join(L' '))).arg(m_buildAndRun->readAllStandardError().data()));
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

	QString f = QFileDialog::getOpenFileName(this, "Open Solution", QDir::homePath(), "*.xml");
	if (f.isEmpty())
		return;

	codeView->setSubject(0);
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
			setWindowTitle(tr("Martta - <New Solution>"));
		else
			setWindowTitle(tr("Martta - %1").arg(m_filename));
	else
		setWindowTitle("Martta");
}

bool MainWindow::save()
{
	if (m_filename.isEmpty())
	{
		QString fn = QFileDialog::getSaveFileName(this, "Save Solution", QDir::homePath(), "*.xml");
		if (fn.isEmpty())
			return false;
		if (!fn.endsWith(".xml"))
			fn.append(".xml");
		setFilename(fn);
	}

	foreach (Project* p, projects())
		if (!m_projects[p].isEmpty())
			if (!saveProject(qs(m_projects[p]), p))
				return false;
	return saveSolution();
}

bool MainWindow::load(String const& _filename)
{
	QFile f(qs(_filename));
	if (!f.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(0, tr("Load Solution"), tr("File does not exist! (name is %1)").arg(f.fileName()));
		return 0;
	}

	QString errorStr;
	int errorLine;
	int errorColumn;

	QDomDocument doc;
	if (!doc.setContent(&f, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::critical(0, tr("Load Solution"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
		return false;
	}
	if (doc.doctype().name() != "MarttaSolution")
	{
		QMessageBox::critical(0, tr("Load Solution"), tr("Invalid solution file format (%1)").arg(doc.doctype().name()));
		return false;
	}

	QStringList projectFiles;
	QList<Project*> projects;
	Entity* e = importDom(doc.documentElement(), 0, &projectFiles, &projects);	// Expects to be able to write into projects.
	if (!e->isKind<Solution>())
	{
		QMessageBox::critical(0, tr("Load Solution"), tr("Invalid file contents (unknown Solution entity %1---missing a plugin?)").arg(qs(e->kind().name())));
		e->killAndDelete();
		return false;
	}

	m_projects.clear();
	foreach (Project* p, projects)
		m_projects.insert(p, QString::null);
	delete m_solution;
	m_solution = e->asKind<Solution>();
	setFilename(qs(_filename));

	List<Project*> loadedProjects;
	foreach (QString f, projectFiles)
	{
		Project* p = loadProject(qs(f));
		if (p)
		{
			m_projects[p] = f;
			loadedProjects << p;
		}
		else
			QMessageBox::warning(0, tr("Load Solution"), tr("Project file not found (%1)").arg(f));
	}


	m_solution->initWithProjects(loadedProjects);
	updateSolutionSupportPath();

	return true;
}

bool MainWindow::saveSolution() const
{
	if (!m_solution || m_filename.isEmpty())
		return false;

	QDomDocument doc("MarttaSolution");
	doc.appendChild(exportDom(doc, m_solution->self()));
	QFile f(m_filename);
	if (!f.open(QFile::WriteOnly))
		return false;
	QTextStream fs(&f);
	doc.save(fs, 4);
	return true;
}

bool MainWindow::saveProject(String const& _file, Project* _p) const
{
	QDomDocument doc("MarttaProject");
	doc.appendChild(exportDom(doc, _p->self()));
	QFile f(qs(_file));
	if (!f.open(QFile::WriteOnly))
		return false;
	QTextStream fs(&f);
	doc.save(fs, 4);
	return true;
}

Project* MainWindow::loadProject(String const& _filename)
{
	QFile f(qs(_filename));
	if (!f.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(0, tr("Load Project"), tr("File does not exist! (name is %1)").arg(f.fileName()));
		return 0;
	}

	QString errorStr;
	int errorLine;
	int errorColumn;

	QDomDocument doc;
	if (!doc.setContent(&f, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::critical(0, tr("Load Project"), tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
		return 0;
	}
	if (doc.doctype().name() != "MarttaProject")
	{
		QMessageBox::critical(0, tr("Load Project"), tr("Invalid file format (%1)").arg(doc.doctype().name()));
		return 0;
	}
	return importDom(doc.documentElement(), 0)->asKind<Project>();
}

bool MainWindow::confirmLose()
{
	if (m_solution)
		save();
	return true;
}

QString MainWindow::serialise() const
{
	QDomDocument doc("MarttaSolution");
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
	AssertNR(doc.doctype().name() == "MarttaSolution");

	QStringList projectFiles;
	QList<Project*> projects;
	Entity* e = importDom(doc.documentElement(), 0, &projectFiles, &projects);	// Expects to be able to write into m_projects.
	AssertNR(projectFiles.isEmpty());
	if (!e->isKind<Solution>())
	{
		QTemporaryFile f;
		f.setAutoRemove(false);
		f.open();
		f.write(_s.toUtf8());
		f.close();
		QMessageBox::critical(0, tr("Load Solution"), tr("Document now invalid (unknown Solution entity %1---missing a plugin?). Solution contents dumped to %2.").arg(qs(e->kind().name())).arg(f.fileName()));
		e->killAndDelete();
		return false;
	}

	m_projects.clear();
	foreach (Project* p, projects)
		m_projects.insert(p, QString::null);
	delete m_solution;
	m_solution = e->asKind<Solution>();

	m_solution->initWithProjects();
	updateSolutionSupportPath();

	return true;
}

void MainWindow::updateSolutionSupportPath()
{
#ifdef Q_WS_MAC
	CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
	m_solution->setSupportPath(String(CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding())) + L"/Support/");
	CFRelease(appUrlRef);
	CFRelease(macPath);
#endif
#ifdef Q_WS_WIN
	m_solution->setSupportPath(qs(QCoreApplication::applicationDirPath() + "/Support/"));
#endif
#ifdef Q_WS_X11
	m_solution->setSupportPath(qs(QCoreApplication::applicationDirPath() + "/../support/"));
#endif
}

QDomElement MainWindow::exportDom(QDomDocument& _doc, Entity const* _e, bool _dump) const
{
	QDomElement n = _doc.createElement(qs(_e->kind().name()).replace("::", ":"));
	Hash<String, String> h;
	_e->properties(h);
	for (Hash<String, String>::Iterator i = h.begin(); i != h.end(); ++i)
		n.setAttribute(qs(i.key()), qs(i.value()));
	String iId = _e->namedIndexId();
	if (!iId.isEmpty())
		n.setAttribute("index", qs(iId));
	foreach (Entity* e, _e->savedChildren())
		if (e->isKind<Project>() && !m_projects.value(e->asKind<Project>()).isEmpty() && !_dump)
		{
			QDomElement pe = _doc.createElement("project");
			pe.setAttribute("filename", m_projects.value(e->asKind<Project>()));
			n.appendChild(pe);
		}
		else
		{
			n.appendChild(exportDom(_doc, e));
		}
	return n;
}

Entity* MainWindow::importDom(QDomElement const& _el, Entity* _p, QStringList* _unloadedProjects, QList<Project*>* _projects)
{
	Entity* e = Entity::spawn(qs(_el.nodeName().replace(":", "::")));
	Assert(e, "Spawn doesn't know anything about this kind, yet it did when exported.");

	if (e->isKind<Project>() && _projects)
		*_projects << e->asKind<Project>();

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
		if(i.isElement() && i.toElement().tagName() == "project" && _unloadedProjects)
			*_unloadedProjects << i.namedItem("filename").nodeValue();
		else if (i.isElement())
			importDom(i.toElement(), e, _unloadedProjects, _projects);
	return e;
}

void MainWindow::on_actNewCProject_triggered()
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
//	TypeEntity::s_debugCastability = actCastability->isChecked();
#endif
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
}

void MainWindow::resetSubject()
{
	if (projects().size())
		codeView->setSubject(projects()[0]->self());
	else
		codeView->setSubject(0);

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
	addToLanguage(Kind::of<Entity>(), language);
	language->expandAll();
}

static void addChild(QTreeWidgetItem* _p, Entity const* _c)
{
	QTreeWidgetItem* t = new QTreeWidgetItem(_p, QStringList() << _c->indexName().toCString() << _c->kind().name().toCString());
	foreach (Entity* e, _c->children())
		addChild(t, e);
}

String compileKinds(Kinds const& _t)
{
	String ret;
	foreach (Kind k, _t)
		ret += k.name() + L", ";
	if (ret.endsWith(L", "))
		ret.chop(2);
	return ret;
}

void MainWindow::entityFocused(Entity* _e)
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
	if (Project* p = codeView->subject()->tryKind<Project>())
		programCode->setText(qs(p->finalCode()));
}

void MainWindow::delayedUpdate()
{
	m_updateTimer->deleteLater();
	m_updateTimer = 0;
	Entity* e = codeView->current();
	QString t;
	if (e && e->parent())
	TIME_STATEMENT("update"){
		t += "<table><tr>";
		t += "<td><b>" + qs(e->kind().name()) + "</b>";
		foreach (Kind k, e->parent()->allowedKinds(e->index()))
			t += "<br>" + qs(k.name());
		t += "<br>";
		foreach (Kind k, e->parent()->deniedKinds(e->index()))
			t += "<br><i>" + qs(k.name()) + "</i>";
		t += "</td>";
/*		if (e->isKind<BareTyped>())
		{
			t += "<td><b><i>" + Qt::escape(qs(e->asKind<BareTyped>()->type()->code())) + "</i><br/><font size=\"-2\" color=\"#888888\">(" + Qt::escape(qs(e->asKind<BareTyped>()->apparentType()->code())) + ")</font></b>";
			foreach (Type i, e->asKind<BareTyped>()->ourAllowedTypes())
				t += "<br>" + Qt::escape(qs(i->code()));
			t += "<br>";
			foreach (Type i, e->asKind<BareTyped>()->ourDeniedTypes())
				t += "<br><i>" + Qt::escape(qs(i->code())) + "</i>";
			t += "</td>";
		}*/
		t += "</tr></table>";

		int vvalue = typesVisible->verticalScrollBar()->value();
		typesVisible->clear();
/*		if (e->isKind<Statement>())
		{
			QTreeWidgetItem* l = new QTreeWidgetItem(typesVisible, QStringList() << QString("Local"));
			foreach (ValueDefiner* v, e->asKind<Statement>()->valuesInLocalScope())
				new QTreeWidgetItem(l, QStringList() << qs(v->name()) << qs(v->type()->code()));
		}*/
/*		if (e->hasAncestor<Class>())
		{
			QTreeWidgetItem* m = new QTreeWidgetItem(typesVisible, QStringList() << QString("Members"));
			QTreeWidgetItem* h = new QTreeWidgetItem(m, QStringList() << QString("Hidden"));
			foreach (ValueDefiner* v, castEntities<ValueDefiner>(e->ancestor<Class>()->membersOf<MemberValue>(e->hasAncestor<MemberLambda>() ? e->ancestor<MemberLambda>()->isConst() : false)))
				new QTreeWidgetItem(v->isKind<Artificial>() ? h : m, QStringList() << qs(v->name()) << qs(v->type()->code()));
		}*/
/*		QTreeWidgetItem* g = new QTreeWidgetItem(typesVisible, QStringList() << QString("General"));
		foreach (ValueDefiner* v, e->ancestor<Declaration>()->valuesKnown())
			new QTreeWidgetItem(g, QStringList() << qs(v->name()) << qs(v->type()->code()));*/
/*		QTreeWidgetItem* gl = new QTreeWidgetItem(typesVisible, QStringList() << QString("Global"));
		foreach (ValueDefiner* v, e->root()->childrenOf<ValueDefiner>())
			new QTreeWidgetItem(gl, QStringList() << QString(v->name()) << QString(v->type()->code()));*/
		typesVisible->expandAll();
		typesVisible->verticalScrollBar()->setValue(vvalue);

		vvalue = entityInfo->verticalScrollBar()->value();
		entityInfo->clear();

/*		if (Declaration* d = e->selfAncestor<Declaration>())
		{
			QTreeWidgetItem* decl = new QTreeWidgetItem(entityInfo, QStringList() << QString("Declaration Context"));
			new QTreeWidgetItem(decl, QStringList() << qs(d->name()) << qs(d->kind().name()));
			QTreeWidgetItem* ul = new QTreeWidgetItem(decl, QStringList() << QString("Utilised"));
			foreach (Declaration* u, d->utilised())
				new QTreeWidgetItem(ul, QStringList() << (u ? qs(u->name()) : QString("NULL?")) << (u ? qs(u->kind().name()) : QString("NULL?")));
			QTreeWidgetItem* us = new QTreeWidgetItem(decl, QStringList() << QString("Utilised Siblings"));
			foreach (Declaration* u, d->utilisedSiblings())
				new QTreeWidgetItem(us, QStringList() << (u ? qs(u->name()) : QString("NULL?")) << (u ? qs(u->kind().name()) : QString("NULL?")));
		}
*/
//		new QTreeWidgetItem(entityInfo, QStringList() << QString("HTML") << qs(e->toHtml()));

		QTreeWidgetItem* rc = new QTreeWidgetItem(entityInfo, QStringList() << "Child restrictions");
		foreach (int i, e->knownNames())
			if (e->allowedKinds(i).count())
				new QTreeWidgetItem(rc, QStringList() << qs(e->indexName(i)) << qs(compileKinds(e->allowedKinds(i)) + " (%1 req'd)").arg(e->minRequired(i)));
		for (int i = 0; i < e->minRequired(); ++i)
			new QTreeWidgetItem(rc, QStringList() << QString::number(i) << qs(compileKinds(e->allowedKinds(i)) + " (req'd)"));
		if (e->allowedKinds(e->minRequired()).count())
			new QTreeWidgetItem(rc, QStringList() << "..." << qs(compileKinds(e->allowedKinds(e->minRequired()))));


/*		if (TypeNamer* td = e->tryKind<TypeNamer>())
		{
			QTreeWidgetItem* te = new QTreeWidgetItem(entityInfo, QStringList() << QString("Type Namer Information") << qs(td->type()->code()));
			new QTreeWidgetItem(te, QStringList() << "Apparent type" << qs(td->apparentType()->code()));
			new QTreeWidgetItem(te, QStringList() << "Type restrictions" << qs(compileTypes(td->ourAllowedTypes()) + " (%1 types)").arg(td->ourAllowedTypes().size()));
			foreach (ValueDefiner* v, td->type()->applicableMembers())
				new QTreeWidgetItem(te, QStringList() << qs(v->name()) << qs(v->type()->code()));
		}
*/		/*
		if (TypedOwner* to = e->tryKind<TypedOwner>())
		{
			QTreeWidgetItem* te = new QTreeWidgetItem(entityInfo, QStringList() << QString("Typed Owner Information"));

			foreach (int i, e->knownNames())
				if (to->allowedTypes(i).count())
					new QTreeWidgetItem(te, QStringList() << qs(e->indexName(i)) + ": " + qs(to->nominalType(i)->code() + " {" + to->effectiveType(i)->code() + "}") << qs(compileTypes(to->allowedTypes(i)) + " (%1 types)").arg(to->allowedTypes(i).size()) );
			for (int i = 0; i < max(e->minRequired(), e->cardinalChildCount()); ++i)
				if (to->allowedTypes(i).count())
					new QTreeWidgetItem(te, QStringList() << QString::number(i) + ": " + qs(to->nominalType(i)->code() + " {" + to->effectiveType(i)->code() + "}") << qs(compileTypes(to->allowedTypes(i)) + " (%1 types)").arg(to->allowedTypes(i).size()) );
			if (to->allowedTypes(max(e->minRequired(), e->cardinalChildCount())).count())
				new QTreeWidgetItem(te, QStringList() << "..." << qs(compileTypes(to->allowedTypes(max(e->minRequired(), e->cardinalChildCount()))) + " (%1 types)").arg(to->allowedTypes(max(e->minRequired(), e->cardinalChildCount())).size()) );
		}*/

		QTreeWidgetItem* ac = new QTreeWidgetItem(entityInfo, QStringList() << "Actual children" << qs(e->kind().name()));
		foreach (Entity* e, e->children())
			addChild(ac, e);

		entityInfo->expandAll();
		entityInfo->verticalScrollBar()->setValue(vvalue);
	}
	else
		t += "Nothing selected.";
	informationView->setText(t);

	if (ChangeMan::get()->hasChanged())
	{
		updateProgramCode();
		ChangeMan::get()->resetChanged();
	}
}

}
