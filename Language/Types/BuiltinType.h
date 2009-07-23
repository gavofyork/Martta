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

#pragma once

#include "Type.h"
#include "CTypes.h"
#include "PhysicalType.h"

namespace Martta
{

extern const int s_simpleIds[];
extern const int s_simpleIdsCount;

class BuiltinOperator;

template<class T> class NameTrait;

class BuiltinType: public PhysicalType
{
	MARTTA_INITIALISED_OBJECT(PhysicalType)

	friend class BuiltinOperator;	// For use of s_nonMembers. QUICK Should probably use an interface for this.
	friend class NameTrait<int>;
	
public:
	static void initialiseClass();
	static void finaliseClass();
	
	static inline void registerExtra(String const& _name, Kind const& _k) { s_recognisedExtras.insert(_name, _k); }
	static inline void unregisterExtra(String const& _name) { s_recognisedExtras.remove(_name); }

	BuiltinType(uint _d = (uint)-1): m_id(_d) {}

	uint id() const { return m_id; }
	void setId(uint _d) { m_id = _d; changed(); }
	void committed(int);

	virtual String code(String const& _middle) const;

	static inline String name(uint _d)
	{
		return (_d == (uint)-1) ? String() : (_d == Wchar) ? String("wchar_t") : (String((_d & Unsigned) ? (_d & Float || _d & Double) ? "c" : "u" : "") +
				(((_d & Longlong) == Longlong) ? "ll" : (_d & Short) ? "s" : (_d & Long) ? "l" : "") +
				((_d & Float) ? "float" : (_d & Bool) ? "bool" : (_d & Double) ? "double" : (_d & Char) ? "char" : (_d & Int) ? "int" : "void"));
	}

	static int id(String const& _n)
	{
		for (int i = 0; i < s_simpleIdsCount; i++)
			if (name(s_simpleIds[i]) == _n)
				return s_simpleIds[i];
		return (uint)-1;
	}

	List<int>							possibilities();
	virtual String						defineEditLayout(ViewKeys const&, int) const;
	inline int							get() const { return m_id; }
	inline void							set(uint _m) { setId(_m); }
	virtual bool						isNull() const { return m_id == (uint)-1; }
	
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual bool						hasDefaultConstructor() const { return true; }
	virtual Types						assignableTypes() const;
	virtual bool						contentsEquivalentTo(TypeEntity const* _t) const { return _t->asKind<BuiltinType>()->m_id == m_id; }
	virtual Rgb							idColour() const { return 0xffbb77; }
	virtual TypeEntity*					newClone() const { return new BuiltinType(m_id); }
	virtual String						defineLayout(ViewKeys const&) const;
	virtual EditDelegateFace*			newDelegate(CodeScene* _s);
	virtual bool						isSuperfluous() const { return Super::isSuperfluous() && m_id == (uint)-1; }
	virtual bool						keyPressed(KeyEvent const* _e);
	virtual bool						defineSimilarityFrom(TypeEntity const* _f, Castability _c) const;
	virtual bool						defineSimilarityTo(TypeEntity const* _t, Castability _c) const;
	virtual void						properties(Hash<String, String>& _p) const { Super::properties(_p); _p[L"id"] = String::number(m_id); }
	virtual void						setProperties(Hash<String, String> const& _p) { Super::setProperties(_p); m_id = _p[L"id"].toInt(); }

private:
	uint m_id;

	static Hash<String, Kind>			s_recognisedExtras;
	static List<BuiltinOperator*>		s_nonMembers;
};

template<>
struct TypeConstructor<uint const&>
{
	static inline void construct(Type* _this, uint const& _builtin) { _this->m_top = new BuiltinType(_builtin); _this->m_top->setOwner(_this); }
};

}