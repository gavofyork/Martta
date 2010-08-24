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

#include <CQualifiers.h>
#include <CTypes.h>
#include <Concept.h>

#include "DeclarationsHandler.h"

namespace Martta
{

static inline String qs(QString const& _qs)
{
	String ret;
	ret.resize(_qs.length());
	wchar_t* d = ret.data();
	_qs.toWCharArray(d);
	ret.dataChanged(d);
	return ret;
}

QString properName(QXmlAttributes const& _a)
{
	QString ret;
	ret = _a.value("mangled").isEmpty() ? _a.value("name") : _a.value("demangled").isEmpty() ? _a.value("mangled") : _a.value("demangled");
	ret = ret.section("::", -1);
	if (_a.value("name").isEmpty() && !ret.startsWith("."))
		ret = "." + ret;
	return ret;
}

class IncomingFunctionType
{
	friend class DeclarationsHandler;

public:
	IncomingFunctionType() {}

	// Population methods.
	IncomingFunctionType(QXmlAttributes const& _a) { m_returnsId = _a.value("returns"); }
	void addArgument(QXmlAttributes const& _a) { m_argIds << _a.value("type"); }
	void addEllipsis() { m_argIds << ""; }

private:
	QString					m_returnsId;
	QStringList				m_argIds;
};

class ArrayType
{
	friend class DeclarationsHandler;

public:
	ArrayType() {}

	// Population methods.
	ArrayType(QXmlAttributes const& _a)
	{
		m_type = _a.value("type");
		m_length = _a.value("max").toInt() + 1;
	}

private:
	QString	m_type;
	int		m_length;
};

class CvQualifiedType
{
	friend class DeclarationsHandler;

public:
	CvQualifiedType() {}

	// Population methods.
	CvQualifiedType(QXmlAttributes const& _a)
	{
		m_type = _a.value("type");
		m_const = _a.value("const") == "1";
		m_qualifiers.set(Restrict, _a.value("restrict") == "1");
		m_qualifiers.set(Volatile, _a.value("volatile") == "1");
	}

private:
	QString 	m_type;
	bool		m_const;
	Qualifiers	m_qualifiers;
};

class PointerType
{
	friend class DeclarationsHandler;

public:
	PointerType() {}

	// Population methods.
	PointerType(QXmlAttributes const& _a)
	{
		m_type = _a.value("type");
	}

private:
	QString	m_type;
};

class Resolver
{
public:
	Resolver(Concept* _s, QXmlAttributes const& _a, bool _isNamed = true): m_subject(_s)
	{
		if (_isNamed)
			m_subject->adoptWithShove(Concept::evaluate(String(L"TextLabel[text=%1]").arg(qs(properName(_a)))));
	}
	virtual ~Resolver() {}

	Concept*					subject() const { return m_subject; }

	virtual void			resolve(DeclarationsHandler*) {}
	virtual bool			isType() const { return false; }

protected:
	Concept*					m_subject;
};

class FunctionResolver: public Resolver
{
public:
	FunctionResolver(Concept** _s, QXmlAttributes const& _a):
		Resolver	(*_s = Concept::evaluate(String(L"Function")), _a),
		m_returnsId	(_a.value("returns")),
		m_contextId (_a.value("context")),
		m_fileId	(_a.value("file"))
	{}

	void addArgument(QXmlAttributes const& _a)
	{
		// TODO: Will have to handle defaults for C++ stuff (i.e. know if it has a default).
		m_argIds << _a.value("type");
		m_subject->adopt(Concept::evaluate(String(L"Argument{TextLabel[name=%1]}").arg(qs(_a.value("name")))));
	}

	void addEllipsis() { Hash<String, String> p; p[L"ellipsis"] = L"true"; m_subject->setProperties(p); }

	virtual void resolve(DeclarationsHandler* _h)
	{
		m_subject->adopt(_h->resolveType(m_returnsId));
		for (int i = 0; i < m_argIds.size(); i++)
			m_subject->child(i)->adopt(_h->resolveType(m_argIds[i]));
	}

private:
	QString					m_returnsId;
	QString					m_contextId;
	QString					m_fileId;
	QStringList				m_argIds;
};

class VariableResolver: public Resolver
{
public:
	VariableResolver(Concept** _s, QXmlAttributes const& _a):
		Resolver	(*_s = Concept::evaluate(String(L"Variable[extern=%1]").arg(_a.value("extern") == "1")), _a),
		m_typeId	(_a.value("type")),
		m_contextId (_a.value("context")),
		m_fileId	(_a.value("file"))
	{}

	virtual void resolve(DeclarationsHandler* _h)
	{
		m_subject->adopt(_h->resolveType(m_typeId));
	}

protected:
	QString				m_typeId;
	QString				m_contextId;
	QString				m_fileId;
};

class EnumValueResolver: public Resolver
{
public:
	EnumValueResolver(Concept** _s, QXmlAttributes const& _a):
		Resolver(*_s = Concept::evaluate(String(L"EnumValue")), _a)
	{}
};

class TypeResolver: public Resolver
{
public:
	TypeResolver(Concept* _s, QXmlAttributes const& _a, bool _isNamed = true):
		Resolver	(_s, _a, _isNamed),
		m_id		(_a.value("id")),
		m_contextId	(_a.value("context")),
		m_fileId	(_a.value("file"))
	{
		Assert(!m_fileId.isEmpty(), "fileId must be non-null for non-fundamental types");
	}

	virtual bool isType() const { return true; }
	virtual bool isWithFields() const { return false; }

	QString id() const { return m_id; }

protected:
	QString					m_id;
	QString					m_contextId;
	QString					m_fileId;
};

class EnumerationResolver: public TypeResolver
{
public:
	EnumerationResolver(Concept** _s, QXmlAttributes const& _a):
		TypeResolver(*_s = Concept::evaluate("Enumeration"), _a, !properName(_a).startsWith("."))
	{}
};

class TypedefResolver: public TypeResolver
{
public:
	TypedefResolver(Concept** _s, QXmlAttributes const& _a):
		TypeResolver(*_s = Concept::evaluate("Typedef"), _a), m_typeId(_a.value("type"))
	{
		m_name = properName(_a);
	}

	virtual void resolve(DeclarationsHandler* _h)
	{
		TypeResolver::resolve(_h);
		Concept* td;
		m_subject->adopt(_h->resolveType(m_typeId, &td));

		if (_h->nameOfType(td) == m_name)
		{
			// Cloned struct name; make the structure anonymous.
			m_subject->adopt(td);
		}
	}

private:
	QString				m_typeId;
	QString				m_name;
};

class WithFieldsResolver: public TypeResolver
{
public:
	WithFieldsResolver(Concept* _s, QXmlAttributes const& _a): TypeResolver(_s, _a) {}

	virtual bool isWithFields() const { return true; }
};

class UnionResolver: public WithFieldsResolver
{
public:
	UnionResolver(Concept** _s, QXmlAttributes const& _a): WithFieldsResolver(*_s = Concept::evaluate("Union"), _a) {}
};

class StructResolver: public WithFieldsResolver
{
public:
	StructResolver(Concept** _s, QXmlAttributes const& _a): WithFieldsResolver(*_s = Concept::evaluate("Struct"), _a) {}
};

//////////////////////
// DeclarationsHandler

Concept* DeclarationsHandler::resolveType(QString const& _typeId, Concept** _td)
{
	if (m_types.contains(_typeId))
	{
		if (_td)
			*_td = m_types[_typeId];
		return Concept::evaluate(String("ReferencedType[key=0x%1]").arg((void*)m_types[_typeId]));
	}
	else if (m_simples.contains(_typeId))
		return Concept::evaluate(String(L"BuiltinType[id=%1]").arg(m_simples[_typeId]));
	else if (m_functionTypes.contains(_typeId))
	{
		bool ellipsis = !m_functionTypes[_typeId]->m_argIds.isEmpty() && m_functionTypes[_typeId]->m_argIds.last().isEmpty();
		Concept* r = resolveType(m_functionTypes[_typeId]->m_returnsId);
		r = r->insert(Concept::evaluate(String(L"FunctionType[ellipsis=%1]").arg(ellipsis)));
		foreach(QString i, m_functionTypes[_typeId]->m_argIds)
			if (!i.isEmpty())
				r->back().place(resolveType(i));
			else
			{
				AssertEqNR(r->cardinalChildCount(), m_functionTypes[_typeId]->m_argIds.count() - 1)
			}
		return r;
	}
	else if (m_pointers.contains(_typeId))
		return resolveType(m_pointers[_typeId]->m_type)->insert(Concept::evaluate(L"Pointer"));
	else if (m_cvQualifieds.contains(_typeId))
	{
		Concept* r = resolveType(m_cvQualifieds[_typeId]->m_type);
		if (m_cvQualifieds[_typeId]->m_const)
			r = r->insert(Concept::evaluate(L"Const"));
		return r;
	}
	else if (m_arrays.contains(_typeId))
	{
		Concept* r = resolveType(m_arrays[_typeId]->m_type);
		if (m_arrays[_typeId]->m_length)
			r = r->insert(Concept::evaluate(String(L"Array{IntegerLiteral[value=%1]}").arg(m_arrays[_typeId]->m_length)));
		else
			r = r->insert(Concept::evaluate(L"Array"));
		return r;
	}
	qCritical("Couldn't resolve type (%s)!", _typeId.toLatin1().data());
	return 0;
}

bool DeclarationsHandler::startDocument()
{
	m_lastEnum = 0;
	m_lastFunction = 0;
	m_lastIncomingFunctionType = 0;
	return true;
}

bool DeclarationsHandler::startElement(QString const&, QString const& _n, QString const&, QXmlAttributes const& _a)
{
	QString conId = _a.value("context");
	AssertNR(conId.isEmpty() || m_contexts.contains(conId));
	Concept* parent = m_contexts[conId];
	AssertNR(conId.isEmpty() || parent);

	if (false) {}
	else if (_n == "PointerType")
		m_pointers[_a.value("id")] = new PointerType(_a);
	else if (_n == "ArrayType")
		m_arrays[_a.value("id")] = new ArrayType(_a);
	else if (_n == "CvQualifiedType")
		m_cvQualifieds[_a.value("id")] = new CvQualifiedType(_a);
	else if (_n == "FundamentalType")
	{
		QStringList n = _a.value("name").split(" ");
		int s = 0;

		if (n.contains("signed")) s |= Signed;
		else if (n.contains("unsigned")) s |= Unsigned;
		else if (n.contains("complex")) s |= Complex;

		if (n.contains("short")) s |= Short;
		else if (n.count("long") == 1) s |= Long;
		else if (n.count("long") == 2) s |= Longlong;

		if (n.contains("char")) s |= Char;
		else if (n.contains("float")) s |= Float;
		else if (n.contains("double")) s |= Double;
		else if (n.contains("wchar_t")) s |= Wchar;
		else if (n.contains("int") || n.contains("signed") || n.contains("unsigned") || n.contains("short") || n.contains("long")) s |= Int;

		m_simples[_a.value("id")] = s;
	}
	else if (_n == "Argument")
	{
		if (m_lastFunction)
			m_lastFunction->addArgument(_a);
		else if (m_lastIncomingFunctionType)
			m_lastIncomingFunctionType->addArgument(_a);
	}
	else if (_n == "Field")
	{
		AssertNR(parent);
		foreach(Resolver* i, m_resolvers)
		{
			bool it = i->isType();
			if (it)
			{
				TypeResolver* tr = static_cast<TypeResolver*>(i);
				QString cid = tr->id();
				if (cid == conId)
				{
					bool iwf = tr->isWithFields();
					(void)(iwf);
					AssertNR(iwf);
					Concept* m;
					m_resolvers << new VariableResolver(&m, _a);
					parent->adopt(m);
					goto OK;
				}
			}
		}
		qCritical("Couldn't find with-fields resolver context %s for %s.", _a.value("context").toLatin1().data(), _a.value("id").toLatin1().data());
		OK:;
	}
	else if (_n == "EnumValue")
	{
		AssertNR(m_lastEnum);
		Concept* v;
		m_resolvers << new EnumValueResolver(&v, _a);
		m_lastEnum->back().place(v);
	}
	else if (_n == "Function" && (!_a.value("mangled").isEmpty() || !_a.value("name").startsWith("_")))
	{
		AssertNR(parent);
		m_resolvers << (m_lastFunction = new FunctionResolver(&m_functions[_a.value("id")], _a));
		parent->adopt(m_functions[_a.value("id")]);
		m_contexts[_a.value("id")] = m_functions[_a.value("id")];
	}
	else if (_n == "FunctionType")
	{
		m_lastIncomingFunctionType = new IncomingFunctionType(_a);
		m_functionTypes[_a.value("id")] = m_lastIncomingFunctionType;
	}
	else if (_n == "Ellipsis")
	{
		if (m_lastFunction)
			m_lastFunction->addEllipsis();
		else if (m_lastIncomingFunctionType)
			m_lastIncomingFunctionType->addEllipsis();
	}
	else if (_n == "Typedef")
	{
		m_resolvers << new TypedefResolver(&m_types[_a.value("id")], _a);
		parent->adopt(m_types[_a.value("id")]);
		m_contexts[_a.value("id")] = m_types[_a.value("id")];
		m_typeNames[m_types[_a.value("id")]] = properName(_a);
	}
	else if (_n == "Enumeration")
	{
		m_resolvers << new EnumerationResolver(&m_lastEnum, _a);
		parent->adopt(m_lastEnum);
		m_types[_a.value("id")] = m_lastEnum;
		m_typeNames[m_types[_a.value("id")]] = properName(_a);
	}
	else if (_n == "Struct")
	{
		m_resolvers << new StructResolver(&m_types[_a.value("id")], _a);
		parent->adopt(m_types[_a.value("id")]);
		m_contexts[_a.value("id")] = m_types[_a.value("id")];
		m_typeNames[m_types[_a.value("id")]] = properName(_a);
	}
	else if (_n == "Union")
	{
		m_resolvers << new UnionResolver(&m_types[_a.value("id")], _a);
		parent->adopt(m_types[_a.value("id")]);
		m_contexts[_a.value("id")] = m_types[_a.value("id")];
		m_typeNames[m_types[_a.value("id")]] = properName(_a);
	}
	else if (_n == "Variable" && (!_a.value("mangled").isEmpty() || !_a.value("name").startsWith("_")))
	{
		m_resolvers << new VariableResolver(&m_variables[_a.value("id")], _a);
		parent->adopt(m_variables[_a.value("id")]);
		m_contexts[_a.value("id")] = m_variables[_a.value("id")];
	}
	else if (_n == "Namespace")
	{
		if (_a.value("name") == "::")
			m_contexts[_a.value("id")] = m_d;
		else
			AssertNR(_a.value("members").isEmpty());
	}
	return true;
}

bool DeclarationsHandler::endElement(QString const&, QString const& _n, QString const&)
{
	if (_n == "Function")
		m_lastFunction = 0;
	else if (_n == "FunctionType")
		m_lastIncomingFunctionType = 0;
	else if (_n == "Enumeration")
		m_lastEnum = 0;
	return true;
}

bool DeclarationsHandler::endDocument()
{
#if defined(DEBUG)
	mInfo() << s_news << "total allocations" << s_deletes << "total deallocations.";
#endif
	// Resolve all types.
	foreach(Resolver* f, m_resolvers)
	{
		f->resolve(this);
		delete f;
	}

	{
		// Clean up all temporaries that the resolution process depended on.
		foreach(ArrayType* i, m_arrays.values())
			delete i;
		m_arrays.clear();
		foreach(CvQualifiedType* i, m_cvQualifieds.values())
			delete i;
		m_cvQualifieds.clear();
		foreach(PointerType* i, m_pointers.values())
			delete i;
		m_pointers.clear();
		foreach(IncomingFunctionType* i, m_functionTypes.values())
			delete i;
		m_functionTypes.clear();
	}
	return true;
}

}
