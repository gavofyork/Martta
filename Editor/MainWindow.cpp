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
#undef inline

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

void MainWindow::loadPlugins()
{
	AuxilliaryRegistrar::get()->finaliseClasses();
	while (m_libraries.size())
	{
		m_libraries.last()->unload();
		delete m_libraries.takeLast();
	}

	QFileInfoList list = QDir("/Users/gav/Projects/Martta/plugins").entryInfoList();

	QHash<QString, QSet<QString> > depTree;
	for (int i = 0; i < list.size(); ++i)
		if (list.at(i).canonicalFilePath().endsWith(".dep"))
		{
			QFile f(list.at(i).canonicalFilePath());
			f.open(QIODevice::ReadOnly);
			QStringList deps = QString(f.readAll()).simplified().split(" ");
			depTree[deps[0]] = QSet<QString>::fromList(deps.mid(1));
		}

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
				m_libraries.last()->load();
				qDebug() << "Loaded " << s;
				loaded += s;
				new QTreeWidgetItem(pluginsLoaded, QStringList() << s << QStringList(depTree[s].toList()).join(", "));
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

MainWindow::MainWindow(QWidget* _p, Qt::WindowFlags _f):
	QMainWindow			(_p, _f),
	m_solution			(0),
	m_updateTimer		(0),
	m_buildAndRun		(0)
{
	qApp->addLibraryPath("/Users/gav/Projects/Martta/plugins");

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
//	if (QFile::exists(s.value("mainwindow/lastproject").toString()))
//		openSolution(qs(s.value("mainwindow/lastproject").toString()));

	resetSubject();
	ChangeMan::get()->setChanged();
}

MainWindow::~MainWindow()
{
	CullManager::get()->setDelayedActor(0);

	confirmLose();
	QSettings s;
	s.setValue("mainwindow/state", saveState());
	s.setValue("mainwindow/geometry", saveGeometry());
	s.setValue("mainwindow/lastproject", qs(m_solution->filename()));

	codeView->setSubject(0);
	delete m_solution;
	m_solution = 0;
	delete m_updateTimer;
	m_updateTimer = 0;

#if defined(DEBUG)
//	mInfo() << "Type count:" << TypeEntity::s_typeCount;
#endif
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

void MainWindow::resetSubject()
{
	if (projects().size())
		codeView->setSubject(projects()[0]->self());
	else
		codeView->setSubject(0);

	AssertNR(!codeView->current() || codeView->current()->root() == m_solution->self());
	entityFocused(codeView->current());
}

void MainWindow::on_actAboutMartta_triggered()
{
	QMessageBox::about(this, "About Martta", "<center><font size=+4>Martta</font><br/><font size=-2><b>Technology Preview M1</b></font></center><font size=-2><p>Copyright (c) quid pro code Ltd. (UK)</p></font><font size=-1>Martta, the C++-based Extensible Quasi-Graphical Meta-Language.<br>This program is released according to the GNU GPL and is therefore Free Software: For details on how this program and derivatives thereof may be distributed, please see COPYING or visit <a href=\"http://quidprocode.co.uk/martta/\">quidprocode.co.uk/Martta</a>.</font>");
}

void MainWindow::on_actAboutQt_triggered()
{
	QMessageBox::aboutQt(this);
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
		new QTreeWidgetItem(entityInfo, QStringList() << QString("Layout") << qs(e->defineLayout(codeView->viewKeys(e))));

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

void MainWindow::on_actNew_triggered()
{
	if (!confirmLose())
		return;

	codeView->setSubject(0);

	delete m_solution;

	// For now just assume the default:
	Kinds solutions = Kind::of<Solution>().deriveds();
	solutions = solutions.onlyObjects();
	if (solutions.size())
	{
		m_solution = solutions[0].spawnPrepared()->asKind<Solution>();
		m_solution->initialiseNew();
		updateSolutionSupportPath();
	}
	else
		m_solution = 0;

	resetSubject();
}

bool MainWindow::confirmLose()
{
	if (m_solution)
		m_solution->save();
	return true;
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
//	if (s_changes.size())
//		s_changes.removeFirst();
//	codeView->update();
}

void MainWindow::on_actClearChanges_triggered()
{
//	clearChanges();
//	codeView->update();
}

void MainWindow::on_actNewCProject_triggered()
{
}

// Open solution
void MainWindow::on_actOpen_triggered()
{
	if (!confirmLose()) return;

	QString f = QFileDialog::getOpenFileName(this, "Open Project", "/home/gav", "*.xml");
	if (f.isEmpty()) return;

	codeView->setSubject(0);
/*	m_project->open(f);
	codeView->setSubject(m_project->ns());*/
}

void MainWindow::on_actSave_triggered()
{
/*	saveCode();
	if (m_project->filename().isEmpty())
		m_project->rename(QFileDialog::getSaveFileName(this, "Save Project", "/home/gav", "*.xml"));
	m_project->save();*/
}

/*void MainWindow::saveProject(Project* _p)
{
	AssertNR(!_p->filename().isEmpty());
}*/

void MainWindow::on_actQuit_triggered()
{
	qApp->quit();
}

void MainWindow::on_programIn_returnPressed()
{
	m_buildAndRun->write((programIn->text() + "\n").toAscii());
	programIn->setText("");
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

void MainWindow::updateProgramCode()
{
	if (Project* p = codeView->subject()->tryKind<Project>())
		programCode->setText(qs(p->finalCode()));
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

QStringList qs(List<String> const& _f)
{
	QStringList ret;
	foreach (String s, _f)
		ret << qs(s);
	return ret;
}

void MainWindow::stepFinished()
{
	StringList step = m_steps.takeFirst();
	if (m_buildAndRun->exitStatus() || m_buildAndRun->exitCode())
	{
		QMessageBox::critical(this, "Build failed", tr("Could not build executable. Build step '%1' returned with: '%2'").arg(qs(step.join(L' '))).arg(m_buildAndRun->readAllStandardError().data()));
		delete m_buildAndRun;
		m_buildAndRun = 0;
		actExecute->setEnabled(true);
		return;
	}
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
	else
	{
		// Program finished.
		statusBar()->showMessage(QString("Program terminated with exit code %1").arg(m_buildAndRun->exitCode()));
		actExecute->setEnabled(true);
		programIn->setEnabled(false);
		m_buildAndRun->deleteLater();
		m_buildAndRun = 0;
		m_outputOwed = "";
	}
}

}
