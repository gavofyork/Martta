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

#include "Project.h"
#include "Artificial.h"
#include "Function.h"
#include "MemberVariable.h"
#include "BareTyped.h"
#include "IncludeProject.h"
#include "CodeScene.h"
#include "MainWindow.h"

namespace Martta
{

MainWindow::MainWindow(QWidget* _p, Qt::WindowFlags _f):
	QMainWindow			(_p, _f),
	m_program			(0)
{
	setupUi(this);
	m_codeScene = codeView;

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

	connect(m_project, SIGNAL(changed()), SLOT(updateProgramCode()));
	updateProgramCode();

	connect(m_project, SIGNAL(subjectInvalid()), SLOT(resetSubject()));
	resetSubject();

	connect(m_codeScene, SIGNAL(currentChanged(Entity*)), SLOT(entityFocused(Entity*)));

//	classesView->setSubject(m_project->ns());

#ifdef Q_WS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
#endif

	updateLanguage();
}

MainWindow::~MainWindow()
{
	confirmLose();
	QSettings s;
	s.setValue("mainwindow/state", saveState());
	s.setValue("mainwindow/geometry", saveGeometry());
	s.setValue("mainwindow/lastproject", m_project->filename());
}

template<class T> void addToLanguage(Kind const& _k, T* _p)
{
	QString ifsS;
	foreach (Kind k, _k.interfaces())
		if (_k.immediateInterfaces().contains(k))
			ifsS += "*" + k.name().remove("Martta::") + "* ";
		else
			ifsS += k.name().remove("Martta::") + " ";
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
	m_codeScene->setSubject(m_project->ns());
	M_ASSERT(!m_codeScene->current() || m_codeScene->current()->rootEntity() == m_project->root());
	entityFocused(m_codeScene->current());
}

void MainWindow::on_actAboutMartta_triggered()
{
	QMessageBox::about(this, "About Martta", "<center><font size=+4>Martta</font><br/><font size=-2><b>Technology Preview M0</b></font></center><font size=-2><p>Copyright (c) quid pro code Ltd. (UK)</p></font><font size=-1>Martta, the C++-based Extensible Quasi-Graphical Meta-Language.<br>This program is released according to the GNU GPL and is therefore Free Software: For details on how this program and derivatives thereof may be distributed, please see COPYING or visit <a href=\"http://quidprocode.co.uk/martta/\">quidprocode.co.uk/Martta</a>.</font>");
}

void MainWindow::on_actAboutQt_triggered()
{
	QMessageBox::aboutQt(this);
}

void MainWindow::entityFocused(Entity* _e)
{
	if (m_codeScene->current() != _e)
		m_codeScene->setCurrent(_e);

	QString t;
	if (_e && _e->parent())
	{
		t += "<table><tr>";
		t += "<td><b>" + _e->kind().name() + "</b>";
		foreach (Kind k, _e->parent()->allowedKinds(_e->index()))
			t += "<br>" + k.name();
		t += "<br>";
		foreach (Kind k, _e->parent()->deniedKinds(_e->index()))
			t += "<br><i>" + k.name() + "</i>";
		t += "</td>";
		if (_e->isKind<BareTyped>())
		{
			t += "<td><b><i>" + Qt::escape(_e->asKind<BareTyped>()->type()->code()) + "</i><br/><font size=\"-2\" color=\"#888888\">(" + Qt::escape(_e->asKind<BareTyped>()->apparentType()->code()) + ")</font></b>";
			foreach (Type i, _e->asKind<BareTyped>()->allowedTypes())
				t += "<br>" + Qt::escape(i->code());
			t += "<br>";
			foreach (Type i, _e->asKind<BareTyped>()->deniedTypes())
				t += "<br><i>" + Qt::escape(i->code()) + "</i>";
			t += "</td>";
		}
		t += "</tr></table>";
		
		int vvalue = typesVisible->verticalScrollBar()->value();
		typesVisible->clear();
		if (_e->isKind<Statement>())
		{
			QTreeWidgetItem* l = new QTreeWidgetItem(typesVisible, QStringList() << QString("Local"));
			foreach (ValueDefiner* v, _e->asKind<Statement>()->valuesInLocalScope())
				new QTreeWidgetItem(l, QStringList() << QString(v->name()) << QString(v->type()->code()));
		}
		if (_e->hasAncestor<Class>())
		{
			QTreeWidgetItem* m = new QTreeWidgetItem(typesVisible, QStringList() << QString("Members"));
			QTreeWidgetItem* h = new QTreeWidgetItem(m, QStringList() << QString("Hidden"));
			foreach (ValueDefiner* v, castEntities<ValueDefiner>(_e->ancestor<Class>()->membersOf<MemberValue>(_e->hasAncestor<MemberLambda>() ? _e->ancestor<MemberLambda>()->isConst() : false)))
				new QTreeWidgetItem(v->isKind<Artificial>() ? h : m, QStringList() << QString(v->name()) << QString(v->type()->code()));
		}
		QTreeWidgetItem* g = new QTreeWidgetItem(typesVisible, QStringList() << QString("General"));
		foreach (ValueDefiner* v, _e->ancestor<DeclarationEntity>()->valuesKnown())
			new QTreeWidgetItem(g, QStringList() << QString(v->name()) << QString(v->type()->code()));
		QTreeWidgetItem* gl = new QTreeWidgetItem(typesVisible, QStringList() << QString("Global"));
		foreach (ValueDefiner* v, _e->rootEntity()->childrenHereAndBeforeOf<ValueDefiner>())
			new QTreeWidgetItem(gl, QStringList() << QString(v->name()) << QString(v->type()->code()));
		typesVisible->expandAll();
		typesVisible->verticalScrollBar()->setValue(vvalue);
		
		vvalue = entityInfo->verticalScrollBar()->value();
		entityInfo->clear();
		if (DeclarationEntity* d = _e->selfAncestor<DeclarationEntity>())
		{
			QTreeWidgetItem* decl = new QTreeWidgetItem(entityInfo, QStringList() << QString("Declaration Context"));
			new QTreeWidgetItem(decl, QStringList() << QString(d->name()) << QString(d->kind().name()));
			QTreeWidgetItem* ul = new QTreeWidgetItem(decl, QStringList() << QString("Utilised"));
			foreach (DeclarationEntity* u, d->utilised())
				new QTreeWidgetItem(ul, QStringList() << QString(u ? u->name() : "NULL?") << QString(u ? u->kind().name() : "NULL?"));
			QTreeWidgetItem* us = new QTreeWidgetItem(decl, QStringList() << QString("Utilised Siblings"));
			foreach (DeclarationEntity* u, d->utilisedSiblings())
				new QTreeWidgetItem(us, QStringList() << QString(u ? u->name() : "NULL?") << QString(u ? u->kind().name() : "NULL?"));
		}
		new QTreeWidgetItem(entityInfo, QStringList() << QString("Layout") << QString(_e->defineLayout(m_codeScene->viewKeys(_e))));
		
		if (BareTyped* td = _e->tryKind<BareTyped>())
		{
			QTreeWidgetItem* te = new QTreeWidgetItem(entityInfo, QStringList() << QString("Type Information") << td->type()->code());
			foreach (ValueDefiner* v, td->type()->applicableMembers())
				new QTreeWidgetItem(te, QStringList() << QString(v->name()) << QString(v->type()->code()));
		}
		
		entityInfo->expandAll();
		entityInfo->verticalScrollBar()->setValue(vvalue);
	}
	else
		t += "Nothing selected.";
	informationView->setText(t);
}

void MainWindow::on_actShowDeps_triggered()
{
	codeView->setShowDependencyInfo(actShowDeps->isChecked());
}

void MainWindow::on_actCastability_triggered()
{
	g_debugCastability = actCastability->isChecked();
}

void MainWindow::updateCode(Function* _f)
{
	TIME_FUNCTION;
	if (_f == m_codeEditFunction)
		return;

	saveCode();

	if (_f)
	{
//		m_codeScene->setSubject(_f);
	}
	else
	{
//		m_codeScene->setSubject(0);
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
//	m_codeScene->setSubject(reinterpret_cast<Class*>(_i.data(Project::OwnerRole).value<void*>()));
}

void MainWindow::on_actNewProject_triggered()
{
	if (!confirmLose()) return;

	m_codeScene->setSubject(0);
	m_project->resetAsNew();
	m_codeScene->setSubject(m_project->ns());
	m_codeScene->setCurrent(m_project->program());
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
	if (s_changes.size())
		s_changes.removeFirst();
	codeView->update();
}

void MainWindow::on_actClearChanges_triggered()
{
	clearChanges();
	codeView->update();
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

	QString f = QFileDialog::getOpenFileName(this, "Open Project", "/home/gav", "*.project");
	if (f.isEmpty()) return;

	m_codeScene->setSubject(0);
	m_project->open(f);
	m_codeScene->setSubject(m_project->ns());
	m_codeScene->setCurrent(m_project->program());
}

void MainWindow::on_actSave_triggered()
{
	saveCode();
	if (m_project->filename().isEmpty())
		m_project->rename(QFileDialog::getSaveFileName(this, "Save Project", "/home/gav", "*.project"));
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
	m_codeScene->setEditing(0);
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
