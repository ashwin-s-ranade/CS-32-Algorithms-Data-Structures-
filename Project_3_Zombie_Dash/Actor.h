#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>

using namespace std;

class StudentWorld; 
// class Goodie; is this needed? 

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
	Actor(const int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* world);
	
	virtual void doSomething() {} // virtual function
	virtual void activateIfAppropriate(Actor* a) {}
	virtual bool canExit() {return false;} //redefined in the classes where it returns true
	virtual bool canBlockMovement() { return false;  }
	virtual bool canPickUp(){ return false; }
	void setState(bool state) {
		if (state) //if state is true, this means keep it alive
			m_alive = true;
		else
			m_alive = false; 
	}
	bool getState() const { return m_alive; }
	virtual void changeGas(int amt) {} // virtual functions
	virtual void changeVax(int amt) {}
	virtual void changeLandmines(int amt) {}
	virtual int getGoodies(char c) { return 0; }
	virtual void decrementFlame() {}
	virtual int getFlameState() { return -1000; }
	virtual void decrementVomitState() {}
	virtual int getVomitState() { return -1000;  }
	virtual int getLandmineState() { return -2000; }
	virtual void decrementLandmineState() {}
	virtual bool canGetFlamed() { return false; }
	virtual bool isWallOrExit() { return false;  }//for determining flame collisions
	virtual bool isAgent() { return false;}
	virtual void getRekt() {}
	virtual bool getInfectionStatus() { return false; }
	virtual void setInfectionStatus(bool infectionStatus) {}
	virtual int getInfectionCount() { return -1000; }
	virtual void incrementInfectionCount(int amt) {}
	virtual bool isZombie() { return false;  }
	virtual bool canbeSaved() { return false;  }
	virtual int getMovementPlan() { return 0; }
	virtual void modifyMovementPlan(){}
	virtual bool isFlame() { return false;  }

	StudentWorld* getWorld() const{
		return m_world; 
	}
private:
	StudentWorld* m_world; 
	bool m_alive; 
};
class Agent : public Actor
{
public:
	Agent(StudentWorld* w, int imageID, double x, double y, int dir);
	virtual bool canGetFlamed() { return true; } 
	virtual bool isAgent() { return true; }

};

class Human : public Agent
{
public:
	Human(StudentWorld* w, int imageID, double x, double y);
	virtual bool canExit() { //overrides base class function definition 
		return true;
	}
	virtual bool canBlockMovement() { return true; }
	virtual bool getInfectionStatus() { return m_infectionStatus;}
	virtual void setInfectionStatus(bool infectionStatus) {
		m_infectionStatus = infectionStatus; 
	}
	virtual int getInfectionCount() { return m_infectionCount; }
	virtual void incrementInfectionCount(int amt) {
		m_infectionCount += amt;
	}
private:
	int m_infectionCount;
	bool m_infectionStatus; 
};

class Penelope : public Human {
public:
	Penelope(double startX, double startY, StudentWorld* world);
	virtual void doSomething();
		//if all the citizens have survived, then save Penelope 
	virtual void changeGas(int amt) { //amt can be negative! 
		m_gascans += amt; 
	}
	virtual void changeVax(int amt) {
		m_vaccines += amt; 
	}
	virtual void changeLandmines(int amt) {
		m_landmines += amt; 
	}
	virtual int getGoodies(char c) {
		switch (c) {
		case 'l':
			return m_landmines; 
		case 'g':
			return m_gascans; 
		case 'v':
			return m_vaccines; 
		}
		return -1000; //something went wrong 
	}
	virtual bool canPickUp() {
		return true; 
	}
	virtual void getRekt() {}
private:
	int m_vaccines; 
	int m_gascans; 
	int m_landmines; 
};

class Citizen : public Human
{
public:
	Citizen(StudentWorld* w, double x, double y);
	virtual void doSomething(); 
	virtual void getRekt(); 
	virtual bool canbeSaved() { return true; }
private:
	int paralyzed; 
};

class Zombie : public Agent
{
public:
	Zombie(StudentWorld* w, double x, double y);
	virtual bool canBlockMovement() { return true; }
	virtual bool isZombie() { return true;  }
	virtual int getMovementPlan() { return m_movementPlanDistance; }
	virtual void modifyMovementPlan(int amt) { m_movementPlanDistance += amt; }
private:
	int m_movementPlanDistance; 
};

class DumbZombie : public Zombie
{
public:
	DumbZombie(StudentWorld* w, double x, double y);
	virtual void doSomething(); 
	virtual void getRekt();
private:
	int paralyzed; 
};

class SmartZombie : public Zombie
{
public:
	SmartZombie(StudentWorld* w, double x, double y);
	virtual void doSomething(); 
	virtual void getRekt();
private:
	int paralyzed; 
};


class Wall : public Actor {
public: 
	Wall(StudentWorld* world, double startX, double startY); 
	virtual bool canBlockMovement() { return true; }
	virtual bool isWallOrExit() { return true; }
};


class ActivatingObject : public Actor
{
public:
	ActivatingObject(StudentWorld* w, int imageID, double x, double y, int depth, int dir);
};

class Exit : public ActivatingObject
{
public:
	Exit(StudentWorld* w, double x, double y);
	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);
	virtual bool isWallOrExit() { return true; }
};

class Pit : public ActivatingObject
{
public:
	Pit(StudentWorld* w, double x, double y);
	virtual void activateIfAppropriate(Actor* a);
	virtual void doSomething(); 
};

class Flame : public ActivatingObject
{
public:
	Flame(StudentWorld* w, double x, double y, int dir);
	virtual void activateIfAppropriate(Actor* a);
	virtual void doSomething(); 
	virtual void decrementFlame() {
		m_flameState--; 
	}
	virtual bool isFlame() {
		return true; 
	}
	virtual int getFlameState() { return m_flameState; }
private:
	int m_flameState; 
};

class Vomit : public ActivatingObject
{
public:
	Vomit(StudentWorld* w, double x, double y, int dir);
	virtual void activateIfAppropriate(Actor* a);
	virtual void doSomething(); 
	virtual int getVomitState() { return m_vomitState; }
	virtual void decrementVomitState() { m_vomitState--;  }

private: 
	int m_vomitState; 
};

class Landmine : public ActivatingObject
{
public:
	Landmine(StudentWorld* w, double x, double y);
	virtual void activateIfAppropriate(Actor* a);
	virtual bool canGetFlamed() { return true; }
	virtual void doSomething(); 
	virtual int getLandmineState() { return m_landmineState; }
	virtual void decrementLandmineState() { m_landmineState--; }
private:
	int m_landmineState; 
};

class Goodie : public ActivatingObject
{
public:
	Goodie(StudentWorld* w, int imageID, double x, double y);
	bool activateHelper(Actor* a); 
	virtual bool canGetFlamed() { return true; }
};

class VaccineGoodie : public Goodie
{
public:
	VaccineGoodie(StudentWorld* w, double x, double y);
	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);
};

class GasCanGoodie : public Goodie
{
public:
	GasCanGoodie(StudentWorld* w, double x, double y);
	virtual void doSomething(); 
	virtual void activateIfAppropriate(Actor* a);
};

class LandmineGoodie : public Goodie
{
public:
	LandmineGoodie(StudentWorld* w, double x, double y);
	virtual void doSomething(); 
	virtual void activateIfAppropriate(Actor* a);
};

#endif // ACTOR_H_
