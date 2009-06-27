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

#include <QDebug>

#include "BasicRoot.h"
#include "Entity.h"
#include "Depender.h"
#include "Dependee.h"
#include "TestHelper.h"
using namespace Martta;

int supportTests();
int coreTests();
int typeTests();

QString activity;

class NewEntity: public Entity, public_interface Depender, public_interface Dependee
{
	MARTTA_OBJECT(Entity)
	MARTTA_INHERITS(Depender, 0)
	MARTTA_INHERITS(Dependee, 1)

public:
	NewEntity(QString _id = QString::null, int _fd = Depender::DependsOnNothing, Kinds const& _ad = Kinds()): m_id(_id), m_familyDependencies(_fd), m_ancestralDependencies(_ad) {}

	virtual bool						botherNotifying() const { return true; }
	virtual int							familyDependencies() const { return m_familyDependencies; }
	virtual Kinds						ancestralDependencies() const { return m_ancestralDependencies; }

	virtual void						onIndexChanged(int _oldIndex) { activity += (m_id + "X%1 ").arg(_oldIndex); }
	virtual void						onDependencyChanged(int _aspect, Entity* _e) { activity += (m_id + "C%1%2 ").arg(_aspect).arg(_e->asKind<NewEntity>()->m_id); }
	virtual void						onDependencySwitched(Entity* _e, Entity* _old) { activity += (m_id + "S%1%2 ").arg(_e->asKind<NewEntity>()->m_id).arg(_old->asKind<NewEntity>()->m_id); }
	virtual void						onChildMoved(Entity* _e, int _oldIndex) { activity += (m_id + "M%1%2 ").arg(_e->asKind<NewEntity>()->m_id).arg(_oldIndex); }
	virtual void						onDependencyAdded(Entity* _e) { activity += (m_id + "+%1 ").arg(_e->asKind<NewEntity>()->m_id); }
	virtual void						onDependencyRemoved(Entity* _e, int _index) { activity += (m_id + "-%1%2 ").arg(_index == UndefinedIndex ? QString::null : QString::number(_index)).arg(_e->asKind<NewEntity>()->m_id); }
	virtual void						onChildrenInitialised() { activity += (m_id + "I "); }
	
	bool								changed(int _aspect = AllAspects) { return Dependee::changed(_aspect); }
	
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
	
private:
	QString m_id;
	int m_familyDependencies;
	Kinds m_ancestralDependencies;
};
MARTTA_OBJECT_CPP(NewEntity);

class NewEntity2: public NewEntity
{
	MARTTA_OBJECT(NewEntity)
	
public:
	NewEntity2(QString _id = QString::null, int _fd = Depender::DependsOnNothing, Kinds const& _ad = Kinds()): NewEntity(_id, _fd, _ad) {}
};
MARTTA_OBJECT_CPP(NewEntity2);

int test()
{
	s_testing = true;
	int failed = 0;
	TEST("Asserting")
	{
		bool testHasActuallySucceeded = true;
		M_ASSERT(!testHasActuallySucceeded);
		FAILED_IF(!s_asserted);
		s_asserted = 0;
	}
	
//	failed += supportTests();
	TEST("Auxilliaries Initialisation")
		AuxilliaryRegistrar::get()->jigCache();
//	failed += coreTests();
//	failed += typeTests();
	
	enum { DependsOnNothing = 0, DependsOnParent = 1, DependsOnChildren = 2, DependsOnBoth = 3, DependsOnIndex = 4, TestOnOrder = 8, DependsOnChildOrder = DependsOnChildren | TestOnOrder };
	TEST("Change system: Family dependencies changing, inert")
	{
		BasicRoot r;
		
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
		BasicRoot r;
		NewEntity* n1 = new NewEntity("n1", DependsOnChildren);
		r.back().place(n1);
		NewEntity* n2 = new NewEntity("n2", DependsOnParent);
		n1->back().place(n2);
		FAILED_IF(activity != "n1+n2 n2+n1 " && activity != "n2+n1 n1+n2 ");
		
		activity.clear();
		n1->changed(4);
		qDebug() << activity;
		FAILED_IF(activity != "n2C4n1 ");
		
		activity.clear();
		n2->changed(1);
		qDebug() << activity;
		FAILED_IF(activity != "n1C1n2 ");
		
		activity.clear();
		n2->killAndDeleteWithNotification();
		n1->killAndDeleteWithNotification();
		FAILED_IF(activity != "n1-0n2 n2-n1 " && activity != "n2-n1 n1-0n2 ");
	}
	
	TEST("Change system: Family dependencies moving")
	{
	}

	TEST("Change system: Aspects & recursive changing")
	{
		// n1[NE2] ---> n2[NE2] ---> n3[NE] ---> n4[NE]
		//               ^ - - - - - - - - - - - /
		activity.clear();
		BasicRoot r;
		NewEntity* n1 = new NewEntity2("n1", DependsOnNothing);
		r.back().place(n1);
		NewEntity* n2 = new NewEntity2("n2", DependsOnNothing);
		n1->back().place(n2);
		NewEntity* n3 = new NewEntity("n3", DependsOnNothing);
		n2->back().place(n3);
		NewEntity* n4 = new NewEntity("n4", DependsOnNothing, Kinds() << Kind::of<NewEntity2>());
		n3->back().place(n4);
		r.debugTree();
		qDebug() << activity;
		FAILED_IF(activity != "n4+n2 ");
		
		activity.clear();
		n2->changed(16);
		qDebug() << activity;
		FAILED_IF(activity != "n4C16n2 ");
		
		activity.clear();
		n2->move(r.back());
		r.debugTree();
		qDebug() << activity;
		FAILED_IF(activity != "");
		
		activity.clear();
		n3->move(n1->back());
		r.debugTree();
		qDebug() << activity;
		FAILED_IF(activity != "n4Sn1n2 ");

		activity.clear();
		n3->move(n2->back());
		r.debugTree();
		qDebug() << activity;
		FAILED_IF(activity != "n4Sn2n1 ");

		n4->killAndDeleteWithNotification();
		n3->killAndDeleteWithNotification();
		n2->killAndDeleteWithNotification();
		n1->killAndDeleteWithNotification();
	}

	TEST("Change system: Ancestral dependencies changing")
	{
	}

	TEST("Change system: Ancestral dependencies moving")
	{
	}

	TEST("Change system: Freefrom dependencies")
	{
	}

	s_testing = false;
	qInformation() << "PASSED :-)";
	
	exit(failed);
	return failed;
}































