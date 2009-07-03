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

#include "IntegerLiteral.h"
#include "StringLiteral.h"
#include "TypedOwner.h"
#include "Compound.h"
#include "Typedef.h"
#include "Enumeration.h"
#include "EnumValue.h"
#include "Union.h"
#include "Struct.h"
#include "Class.h"
#include "Variable.h"
#include "Argument.h"
#include "Function.h"
#include "Namespace.h"
#include "Type.h"
#include "Array.h"
#include "Const.h"
#include "Pointer.h"
#include "SimpleType.h"
#include "ExplicitType.h"
#include "FunctionType.h"
#include "Memberify.h"
#include "Reference.h"
#include "TextLabel.h"
#include "DeclarationsHandler.h"

namespace Martta
{

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
		setFlag(m_qualifiers, Restrict, _a.value("restrict") == "1");
		setFlag(m_qualifiers, Volatile, _a.value("volatile") == "1");
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
	virtual ~Resolver() {}
	virtual void resolve(DeclarationsHandler* _h) = 0;
	virtual bool isType() const { return false; }
};

class FunctionResolver: public Resolver
{
public:
	FunctionResolver(Function* _s, QXmlAttributes const& _a);
	void addArgument(QXmlAttributes const& _a);
	void addEllipsis() { m_subject->setEllipsis(true); }

	virtual void resolve(DeclarationsHandler* _h);

private:
	Function*				m_subject;

	QString					m_returnsId;
	QString					m_contextId;
	QString					m_fileId;
	QList<QString>			m_argIds;
};

class VariableResolver: public Resolver
{
public:
	VariableResolver(Variable* _s, QXmlAttributes const& _a);

	virtual void resolve(DeclarationsHandler* _h);

protected:
	Variable*			m_subject;

	QString				m_typeId;
	QString				m_contextId;
	QString				m_fileId;
};

class EnumValueResolver: public Resolver
{
public:
	EnumValueResolver(EnumValue* _s, QXmlAttributes const& _a);

	virtual void resolve(DeclarationsHandler*) {}
};

class TypeResolver: public Resolver
{
public:
	virtual void init(QXmlAttributes const& _a);
	virtual void resolve(DeclarationsHandler* _h);
	virtual bool isType() const { return true; }
	virtual bool isWithFields() const { return false; }

	QString id() const { return m_id; }

protected:
	virtual TopLevelType*	subject() const = 0;

	QString					m_id;
	QString					m_contextId;
	QString					m_fileId;
};

template<class T> // T must be TopLevelType-derived.
class SimpleResolver: public TypeResolver
{
public:
	SimpleResolver(T* _s): m_subject(_s) {}

protected:
	virtual TopLevelType*	subject() const { return m_subject; }
	T*		m_subject;
};

class EnumerationResolver: public SimpleResolver<Enumeration>
{
public:
	EnumerationResolver(Enumeration* _s, QXmlAttributes const& _a): SimpleResolver<Enumeration>(_s) { init(_a); }
};

class TypedefResolver: public SimpleResolver<Typedef>
{
public:
	TypedefResolver(Typedef* _s, QXmlAttributes const& _a): SimpleResolver<Typedef>(_s) { init(_a); }

	virtual void init(QXmlAttributes const& _a);
	virtual void resolve(DeclarationsHandler* _h);

private:
	QString				m_typeId;
};

class WithFieldsResolver: public TypeResolver
{
public:
	virtual bool isWithFields() const { return true; }
};

template<class T> // T must have m_members member.
class WithFieldsSimpleResolver: public WithFieldsResolver
{
public:
	WithFieldsSimpleResolver(T* _s): m_subject(_s) {}

	virtual void resolve(DeclarationsHandler* _h);

protected:
	virtual TopLevelType*	subject() const { return m_subject; }

private:
	T*			m_subject;
};

class UnionResolver: public WithFieldsSimpleResolver<Union>
{
public:
	UnionResolver(Union* _s, QXmlAttributes const& _a): WithFieldsSimpleResolver<Union>(_s) { init(_a); }
};

class StructResolver: public WithFieldsSimpleResolver<Struct>
{
public:
	StructResolver(Struct* _s, QXmlAttributes const& _a): WithFieldsSimpleResolver<Struct>(_s) { init(_a); }
};

QString properName(QXmlAttributes const& _a)
{
	QString ret;
	ret = _a.value("mangled").isEmpty() ? _a.value("name") : _a.value("demangled").isEmpty() ? _a.value("mangled") : _a.value("demangled");
	ret = ret.section("::", -1);
	if (_a.value("name").isEmpty() && !ret.startsWith("."))
		ret = "." + ret;
	return ret;
}

EnumValueResolver::EnumValueResolver(EnumValue* _s, QXmlAttributes const& _a)
{
	_s->middle(Identifiable::Identity).place(new TextLabel(properName(_a)));
}

FunctionResolver::FunctionResolver(Function* _s, QXmlAttributes const& _a):
	m_subject(_s)
{
	_s->middle(Identifiable::Identity).place(new TextLabel(properName(_a)));
//	setFlag(m_subject->m_qualifiers, Extern, _a.value("extern") == "1");
	m_subject->m_location.m_lineNumber = _a.value("line").toInt();

	m_returnsId = _a.value("returns");
	m_contextId = _a.value("context");
	m_fileId = _a.value("file");
}

void FunctionResolver::addArgument(QXmlAttributes const& _a)
{
	// TODO: Will have to handle defaults for C++ stuff (i.e. know if it has a default).
	m_argIds << _a.value("type");
	Argument* v = new Argument;
	m_subject->back().place(v);
	v->middle(Identifiable::Identity).place(new TextLabel(_a.value("name")));
}

void FunctionResolver::resolve(DeclarationsHandler* _h)
{
	// at position 2 it's just a placeholder so this will work ok.
	m_subject->middle(Function::Returned).place(_h->resolveType(m_returnsId));

	for (int i = 0; i < m_argIds.size(); i++)
		m_subject->argument(i)->middle(VariableNamer::OurType).place(_h->resolveType(m_argIds[i]));
	m_subject->m_location.m_file = _h->commitToFile(m_fileId, m_subject);
}

VariableResolver::VariableResolver(Variable* _s, QXmlAttributes const& _a):
	m_subject(_s)
{
	_s->middle(Identifiable::Identity).place(new TextLabel(properName(_a)));
	setFlag(m_subject->m_qualifiers, Extern, _a.value("extern") == "1");
	m_subject->m_location.m_lineNumber = _a.value("line").toInt();

	m_typeId = _a.value("type");
	m_contextId = _a.value("context");
	m_fileId = _a.value("file");
}

void VariableResolver::resolve(DeclarationsHandler* _h)
{
	m_subject->middle(VariableNamer::OurType).place(_h->resolveType(m_typeId));
	if (!m_subject->parent()->parent())
		m_subject->m_location.m_file = _h->commitToFile(m_fileId, m_subject);
}

void TypeResolver::init(QXmlAttributes const& _a)
{
	if (properName(_a).startsWith(".") && subject()->isKind<Enumeration>())
	{
		// Don't do anything - we're an anonymous enum.
	}
	else
		subject()->middle(Identifiable::Identity).place(new TextLabel(properName(_a)));
	subject()->m_location.m_lineNumber = _a.value("line").toInt();

	m_id = _a.value("id");
	m_contextId = _a.value("context");
	m_fileId = _a.value("file");
	if (m_fileId.isEmpty())
		qCritical("NULL fileId for non-fundamental type %s", subject()->name().toCString());
}

void TypeResolver::resolve(DeclarationsHandler* _h)
{
	if (!subject()->parent()->parent())
		subject()->m_location.m_file = _h->commitToFile(m_fileId, subject());
}

void TypedefResolver::init(QXmlAttributes const& _a)
{
	TypeResolver::init(_a);
	m_typeId = _a.value("type");
}

void TypedefResolver::resolve(DeclarationsHandler* _h)
{
	TypeResolver::resolve(_h);
	m_subject->middle(Typedef::Aliased).place(_h->resolveType(m_typeId));
	if (ExplicitType* e = m_subject->tryChild<ExplicitType>(Typedef::Aliased))
		if (Struct* s = e->subject()->tryKind<Struct>())
			if (s->codeName() == m_subject->codeName())
			{
				// Cloned struct name; make the structure anonymous.
				s->silentMove(m_subject->middle(Typedef::ShadowedStruct));
				_h->removeFromFile(s);
			}
}

template<class T>
void WithFieldsSimpleResolver<T>::resolve(DeclarationsHandler* _h)
{
	TypeResolver::resolve(_h);
}

//////////////////////
// DeclarationsHandler

TypeEntity* DeclarationsHandler::resolveType(QString const& _typeId)
{
	if (m_types.contains(_typeId))
	{
		return new ExplicitType(m_types[_typeId]);
	}
	else if (m_simples.contains(_typeId))
	{
		return new SimpleType(m_simples[_typeId]);
	}
	else if (m_functionTypes.contains(_typeId))
	{
		FunctionType* q = new FunctionType;
		q->middle(FunctionType::Returned).place(resolveType(m_functionTypes[_typeId]->m_returnsId));
		foreach(QString i, m_functionTypes[_typeId]->m_argIds)
			if (i.isEmpty())
				q->setEllipsis();
		else
			q->back().place(resolveType(i));
		return q;
	}
	else if (m_pointers.contains(_typeId))
	{
		TypeEntity* r = new Pointer;
		r->middle(ModifyingType::Original).place(resolveType(m_pointers[_typeId]->m_type));
		return r;
	}
	else if (m_cvQualifieds.contains(_typeId))
	{
		TypeEntity* r;
		if (m_cvQualifieds[_typeId]->m_const)
			(r = new Const)->middle(ModifyingType::Original).place(resolveType(m_cvQualifieds[_typeId]->m_type));
		else
			r = resolveType(m_cvQualifieds[_typeId]->m_type);
		return r;
	}
	else if (m_arrays.contains(_typeId))
	{
		TypeEntity* r = new Array;
		r->middle(ModifyingType::Original).place(resolveType(m_arrays[_typeId]->m_type));
		if (m_arrays[_typeId]->m_length)
			r->middle(Array::Length).place(new IntegerLiteral(m_arrays[_typeId]->m_length));
		return r;
	}
	qCritical("Couldn't resolve type (%s)!", _typeId.toLatin1().data());
	return TypeEntity::null;
}

QString DeclarationsHandler::commitToFile(QString const& _fileId, Function* _f)
{
	M_ASSERT(m_files.contains(_fileId));
	m_files[_fileId]->m_functions << _f;
	return m_files[_fileId]->m_filename;
}

QString DeclarationsHandler::commitToFile(QString const& _fileId, Variable* _f)
{
	M_ASSERT(m_files.contains(_fileId));
	m_files[_fileId]->m_variables << _f;
	return m_files[_fileId]->m_filename;
}

QString DeclarationsHandler::commitToFile(QString const& _fileId, TopLevelType* _f)
{
	M_ASSERT(m_files.contains(_fileId));
	m_files[_fileId]->m_types << _f;
	return m_files[_fileId]->m_filename;
}

void DeclarationsHandler::removeFromFile(TopLevelType* _e)
{
	foreach (DeclarationFile* f, m_files.values())
		f->m_types.removeAll(_e);
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
	M_ASSERT(conId.isEmpty() || m_contexts.contains(conId));
	Declaration* con = m_contexts[conId];

	if (false) {}
	else if (_n == "PointerType")
	{
		m_pointers[_a.value("id")] = new PointerType(_a);
	}
	else if (_n == "ArrayType")
	{
		m_arrays[_a.value("id")] = new ArrayType(_a);
	}
	else if (_n == "CvQualifiedType")
	{
		m_cvQualifieds[_a.value("id")] = new CvQualifiedType(_a);
	}
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
					M_ASSERT(iwf);
					Variable* m = new Variable;
					con->back().place(m);
					m_resolvers << new VariableResolver(m, _a);
					goto OK;
				}
			}
		}
		qCritical("Couldn't find with-fields resolver context %s for %s.", _a.value("context").toLatin1().data(), _a.value("id").toLatin1().data());
		OK:;
	}
	else if (_n == "EnumValue")
	{
		M_ASSERT(m_lastEnum);
		// TODO: refactor for consistency
		EnumValue* v = new EnumValue;
		m_lastEnum->back().place(v);
		m_resolvers << new EnumValueResolver(v, _a);
	}
	else if (_n == "Function" && (!_a.value("mangled").isEmpty() || !_a.value("name").startsWith("_")))
	{
		con->back().place(m_functions[_a.value("id")] = new Function);
		m_lastFunction = new FunctionResolver(m_functions[_a.value("id")], _a);
		m_resolvers << m_lastFunction;
		m_contexts[_a.value("id")] = m_functions[_a.value("id")];
	}
	else if (_n == "FunctionType")
	{
		m_functionTypes[_a.value("id")] = new IncomingFunctionType(_a);
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
		Typedef* t = new Typedef;
		con->back().place(t);
		m_types[_a.value("id")] = t;
		m_resolvers << new TypedefResolver(t, _a);
		m_contexts[_a.value("id")] = t;
	}
	else if (_n == "Enumeration")
	{
		con->back().place(m_lastEnum = new Enumeration);
		m_types[_a.value("id")] = m_lastEnum;
		m_resolvers << new EnumerationResolver(m_lastEnum, _a);
	}
	else if (_n == "Struct")
	{
		Struct* t = new Struct;
		con->back().place(t);
		m_types[_a.value("id")] = t;
		m_resolvers << new StructResolver(t, _a);
		m_contexts[_a.value("id")] = t;
	}
	else if (_n == "Union")
	{
		Union* t = new Union;
		con->back().place(t);
		m_types[_a.value("id")] = t;
		m_resolvers << new UnionResolver(t, _a);
		m_contexts[_a.value("id")] = t;
	}
	else if (_n == "Variable" && (!_a.value("mangled").isEmpty() || !_a.value("name").startsWith("_")))
	{
		con->back().place(m_variables[_a.value("id")] = new Variable);
		m_resolvers << new VariableResolver(m_variables[_a.value("id")], _a);
		m_contexts[_a.value("id")] = m_variables[_a.value("id")];
	}
	else if (_n == "File")
	{
		m_files[_a.value("id")] = new DeclarationFile(_a.value("name"));
	}
	else if (_n == "Namespace")
	{
		if (_a.value("name") == "::")
		{
			m_contexts[_a.value("id")] = m_d;
		}
		else
		{
			M_ASSERT(_a.value("members").isEmpty());
		}
	}
	return true;
}

bool DeclarationsHandler::endElement(QString const&, QString const& _n, QString const&)
{
	if (_n == "Function")
		m_lastFunction = 0;
	else if (_n == "IncomingFunctionType")
		m_lastIncomingFunctionType = 0;
	else if (_n == "Enumeration")
		m_lastEnum = 0;
	return true;
}

bool DeclarationsHandler::endDocument()
{
	qInformation() << s_news << "total allocations" << s_deletes << "total deallocations.";
	// Resolve all types.
	TIME_STATEMENT(Resolving types)
	foreach(Resolver* f, m_resolvers)
	{
		f->resolve(this);
		delete f;
	}

	TIME_STATEMENT(Clean temps)
{
	// Clean up all temporaries that the resolution process depended on.
	foreach(ArrayType* i, m_arrays.values()) delete i;
	m_arrays.clear();
	foreach(CvQualifiedType* i, m_cvQualifieds.values()) delete i;
	m_cvQualifieds.clear();
	foreach(PointerType* i, m_pointers.values()) delete i;
	m_pointers.clear();
	foreach(IncomingFunctionType* i, m_functionTypes.values()) delete i;
	m_functionTypes.clear();

	(*m_l) = m_files.values();
}
	return true;
}

}
