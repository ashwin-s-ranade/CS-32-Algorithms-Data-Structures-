#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include "Actor.h"
#include "Level.h"
#include <cmath> 
#include <sstream> //for stringstreams 
#include <iomanip>
#include <iostream> //redefines << operator
#include <vector>
#include <iterator>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp


class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	~StudentWorld(); //destructor, derived from virtual superclass destructor 
	
	//helper methods 
	void addActor(Actor* a) {
		shiver.push_back(a); 
	}
	void addFlame(double posx, double posy, int direction) {
		Actor* f = new Flame(this, posx/SPRITE_WIDTH, posy/SPRITE_HEIGHT, direction); 
		shiver.push_back(f); 
	}
	void addLandmine(double posx, double posy) {
		Actor* a = new Landmine(this, posx / SPRITE_WIDTH, posy / SPRITE_HEIGHT); 
		shiver.push_back(a); 
	}
	void addPit(double posx, double posy) {
		Actor* a = new Pit(this, posx / SPRITE_WIDTH, posy / SPRITE_HEIGHT); 
		shiver.push_back(a); 
	}
	bool nointersect(double x, double y, Actor* a); 
	bool isFlameBlockedAt(double x, double y); 
	void activateOnAppropriateActors(Actor* a);
	void levelFinished(); 
	double distPenelope(double px, double py) {
		double deltaX = px - p->getX(); 
		double deltaY = py - p->getY(); 
		return sqrt(pow(deltaX, 2) + pow(deltaY, 2)); 
	}
	double distanceNearestZombie(double px, double py, Actor* a);
	double distanceNearestHuman(double px, double py, double& nearX, double& nearY); 
	bool findActorToVomitOn(double x, double y); 

	void changeGoodies(char c, int amt) {
		switch (c) {
		case 'v':
			p->changeVax(amt); 
			break;
		case 'g': 
			p->changeGas(amt); 
			break;
		case 'l':
			p->changeLandmines(amt); 
			break;
		}
	}
	Actor* getPenelope() {
		return p; 
	}

	void citizenDec() {
		citizensAlive--; 
	}

	bool citizensAllGone();


private:
	//vector containing all Actors, but not Penelope
	Actor* p; 
	vector< Actor*> shiver; 
	int status; 
	int citizensAlive; 
};

#endif // STUDENTWORLD_H_
