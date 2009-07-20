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

#include <msList.h>
#undef inline
using namespace MarttaSupport;

class QDomDocument;
class QProcess;

namespace Martta
{

class Entity;
class ProjectIncludesModel;
class IncludeProject;

class Project: public QObject
{
	Q_OBJECT

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
	List<IncludeProject*>		cDepends() const { return m_cDepends; }
	void 						rename(QString const& _filename) { m_filename = _filename; nameChanged(); }

	Entity*						ns() { return m_namespace; }
	Entity const*				ns() const { return m_namespace; }
	Entity*						program() const { return m_program; }
	QString						executable();
	QString						lastCompileError() const { return m_lastCompileError; }

	Entity const*				root() const { return m_root; }
	Entity*						root() { return m_root; }

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
	class CDepends: public QAbstractItemModel, public List<IncludeProject*>
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

	Entity*						m_program;			///< Ptr to the program class.

	// State
	Entity*						m_root;				///< Our full program model. Merged with all other include/projects.

	Entity*						m_namespace;		///< Our namespace. All the project's stuff goes under here. Probably a better alternative to m_classes.

	// Temporaries
	bool						m_alteringDepends;	///< True whenever we don't want changes to the CDepends model implicitly causing a reset (and thus ser./deser.).
	QProcess*					m_compiler;
	QString						m_lastCompileError;
	QString						m_tempPath;
	QString						m_supportPath;
};

}

#endif
