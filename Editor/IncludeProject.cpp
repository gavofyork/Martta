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

#include <QtXml>

#include <Entity.h>
#undef inline

#include "DeclarationFile.h"
#include "IncludeProject.h"

namespace Martta
{

QString IncludeProject::code() const
{
	QString ret;
	foreach (QString i, m_includes)
		ret += ("#include \"" + i + "\"\n");
	return ret;
}

QStringList IncludeProject::args() const
{
	QStringList ret;
	foreach (QString i, m_libs)
		ret << "-l" + i;
	return ret;
}

void IncludeProject::assimilate(DeclarationFile* _f)
{
	m_types << _f->m_types;
	m_variables << _f->m_variables;
	m_functions << _f->m_functions;
	_f->m_types.clear();
	_f->m_variables.clear();
	_f->m_functions.clear();
}

void slurp(QDomElement& _p, QString const& _e, QStringList const& _l)
{
	foreach (QString s, _l)
	{
		QDomElement q = _p.ownerDocument().createElement(_e);
		q.appendChild(q.ownerDocument().createTextNode(s));
		_p.appendChild(q);
	}
}

QStringList spurt(QDomElement const& _p, QString const& _e)
{
	QStringList ret;
	for (QDomNode e = _p.firstChild(); !e.isNull(); e = e.nextSibling())
		if (e.isElement() && e.nodeName() == _e)
			ret += e.toElement().text();
	return ret;
}

void IncludeProject::exportDom(QDomElement& _context) const
{
	QDomElement p = _context.ownerDocument().createElement("includeproject");
	p.setAttribute("name", m_name);
	slurp(p, "include", m_includes);
	slurp(p, "lib", m_libs);
	_context.appendChild(p);
}

void IncludeProject::importDom(QDomElement const& _element)
{
	m_name = _element.attribute("name");
	m_includes = spurt(_element, "include");
	m_libs = spurt(_element, "lib");
}

}
