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

#include <ChangeMan.h>
#undef inline

#include "Project.h"
#include "IncludeProject.h"
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
}

MainWindow::MainWindow(QWidget* _p, Qt::WindowFlags _f):
	QMainWindow			(_p, _f),
	m_program			(0),
	m_updateTimer		(0)
{
	qApp->addLibraryPath("/Users/gav/Projects/Martta/plugins");

	setupUi(this);
	m_codeView = codeView;

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
	dependencies->setHeader(0);
	connect(dependencies, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(showDependenciesMenu(const QPoint&)));
	connect(m_codeView, SIGNAL(currentChanged(Entity*)), SLOT(entityFocused(Entity*)));

	loadPlugins();
	m_project = new Project(s.value("mainwindow/lastproject").toString());
	projectRenamed();
	connect(m_project, SIGNAL(nameChanged()), SLOT(projectRenamed()));
	connect(m_project, SIGNAL(subjectInvalid()), SLOT(resetSubject()));
	dependencies->setModel(m_project->cDependsModel());


//	classesView->setSubject(m_project->ns());

	resetSubject();
	updateProgramCode();
	updateLanguage();
}

MainWindow::~MainWindow()
{
	CullManager::get()->setDelayedActor(0);

	confirmLose();
	QSettings s;
	s.setValue("mainwindow/state", saveState());
	s.setValue("mainwindow/geometry", saveGeometry());
	s.setValue("mainwindow/lastproject", m_project->filename());

	m_codeView->setSubject(0);
	delete m_project;
	m_project = 0;
	delete m_program;
	m_program = 0;
	delete m_updateTimer;
	m_updateTimer = 0;

#if defined(DEBUG)
	mInfo() << "Type count:" << TypeEntity::s_typeCount;
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
	m_codeView->setSubject(m_project->ns());
	AssertNR(!m_codeView->current() || m_codeView->current()->root() == m_project->root());
	entityFocused(m_codeView->current());
}

void MainWindow::on_actAboutMartta_triggered()
{
	QMessageBox::about(this, "About Martta", "<center><font size=+4>Martta</font><br/><font size=-2><b>Technology Preview M0</b></font></center><font size=-2><p>Copyright (c) quid pro code Ltd. (UK)</p></font><font size=-1>Martta, the C++-based Extensible Quasi-Graphical Meta-Language.<br>This program is released according to the GNU GPL and is therefore Free Software: For details on how this program and derivatives thereof may be distributed, please see COPYING or visit <a href=\"http://quidprocode.co.uk/martta/\">quidprocode.co.uk/Martta</a>.</font>");
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
/*
String compileTypes(Types const& _t)
{
	String ret;
	foreach (Type t, _t)
		ret += t->code() + L", ";
	if (ret.endsWith(L", "))
		ret.chop(2);
	return ret;
}*/

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
	if (m_codeView->current() != _e)
		m_codeView->setCurrent(_e);

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
	Entity* e = m_codeView->current();
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
		new QTreeWidgetItem(entityInfo, QStringList() << QString("Layout") << qs(e->defineLayout(m_codeView->viewKeys(e))));

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
	TypeEntity::s_debugCastability = actCastability->isChecked();
#endif
}

void MainWindow::saveCode()
{
}

void MainWindow::projectRenamed()
{
	setWindowTitle(m_project->name() + " - Martta");
}

void MainWindow::classSelected(QModelIndex const& _i)
{
	(void)_i;
//	m_codeView->setSubject(reinterpret_cast<Class*>(_i.data(Project::OwnerRole).value<void*>()));
}

void MainWindow::on_actNewProject_triggered()
{
	if (!confirmLose()) return;

	m_codeView->setSubject(0);
	m_project->resetAsNew();
	m_codeView->setSubject(m_project->ns());
//	m_codeView->navigateOnto(m_project->ns());
}

bool MainWindow::confirmLose()
{
	if (QFile::exists(m_project->filename()))
		m_project->save();
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
	// Use it for something else for now...

	confirmLose();
	QString name = m_project->name();
	m_codeView->setSubject(0);
	delete m_project;

	loadPlugins();
	m_project = new Project(name);
	projectRenamed();
	connect(m_project, SIGNAL(nameChanged()), SLOT(projectRenamed()));
	connect(m_project, SIGNAL(subjectInvalid()), SLOT(resetSubject()));
	dependencies->setModel(m_project->cDependsModel());

	resetSubject();
	updateProgramCode();
	updateLanguage();

//	clearChanges();
//	codeView->update();
}

void MainWindow::on_actNewCProject_triggered()
{
	QAbstractItemModel* m = m_project->cDependsModel();
	int r = m->rowCount();
	m->insertRow(r);
	m->setData(m->index(r, 0), "New C Project", Qt::EditRole);
	dependencies->edit(m->index(r, 0));
}

void MainWindow::on_actQuit_triggered()
{
	qApp->quit();
}

void MainWindow::on_actOpen_triggered()
{
	if (!confirmLose()) return;

	QString f = QFileDialog::getOpenFileName(this, "Open Project", "/home/gav", "*.xml");
	if (f.isEmpty()) return;

	m_codeView->setSubject(0);
	m_project->open(f);
	m_codeView->setSubject(m_project->ns());
//	m_codeView->setCurrent(m_project->program());
}

void MainWindow::on_actSave_triggered()
{
	saveCode();
	if (m_project->filename().isEmpty())
		m_project->rename(QFileDialog::getSaveFileName(this, "Save Project", "/home/gav", "*.xml"));
	m_project->save();
}

void MainWindow::showDependenciesMenu(QPoint const& _p)
{
	QModelIndex i = dependencies->indexAt(_p);
	QAbstractItemModel* m = m_project->cDependsModel();

	QList<QAction*> actions;
	if (!i.data(Project::ItemRole).isNull() && i.data(Project::ItemRole).toInt() == Project::All)
		actions.append(actRemoveCDependency);
	if (!i.data(Project::HeadingRole).isNull() && i.data(Project::HeadingRole).toInt() == Project::Includes)
		actions.append(actIncludeHeader);
	if (!i.data(Project::ItemRole).isNull() && i.data(Project::ItemRole).toInt() == Project::Includes)
		actions.append(actRemoveHeader);

	if (actions.count() > 0)
	{
		QAction* a = QMenu::exec(actions, dependencies->mapToGlobal(_p));
		if (a == actIncludeHeader)
		{
			QString fn = QFileDialog::getOpenFileName(this, "Include File", "/usr/include", "*.h");
			if (!fn.isEmpty())
			{
				int r = m->rowCount(i);
				m->insertRow(r, i);
				m->setData(m->index(r, 0, i), fn, Qt::EditRole);
			}
		}
		else if (a == actRemoveCDependency || a == actRemoveHeader)
		{
			m->removeRow(i.row(), i.parent());
		}
	}
}

void MainWindow::on_programIn_returnPressed()
{
	m_program->write((programIn->text() + "\n").toAscii());
	programIn->setText("");
}

void MainWindow::programReadyOut()
{
	bool track = programOut->verticalScrollBar()->sliderPosition() == programOut->verticalScrollBar()->maximum();
	QString n = m_outputOwed + QString::fromAscii(m_program->readAllStandardOutput());
	m_outputOwed = "";
	if (n.endsWith("\n")) { n.chop(1); m_outputOwed = "\n"; }
	programOut->setPlainText(programOut->toPlainText() + n);
	if (track) programOut->verticalScrollBar()->setSliderPosition(programOut->verticalScrollBar()->maximum());
}

void MainWindow::programReadyError()
{
	bool track = programOut->verticalScrollBar()->sliderPosition() == programOut->verticalScrollBar()->maximum();
	QString n = m_outputOwed + QString::fromAscii(m_program->readAllStandardError());
	m_outputOwed = "";
	if (n.endsWith("\n")) { n.chop(1); m_outputOwed = "\n"; }
	programOut->setPlainText(programOut->toPlainText() + n);
	if (track) programOut->verticalScrollBar()->setSliderPosition(programOut->verticalScrollBar()->maximum());
}

void MainWindow::programFinished(int _exitCode)
{
	programIn->setEnabled(false);
	statusBar()->showMessage(QString("Program terminated with exit code %1").arg(_exitCode));
	m_outputOwed = "";
	m_program->deleteLater();
	m_program = 0;
}

void MainWindow::updateProgramCode()
{
	programCode->setText(m_project->code());
}

void MainWindow::on_actExecute_triggered()
{
	m_codeView->setEditing(0);
	if (m_program)
	{
		// TODO: make sure you want to terminate current!
		delete m_program;
		m_program = 0;
	}

	saveCode();

	QString exe = m_project->executable();
	if (exe.isEmpty())
	{
		QMessageBox::critical(this, "Execution Failed", "Could not build executable. Compiler returned with: " + m_project->lastCompileError());
		return;
	}
	programOut->setText("");
	programIn->setEnabled(true);

	m_program = new QProcess(this);
	connect(m_program, SIGNAL(readyReadStandardOutput()), SLOT(programReadyOut()));
	connect(m_program, SIGNAL(readyReadStandardError()), SLOT(programReadyError()));
	connect(m_program, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(programFinished(int)));
	m_program->start(exe, QStringList());
}

}
