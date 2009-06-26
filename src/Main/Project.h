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

#ifndef MARTTAPROJECT_H
#define MARTTAPROJECT_H

#include <QObject>
#include <QAbstractItemModel>

#include "NamespaceEntity.h"
#include "Root.h"
#include "Method.h"
#include "Class.h"

class QDomDocument;
class QProcess;

namespace Martta
{

class ProjectIncludesModel;
class IncludeProject;

class Project: public QObject
{
	Q_OBJECT

	friend class Classes;
	friend class ProjectIncludesModel;

public:
	enum { Types = 0, Functions, Variables, Includes, All, Max };
	enum { HeadingRole = Qt::UserRole, ItemRole, OwnerRole };

	Project(QString const& _load = QString());
	~Project();

	QString						name() const { return m_filename.isEmpty() ? "(Untitled)" : m_filename.section("/", -1).replace(".project", ""); }
#ifdef WIN32
	QString						exeName() const { return name() + ".exe"; }
#else
	QString						exeName() const { return name(); }
#endif
	QString						filename() const { return m_filename; }
	QAbstractItemModel*			cDependsModel() { return &m_cDepends; }
	QAbstractItemModel*			classesModel() { return &m_classes; }
	QList<IncludeProject*>		cDepends() const { return m_cDepends; }
	QList<Class*>				classes() const { return m_classes; }
	void 						rename(QString const& _filename) { m_filename = _filename; nameChanged(); }

	NamespaceEntity*			ns() { return m_namespace; }
	NamespaceEntity const*		ns() const { return m_namespace; }
	Method*						program() const { return m_program; }
	Class*						programClass() const { return m_program->parent()->asKind<Class>(); }
	QString						executable();
	QString						lastCompileError() const { return m_lastCompileError; }
	Root const*			root() const { return &m_declarations; }
	Root*					root() { return &m_declarations; }

	void						serialise(QDomDocument& _d) const;
	void						deserialise(QDomDocument& _d);

public slots:
	void resetAsNew();
	void open(QString const& _filename) { clear(); rename(_filename); revert(); }
	void revert();
	void save() const;
	void build();

	void clear();
	void triggerChange() { changed(); }

private slots:
	void reloadHeaders();	// don't forget to update any subjects after this! 
	void reloadHeadersAndUpdateSubject() { reloadHeaders(); emit subjectInvalid(); }
	void stopCompile();
	void compileFinished();

signals:
	void subjectInvalid();
	void changed();
	void nameChanged();

public:
	QString						code() const;

	// Properties
	class CDepends: public QAbstractItemModel, public QList<IncludeProject*>
	{
	public:
		void 					reset() { QAbstractItemModel::reset(); }
	private:
		virtual bool			insertRows(int _r, int _c, QModelIndex const& _p = QModelIndex());
		virtual bool			removeRows(int _r, int _c, QModelIndex const& _p = QModelIndex());
		virtual QVariant		headerData(int, Qt::Orientation, int = Qt::DisplayRole) const { return 0; }
		virtual int				columnCount(QModelIndex const& = QModelIndex()) const { return 1; }
		virtual QVariant		data(QModelIndex const& _i, int _r) const;
		virtual bool			setData(QModelIndex const& _i, QVariant const& _v, int _r = Qt::EditRole);
		virtual Qt::ItemFlags	flags(QModelIndex const& _i) const;
		virtual int				rowCount(QModelIndex const& _i = QModelIndex()) const;
		virtual QModelIndex		index(int _r, int _c, QModelIndex const& _p = QModelIndex()) const;
		virtual QModelIndex		parent(QModelIndex const& _i) const;
		IncludeProject*			checkHeading(QModelIndex _i, int _kind) const;
		IncludeProject*			checkItem(QModelIndex _i, int _kind) const;
		IncludeProject*			checkProject(QModelIndex _i) const;
		bool					checkRoot(QModelIndex _i) const { return _i == QModelIndex(); }
	} m_cDepends;
	QString						m_filename;

	// Our language entities. These may be found in the top level of this project's namespace. They
	// do not include the classes internal to other classes or namespaces (although the latter is
	// moot anyway since we're only allowing one namespace per project). These could be included
	// (but scoped accordingly).
	//
	// NOTE: These can also be found in the program model (m_declarations), but that is a
	// fully merged program model, and so combines them with other projects and include projects.
	//
	// These are for reference only; the real "owner" is the parent entity in the language
	// model. It is used and maintained just to keep Qt's model/view system happy.
	class Classes: public QAbstractItemModel, public QList<Class*>
	{
	public:
		void 					reset() { QAbstractItemModel::reset(); }
	private:
		virtual bool			insertRows(int _r, int _c, QModelIndex const& _p = QModelIndex());
		virtual bool			removeRows(int _r, int _c, QModelIndex const& _p = QModelIndex());
		virtual QVariant		headerData(int, Qt::Orientation, int = Qt::DisplayRole) const { return 0; }
		virtual int				columnCount(QModelIndex const& = QModelIndex()) const { return 1; }
		virtual QVariant		data(QModelIndex const& _i, int _r) const;
		virtual bool			setData(QModelIndex const& _i, QVariant const& _v, int _r = Qt::EditRole);
		virtual Qt::ItemFlags	flags(QModelIndex const& _i) const;
		virtual int				rowCount(QModelIndex const& _i = QModelIndex()) const;
		virtual QModelIndex		index(int _r, int _c, QModelIndex const& _p = QModelIndex()) const;
		virtual QModelIndex		parent(QModelIndex const& _i) const;
		bool					checkRoot(QModelIndex _i) const { return _i == QModelIndex(); }
	} m_classes;
	Method*						m_program;			///< Ptr to the program class.

	// State
	Root					m_declarations;		///< Our full program model. Merged with all other include/projects.

	NamespaceEntity*			m_namespace;		///< Our namespace. All the project's stuff goes under here. Probably a better alternative to m_classes.

	// Temporaries
	bool						m_alteringDepends;	///< True whenever we don't want changes to the CDepends model implicitly causing a reset (and thus ser./deser.).
	QProcess*					m_compiler;
	QString						m_lastCompileError;
	QString						m_tempPath;
	QString						m_supportPath;
};

}

#endif
