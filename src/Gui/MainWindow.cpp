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

// CHANGE PRECOMPILED HEADER SO IT INCLUDES QString BEFORE msString.
// NO NEED TO DO THAT OUTSIDE THE MARTTA GUI THOUGH (i.e. IN THE LANGUAGE GENERALLY).

#include "CullManager.h"
#include "ChangeMan.h"
#include "Project.h"
#include "Artificial.h"
#include "Function.h"
#include "MemberVariable.h"
#include "BareTyped.h"
#include "IncludeProject.h"
#include "CodeScene.h"
#include "Timer.h"
#include "MainWindow.h"

#include <msSupport.h>
using namespace MarttaSupport;

namespace Martta
{

void CullActor::doCulling()
{
	TIME_FUNCTION;
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

MainWindow::MainWindow(QWidget* _p, Qt::WindowFlags _f):
	QMainWindow			(_p, _f),
	m_program			(0),
	m_updateTimer		(0)
{
	setupUi(this);
	m_codeView = codeView;

	CullManager::get()->setDelayedActor(new CullActor(this));

	QSettings s;
	restoreState(s.value("mainwindow/state").toByteArray());
	restoreGeometry(s.value("mainwindow/geometry").toByteArray());
	m_project = new Project(s.value("mainwindow/lastproject").toString());
	projectRenamed();

	
	dependencies->setModel(m_project->cDependsModel());
	dependencies->setHeader(0);
	connect(dependencies, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(showDependenciesMenu(const QPoint&)));

	connect(m_project, SIGNAL(nameChanged()), SLOT(projectRenamed()));

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	updateProgramCode();

	connect(m_project, SIGNAL(subjectInvalid()), SLOT(resetSubject()));
	resetSubject();

	connect(m_codeView, SIGNAL(currentChanged(Entity*)), SLOT(entityFocused(Entity*)));

//	classesView->setSubject(m_project->ns());

#ifdef Q_WS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
#endif

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
			ifsS += "*" + k.name().toQString().remove("Martta::") + "* ";
		else
			ifsS += k.name().toQString().remove("Martta::") + " ";
	if (ifsS.endsWith(", "))
		ifsS.chop(2);
	QTreeWidgetItem* i = new QTreeWidgetItem(_p, QStringList() << _k.name().remove("Martta::") << ifsS);
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

String compileTypes(Types const& _t)
{
	String ret;
	foreach (Type t, _t)
		ret += t->code() + L", ";
	if (ret.endsWith(L", "))
		ret.chop(2);
	return ret;
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
		t += "<td><b>" + e->kind().name().toQString() + "</b>";
		foreach (Kind k, e->parent()->allowedKinds(e->index()))
			t += "<br>" + k.name().toQString();
		t += "<br>";
		foreach (Kind k, e->parent()->deniedKinds(e->index()))
			t += "<br><i>" + k.name().toQString() + "</i>";
		t += "</td>";
		if (e->isKind<BareTyped>())
		{
			t += "<td><b><i>" + Qt::escape(e->asKind<BareTyped>()->type()->code()) + "</i><br/><font size=\"-2\" color=\"#888888\">(" + Qt::escape(e->asKind<BareTyped>()->apparentType()->code()) + ")</font></b>";
			foreach (Type i, e->asKind<BareTyped>()->ourAllowedTypes())
				t += "<br>" + Qt::escape(i->code().toQString());
			t += "<br>";
			foreach (Type i, e->asKind<BareTyped>()->ourDeniedTypes())
				t += "<br><i>" + Qt::escape(i->code().toQString()) + "</i>";
			t += "</td>";
		}
		t += "</tr></table>";
		
		int vvalue = typesVisible->verticalScrollBar()->value();
		typesVisible->clear();
		if (e->isKind<Statement>())
		{
			QTreeWidgetItem* l = new QTreeWidgetItem(typesVisible, QStringList() << QString("Local"));
			foreach (ValueDefiner* v, e->asKind<Statement>()->valuesInLocalScope())
				new QTreeWidgetItem(l, QStringList() << v->name().toQString() << v->type()->code().toQString());
		}
		if (e->hasAncestor<Class>())
		{
			QTreeWidgetItem* m = new QTreeWidgetItem(typesVisible, QStringList() << QString("Members"));
			QTreeWidgetItem* h = new QTreeWidgetItem(m, QStringList() << QString("Hidden"));
			foreach (ValueDefiner* v, castEntities<ValueDefiner>(e->ancestor<Class>()->membersOf<MemberValue>(e->hasAncestor<MemberLambda>() ? e->ancestor<MemberLambda>()->isConst() : false)))
				new QTreeWidgetItem(v->isKind<Artificial>() ? h : m, QStringList() << v->name().toQString() << v->type()->code().toQString());
		}
		QTreeWidgetItem* g = new QTreeWidgetItem(typesVisible, QStringList() << QString("General"));
		foreach (ValueDefiner* v, e->ancestor<Declaration>()->valuesKnown())
			new QTreeWidgetItem(g, QStringList() << v->name().toQString() << v->type()->code().toQString());
/*		QTreeWidgetItem* gl = new QTreeWidgetItem(typesVisible, QStringList() << QString("Global"));
		foreach (ValueDefiner* v, e->root()->childrenOf<ValueDefiner>())
			new QTreeWidgetItem(gl, QStringList() << QString(v->name()) << QString(v->type()->code()));*/
		typesVisible->expandAll();
		typesVisible->verticalScrollBar()->setValue(vvalue);
		
		vvalue = entityInfo->verticalScrollBar()->value();
		entityInfo->clear();
		
		if (Declaration* d = e->selfAncestor<Declaration>())
		{
			QTreeWidgetItem* decl = new QTreeWidgetItem(entityInfo, QStringList() << QString("Declaration Context"));
			new QTreeWidgetItem(decl, QStringList() << d->name().toQString() << d->kind().name().toQString());
			QTreeWidgetItem* ul = new QTreeWidgetItem(decl, QStringList() << QString("Utilised"));
			foreach (Declaration* u, d->utilised())
				new QTreeWidgetItem(ul, QStringList() << (u ? u->name().toQString() : QString("NULL?")) << (u ? u->kind().name().toQString() : QString("NULL?")));
			QTreeWidgetItem* us = new QTreeWidgetItem(decl, QStringList() << QString("Utilised Siblings"));
			foreach (Declaration* u, d->utilisedSiblings())
				new QTreeWidgetItem(us, QStringList() << (u ? u->name().toQString() : QString("NULL?")) << (u ? u->kind().name().toQString() : QString("NULL?")));
		}
		
		new QTreeWidgetItem(entityInfo, QStringList() << QString("Layout") << e->defineLayout(m_codeView->viewKeys(e)).toQString());
		
		QTreeWidgetItem* rc = new QTreeWidgetItem(entityInfo, QStringList() << "Child restrictions");
		foreach (int i, e->knownNames())
			if (e->allowedKinds(i).count())
				new QTreeWidgetItem(rc, QStringList() << e->indexName(i) << (compileKinds(e->allowedKinds(i)) + " (%1 req'd)").arg(e->minRequired(i)));
		for (int i = 0; i < e->minRequired(); ++i)
			new QTreeWidgetItem(rc, QStringList() << QString::number(i) << (compileKinds(e->allowedKinds(i)) + " (req'd)"));
		if (e->allowedKinds(e->minRequired()).count())
			new QTreeWidgetItem(rc, QStringList() << "..." << compileKinds(e->allowedKinds(e->minRequired())));
		

		if (TypeNamer* td = e->tryKind<TypeNamer>())
		{
			QTreeWidgetItem* te = new QTreeWidgetItem(entityInfo, QStringList() << QString("Type Namer Information") << td->type()->code());
			new QTreeWidgetItem(te, QStringList() << "Apparent type" << td->apparentType()->code());
			new QTreeWidgetItem(te, QStringList() << "Type restrictions" << (compileTypes(td->ourAllowedTypes()) + " (%1 types)").arg(td->ourAllowedTypes().size()));
			foreach (ValueDefiner* v, td->type()->applicableMembers())
				new QTreeWidgetItem(te, QStringList() << v->name().toQString() << v->type()->code().toQString());
		}
		
		if (TypedOwner* to = e->tryKind<TypedOwner>())
		{
			QTreeWidgetItem* te = new QTreeWidgetItem(entityInfo, QStringList() << QString("Typed Owner Information"));
			
			foreach (int i, e->knownNames())
				if (to->allowedTypes(i).count())
					new QTreeWidgetItem(te, QStringList() << e->indexName(i) + ": " + to->nominalType(i)->code() + " {" + to->effectiveType(i)->code() + "}" << (compileTypes(to->allowedTypes(i)) + " (%1 types)").arg(to->allowedTypes(i).size()) );
			for (int i = 0; i < max(e->minRequired(), e->cardinalChildCount()); ++i)
				if (to->allowedTypes(i).count())
					new QTreeWidgetItem(te, QStringList() << QString::number(i) + ": " + to->nominalType(i)->code().toQString() + " {" + to->effectiveType(i)->code().toQString() + "}" << (compileTypes(to->allowedTypes(i)) + " (%1 types)").arg(to->allowedTypes(i).size()) );
			if (to->allowedTypes(max(e->minRequired(), e->cardinalChildCount())).count())
				new QTreeWidgetItem(te, QStringList() << "..." << (compileTypes(to->allowedTypes(max(e->minRequired(), e->cardinalChildCount()))) + " (%1 types)").arg(to->allowedTypes(max(e->minRequired(), e->cardinalChildCount())).size()) );
		}
		
		QTreeWidgetItem* ac = new QTreeWidgetItem(entityInfo, QStringList() << "Actual children" << e->kind().name());
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

void MainWindow::updateCode(Function* _f)
{
	TIME_FUNCTION;
	if (_f == m_codeEditFunction)
		return;

	saveCode();

	if (_f)
	{
//		m_codeView->setSubject(_f);
	}
	else
	{
//		m_codeView->setSubject(0);
	}
	m_codeEditFunction = _f;
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
	m_codeView->setCurrent(m_project->program());
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
	m_codeView->setCurrent(m_project->program());
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
