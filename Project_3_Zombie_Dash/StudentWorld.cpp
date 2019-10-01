#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>

using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath){}

int StudentWorld::init()
{
	//now load the level 
	Level lev(assetPath()); 
	ostringstream temp;

	temp << "level0" << getLevel()<< ".txt"; 
	string currentLevel = temp.str(); 
	Level::LoadResult result = lev.loadLevel(currentLevel); 

	//now check the validity of the level
	if (result == Level::load_fail_file_not_found)
		return GWSTATUS_PLAYER_WON; 
	if (result == Level::load_fail_bad_format) {
		return GWSTATUS_LEVEL_ERROR; //bad input
	}
	//loop through the resultant maze
	citizensAlive = 0; 
	for (int i = 0; i < LEVEL_WIDTH; i++) {
		for (int j = 0; j < LEVEL_HEIGHT; j++) {
			Actor* a; // should i set this equal to null? 
			switch (lev.getContentsOf(i, j)) {
			case lev.wall:
				a = new Wall(this, i, j); 
				shiver.push_back(a); 
				break; 
			case lev.player:
				p = new Penelope(i, j, this); //penelope parameters are ordered differently, we also don't push into the vector 
				break; 
			case lev.dumb_zombie:
				a = new DumbZombie(this, i, j); 
				shiver.push_back(a);
				break;
			case lev.smart_zombie:
				a = new SmartZombie(this, i, j); 
				shiver.push_back(a);
				break; 
			case lev.citizen:
				a = new Citizen(this, i, j); 
				shiver.push_back(a);
				citizensAlive++; 
				break; 
			case lev.exit:
				a = new Exit(this, i, j);
				shiver.push_back(a);
				break;
			case lev.pit:
				a = new Pit(this, i, j);
				shiver.push_back(a);
				break;
			case lev.gas_can_goodie:
				a = new GasCanGoodie(this, i, j);
				shiver.push_back(a);
				break;
			case lev.vaccine_goodie:
				a = new VaccineGoodie(this, i, j);
				shiver.push_back(a);
				break;
			case lev.landmine_goodie:
				a = new LandmineGoodie(this, i, j); 
				shiver.push_back(a);
				break;
			}
		}
	}
	
    status = GWSTATUS_CONTINUE_GAME;
	return status; 
}

int StudentWorld::move() //each time we go through this, one tick occurs 
{

    //iterate through the vector, and Penelope individually 
	p->doSomething(); //checks if the object is dead
	if (status == GWSTATUS_PLAYER_DIED) {
		return GWSTATUS_PLAYER_DIED;
	}
	vector<Actor*>::iterator it = shiver.begin();
	while (it != shiver.end()) { 
		//loop through each actor and give them a chance to do something 
		//check if actor is dead, and if penelope is dead
		(*it)->doSomething(); 
		it++; 
	}

	if (!p->getState()) {//if Penelope is dead
		playSound(SOUND_PLAYER_DIE); 
		decLives(); 
		return GWSTATUS_PLAYER_DIED; 
	}

	//remove newly-dead actors after each tick 
	it = shiver.begin(); //reuse iterator 
	while (it != shiver.end()) {
		if ((*it)->getState() == false) {
			if ((*it)->isAgent()) {
				(*it)->getRekt(); 
			}
			delete (*it); 
			it = shiver.erase(it); 
		}
		else
			it++; 
	}

	//update display text
	ostringstream top; 
	//Score: 004500
	top.fill('0'); 
	if (getScore() < 0) {
		top << "Score: -" << setw(5) << -1*getScore();
	}
	else
		top << "Score: " << setw(6) << getScore();
	top.fill(' '); 
	top << "  Level:" << setw(2) << getLevel() << "  Lives: " << getLives() << "  Vaccines:";
	top << setw(2) << p->getGoodies('v') << "  Flames: " << setw(2) << p->getGoodies('g') << "  Mines:"; 
	top << setw(2) << p->getGoodies('l') << "  Infected: " << setw(3) << p->getInfectionCount(); 
	string text = top.str(); 
	setGameStatText(text); 

	return status;
}

void StudentWorld::activateOnAppropriateActors(Actor* a) {
	//loop through shiver and look at intersects, then call activateOnAppropriate
	double penX = p->getX() - a->getX(); 
	double penY = p->getY() - a->getY(); 
	if (penX*penX + penY * penY <= 100) { 
		a->activateIfAppropriate(p);
	}
	
	//then loop through everything else 
	vector<Actor*>::iterator bz = shiver.begin(); 
	while (bz != shiver.end()) {
		double deltaX = (*bz)->getX() - a->getX(); 
		double deltaY = (*bz)->getY() - a->getY(); 
		if (deltaX*deltaX + deltaY * deltaY <= 100) { //if the objects overlap 
			a->activateIfAppropriate(*bz); //is this the right way? 
		}
		bz++; 
	}
}

void StudentWorld::levelFinished() {
	status = GWSTATUS_FINISHED_LEVEL;
}

void StudentWorld::cleanUp()
{
	if (p != nullptr) {
		delete p;
		p = nullptr;
	}
	vector<Actor*>::iterator nba = shiver.begin(); 
	while (nba != shiver.end()) {
		delete (*nba); 
		nba = shiver.erase(nba); 
	}

}

StudentWorld::~StudentWorld() {
	cleanUp(); //call destructor
	cleanUp();
}

bool StudentWorld::nointersect(double playerX, double playerY, Actor* a)
{
	vector<Actor*>::iterator x = shiver.begin();
	while (x != shiver.end())
	{
		if ((*x)->canBlockMovement() && (*x) != a)
		{
			if ((playerX >= (*x)->getX() && playerX <= (*x)->getX() + SPRITE_WIDTH - 1) && (playerY >= (*x)->getY() && playerY <= (*x)->getY() + SPRITE_HEIGHT - 1))
				return !true;

			if ((playerX + SPRITE_WIDTH - 1 >= (*x)->getX() && playerX + SPRITE_WIDTH - 1 <= (*x)->getX() + SPRITE_WIDTH - 1) && (playerY >= (*x)->getY() && playerY <= (*x)->getY() + SPRITE_HEIGHT - 1))
				return !true;

			if ((playerX >= (*x)->getX() && playerX <= (*x)->getX() + SPRITE_WIDTH - 1) && (playerY + SPRITE_HEIGHT - 1 >= (*x)->getY() && playerY + SPRITE_HEIGHT - 1 <= (*x)->getY() + SPRITE_HEIGHT - 1))
				return !true;

			if ((playerX + SPRITE_WIDTH - 1 >= (*x)->getX() && playerX + SPRITE_WIDTH - 1 <= (*x)->getX() + SPRITE_WIDTH - 1) && (playerY + SPRITE_HEIGHT - 1 >= (*x)->getY() && playerY + SPRITE_HEIGHT - 1 <= (*x)->getY() + SPRITE_HEIGHT - 1))
				return !true;
		}
		x++;
	}
	if (p != a)
	{
		if ((playerX >= p->getX() && playerX <= p->getX() + SPRITE_WIDTH - 1) && (playerY >= p->getY() && playerY <= p->getY() + SPRITE_HEIGHT - 1))
			return !true;

		if ((playerX + SPRITE_WIDTH - 1 >= p->getX() && playerX + SPRITE_WIDTH - 1 <= p->getX() + SPRITE_WIDTH - 1) && (playerY >= p->getY() && playerY <= p->getY() + SPRITE_HEIGHT - 1))
			return !true;

		if ((playerX >= p->getX() && playerX <= p->getX() + SPRITE_WIDTH - 1) && (playerY + SPRITE_HEIGHT - 1 >= p->getY() && playerY + SPRITE_HEIGHT - 1 <= p->getY() + SPRITE_HEIGHT - 1))
			return !true;

		if ((playerX + SPRITE_WIDTH - 1 >= p->getX() && playerX + SPRITE_WIDTH - 1 <= p->getX() + SPRITE_WIDTH - 1) && (playerY + SPRITE_HEIGHT - 1 >= p->getY() && playerY + SPRITE_HEIGHT - 1 <= p->getY() + SPRITE_HEIGHT - 1))
			return !true;
	}
	return !false;
}

bool StudentWorld::findActorToVomitOn(double x, double y) {

	vector<Actor*>::iterator ru = shiver.begin(); 
	while (ru != shiver.end()) {
		if ((*ru)->canExit()) {
			double xx = (*ru)->getX();
			double yy = (*ru)->getY();
			if ((xx - x)*(xx - x) + (yy - y)*(yy - y) <= 100)
				return true; 
		}
		ru++; 
	}
	if ((x - p->getX())*(x - p->getX()) + (y - p->getY())*(y - p->getY()) <= 100)
		return true;

	return false; 
}

bool StudentWorld::citizensAllGone() {
	int citizens = 0;
	vector<Actor*>::iterator yunus = shiver.begin(); 
	while (yunus != shiver.end()) {
		if ((*yunus)->canbeSaved())
			citizens++; 
		yunus++; 
	}
	return citizens == 0; 
}

bool StudentWorld::isFlameBlockedAt(double x, double y) {
	vector<Actor*>::iterator gg = shiver.begin(); 
	while (gg != shiver.end()) {
		if ((*gg)->isWallOrExit()) {
			double xx = (*gg)->getX(); 
			double yy = (*gg)->getY(); 
			if ((xx - x)*(xx - x) + (yy - y)*(yy - y) <= 100)
				return true; //there IS an intersect 
		}
		gg++;
	}

	return false; 
}

double StudentWorld::distanceNearestZombie(double px, double py, Actor* a) {
	vector<Actor*>::iterator hh = shiver.begin(); 
	double minDistance = 100000;
	while (hh != shiver.end()) {
		if ((*hh) == a) {
			hh++;
			continue;
		}
		if ((*hh)->isZombie())
		{
			double xx = (*hh)->getX();
			double yy = (*hh)->getY();
			if (minDistance > sqrt(pow(px - xx, 2) + pow(py - yy, 2)))
				minDistance = sqrt(pow(px - xx, 2) + pow(py - yy, 2));
		}
		hh++; 
	}
	return minDistance; 
}

double StudentWorld::distanceNearestHuman(double px, double py, double& nearX, double& nearY) {
	vector<Actor*>::iterator lbj = shiver.begin(); 
	double minDistance = 100000; 
	double nearx; double neary; 
	while (lbj != shiver.end()) {
		if ((*lbj)->canExit()) {
			double xx = (*lbj)->getX();
			double yy = (*lbj)->getY();
			if (minDistance > sqrt(pow(px - xx, 2) + pow(py - yy, 2))) {
				minDistance = sqrt(pow(px - xx, 2) + pow(py - yy, 2));
				nearx = xx; neary = yy; 
			}
		}
		lbj++; 
	}
	return minDistance; 
}



















