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

// Just to prevent the annoying visibility warnings that Qt throws if included after MS.
#include <QString>

#include "TypeEntity.h"

#include "Root.h"
#include "Entity.h"
#include "Depender.h"
#include "Dependee.h"
#include "TestHelper.h"
using namespace Martta;

String activity;

class NewEntity: public Entity, public_interface Depender, public_interface Dependee
{
	MARTTA_OBJECT(Entity)
	MARTTA_INHERITS(Depender, 0)
	MARTTA_INHERITS(Dependee, 1)

public:
	NewEntity(String const& _id = String::null, int _fd = Depender::DependsOnNothing, Kinds const& _ad = Kinds()): m_id(_id), m_familyDependencies(_fd), m_ancestralDependencies(_ad) {}

	virtual bool						botherNotifying() const { return true; }
	virtual int							familyDependencies() const { return m_familyDependencies; }
	virtual Kinds						ancestralDependencies() const { return m_ancestralDependencies; }

	virtual void						onIndexChanged(int _oldIndex) { activity += (m_id + "X%1 ").arg(_oldIndex); }
	virtual void						onChildMoved(Entity* _e, int _oldIndex) { activity += (m_id + "M%1%2 ").arg(_oldIndex).arg(_e->asKind<NewEntity>()->m_id); }
	virtual void						onChildrenInitialised() { activity += (m_id + "I "); }
	virtual void						onDependencyChanged(int _aspect, Entity* _e) { activity += (m_id + "C%1%2 ").arg(_aspect).arg(_e->asKind<NewEntity>()->m_id); }
	virtual void						onDependencySwitched(Entity* _e, Entity* _old) { activity += (m_id + "S%1%2 ").arg(_e->asKind<NewEntity>()->m_id).arg(_old->asKind<NewEntity>()->m_id); }
	virtual void						onDependencyAdded(Entity* _e) { activity += (m_id + "+%1 ").arg(_e->asKind<NewEntity>()->m_id); }
	virtual void						onDependencyRemoved(Entity* _e, int _index) { activity += (m_id + "-%1%2 ").arg(_index == UndefinedIndex ? String::null : String::number(_index)).arg(_e->asKind<NewEntity>()->m_id); }
	
	bool								changed(int _aspect = AllAspects) { return Dependee::changed(_aspect); }
	
	inline void							addDependency(Dependee* _e) { Depender::addDependency(_e); }
	inline void							removeDependency(Dependee* _e) { Depender::removeDependency(_e);}
	inline void							removeAllDependencies() { Depender::removeAllDependencies();}
	inline bool							haveDependency(Dependee* _e) const { return Depender::haveDependency(_e);}
	
protected:
	virtual Kinds						allowedKinds(int) const { return Kind::of<Entity>(); }

	virtual void						childrenInitialised() { Depender::childrenInitialised(); }
	virtual void						childAdded(int _newChildsIndex) { Depender::childAdded(_newChildsIndex); }
	virtual void						childSwitched(Entity* _currentChild, Entity* _exChild) { Depender::childSwitched(_currentChild, _exChild); }
	virtual void						childRemoved(Entity* _exChild, int _exChildsIndex) { Depender::childRemoved(_exChild, _exChildsIndex); }
	virtual void						childMoved(Entity* _child, int _originalIndex) { Depender::childMoved(_child, _originalIndex); }
	virtual void						parentAdded() { Depender::parentAdded(); }
	virtual void						parentSwitched(Entity* _exParent) { Depender::parentSwitched(_exParent); }
	virtual void						parentRemoved(Entity* _exParent) { Depender::parentRemoved(_exParent); }
	
	String m_id;
private:
	int m_familyDependencies;
	Kinds m_ancestralDependencies;
};
MARTTA_OBJECT_CPP(NewEntity);

class NewEntity2: public NewEntity
{
	MARTTA_OBJECT(NewEntity)
	
public:
	NewEntity2(String const& _id = String::null, int _fd = Depender::DependsOnNothing, Kinds const& _ad = Kinds()): NewEntity(_id, _fd, _ad) {}
};
MARTTA_OBJECT_CPP(NewEntity2);

class NewEntitySilly: public NewEntity
{
	MARTTA_OBJECT(NewEntity)
	
public:
	NewEntitySilly(String const& _id = String::null, int _fd = Depender::DependsOnNothing, Kinds const& _ad = Kinds()): NewEntity(_id, _fd, _ad) {}
	
	virtual void						onDependencyChanged(int _aspect, Entity* _e) { Super::onDependencyChanged(_aspect, _e); if (changed(1)) activity += m_id + "*1 "; }
};
MARTTA_OBJECT_CPP(NewEntitySilly);

int changeTests()
{
	int failed = 0;
	
	enum { DependsOnNothing = 0, DependsOnParent = 1, DependsOnChildren = 2, DependsOnBoth = 3, DependsOnIndex = 4, TestOnOrder = 8, DependsOnChildOrder = DependsOnChildren | TestOnOrder };
	TEST("Change system: Family dependencies changing, inert")
	{
		Root r;
		
		activity.clear();
		NewEntity* n1 = new NewEntity("n1");
		r.back().place(n1);
		NewEntity* n2 = new NewEntity("n2");
		n1->back().place(n2);
		n1->changed(1);
		n2->changed(1);
		n2->killAndDelete();
		n1->killAndDelete();
		FAILED_IF(activity != "");
	}
	
	TEST("Change system: Family dependencies changing, active")
	{
		activity.clear();
		Root r;
		NewEntity* n1 = new NewEntity("n1", DependsOnChildren);
		r.back().place(n1);
		NewEntity* n2 = new NewEntity("n2", DependsOnParent);
		n1->back().place(n2);
		FAILED_IF(activity != "n1+n2 n2+n1 " && activity != "n2+n1 n1+n2 ");
		
		activity.clear();
		n1->changed(4);
		FAILED_IF(activity != "n2C4n1 ");
		
		activity.clear();
		n2->changed(1);
		FAILED_IF(activity != "n1C1n2 ");
		
		activity.clear();
		n2->killAndDeleteWithNotification();
		n1->killAndDeleteWithNotification();
		FAILED_IF(activity != "n1-0n2 n2-n1 " && activity != "n2-n1 n1-0n2 ");
	}
	
	TEST("Change system: Family dependencies moving")
	{
		Root r;
		NewEntity* n1 = new NewEntity("n1", DependsOnChildOrder);
		r.back().place(n1);
		NewEntity* n2 = new NewEntity("n2", DependsOnParent);
		activity.clear();
		n1->back().place(n2);
		FAILED_IF(activity != "n2+n1 n1+n2 " && activity != "n1+n2 n2+n1 ");
		
		activity.clear();
		NewEntity* n3 = new NewEntity("n3", DependsOnIndex);
		n1->back().place(n3);
		FAILED_IF(activity != "n1+n3 ");
		
		activity.clear();
		n2->move(n1->back());
		FAILED_IF(!activity.contains("n1M1n3 ") || !activity.contains("n3X1 ") || !activity.contains("n1M0n2 ") || activity.size() != String("n1M1n3 n1M0n2 n3X1 ").size());
	}

	TEST("Change system: Aspects & recursive changing")
	{
		Root r;
		NewEntity* n1 = new NewEntity("n1");
		r.back().place(n1);
		NewEntity* n2 = new NewEntitySilly("n2", DependsOnChildren);
		n1->back().place(n2);
		NewEntity* n3 = new NewEntitySilly("n3", DependsOnParent);
		n2->back().place(n3);
		activity.clear();
		n2->changed(2);
		FAILED_IF(activity != "n3C2n2 n2C1n3 n3C1n2 n2*1 n3*1 ");
		activity.clear();
		n2->changed(3);
		FAILED_IF(activity != "n3C3n2 n2C1n3 n3*1 ");
	}
	
	
	TEST("Change system: Ancestral dependencies moving")
	{
		// n1[NE2] ---> n2[NE2] ---> n3[NE] ---> n4[NE]
		//               ^ - - - - - - - - - - - /
		activity.clear();
		Root r;
		NewEntity* n1 = new NewEntity2("n1", DependsOnNothing);
		r.back().place(n1);
		NewEntity* n2 = new NewEntity2("n2", DependsOnNothing);
		n1->back().place(n2);
		NewEntity* n3 = new NewEntity("n3", DependsOnNothing);
		n2->back().place(n3);
		NewEntity* n4 = new NewEntity("n4", DependsOnNothing, Kind::of<NewEntity2>());
		n3->back().place(n4);
		FAILED_IF(activity != "n4+n2 ");
		
		activity.clear();
		n2->changed(16);
		FAILED_IF(activity != "n4C16n2 ");
		
		activity.clear();
		n2->move(r.back());
		FAILED_IF(activity != "");
		
		activity.clear();
		n3->move(n1->back());
		FAILED_IF(activity != "n4Sn1n2 ");

		activity.clear();
		n3->move(n2->back());
		FAILED_IF(activity != "n4Sn2n1 ");
		n1->killAndDeleteWithNotification();

		activity.clear();
		n2->changed(1);
		FAILED_IF(activity != "n4C1n2 ");

		activity.clear();
		n3->move(n2->over());
		FAILED_IF(activity != "n4-n2 ");
		n2->killAndDeleteWithNotification();

		n4->killAndDeleteWithNotification();
		n3->killAndDeleteWithNotification();
	}

	TEST("Change system: Freeform dependencies")
	{
		activity.clear();
		Root r;
		NewEntity* n1 = new NewEntity("n1", DependsOnNothing);
		r.back().place(n1);
		NewEntity* n2 = new NewEntity("n2", DependsOnNothing);
		n1->back().place(n2);
		NewEntity* n3 = new NewEntity("n3", DependsOnNothing);
		n2->back().place(n3);
		NewEntity* n4 = new NewEntity("n4", DependsOnNothing);
		n3->back().place(n4);
		FAILED_IF(activity != "");
		
		n1->addDependency(n4);
		n1->addDependency(n3);
		FAILED_IF(activity != "");
		FAILED_IF(!n1->haveDependency(n4));
		FAILED_IF(!n1->haveDependency(n3));
		
		activity.clear();
		n4->changed(8);
		FAILED_IF(activity != "n1C8n4 ");
		
		activity.clear();
		n3->changed(7);
		FAILED_IF(activity != "n1C7n3 ");
		
		activity.clear();
		n4->move(Nowhere);
		n4->changed(8);
		FAILED_IF(activity != "n1C8n4 ");
		
		activity.clear();
		n3->changed(7);
		FAILED_IF(activity != "n1C7n3 ");
		
		activity.clear();
		n4->killAndDeleteWithNotification();
		FAILED_IF(activity != "n1-n4 ");
		FAILED_IF(n1->haveDependency(n4));
	}

	return failed;
}































