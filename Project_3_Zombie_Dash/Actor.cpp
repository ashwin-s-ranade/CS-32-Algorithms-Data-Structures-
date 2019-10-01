#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(const int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* world) 
: GraphObject(imageID, startX, startY, dir, depth, 1.0){
	m_world = world; 
	setState(true); //sets actor to alive 
}

Penelope::Penelope(double startX, double startY, StudentWorld* world)
: Human(world, IID_PLAYER, startX, startY) {
	m_landmines = 0; 
	m_gascans = 0; 
	m_vaccines = 0; 
	setInfectionStatus(false);
}

ActivatingObject::ActivatingObject(StudentWorld* w, int imageID, double x, double y, int depth, int dir) 
	: Actor (imageID, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, dir, depth, w){}

Exit::Exit(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_EXIT, x, y, 1, right) {}

void Exit::doSomething() {
	getWorld()->activateOnAppropriateActors(this);
}
void Exit::activateIfAppropriate(Actor* a) {
	//if the actor can exit, play the finished level sound and increment the level
	if (a->canbeSaved()) { //assuming citizenPerson
		a->getRekt(); 
		a->setState(false);
		getWorld()->playSound(SOUND_CITIZEN_SAVED);
		getWorld()->playSound(SOUND_CITIZEN_SAVED);
		getWorld()->playSound(SOUND_CITIZEN_SAVED);
		getWorld()->playSound(SOUND_CITIZEN_SAVED);
		getWorld()->increaseScore(1500); //compensate for loss of human
	}
	if (a->canExit() && getWorld()->citizensAllGone()) {
		getWorld()->playSound(SOUND_LEVEL_FINISHED);
		getWorld()->levelFinished();
	}
}

Pit::Pit(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_PIT, x, y, 0, right) {}

void Pit::doSomething() {
	getWorld()->activateOnAppropriateActors(this); 
}

void Pit::activateIfAppropriate(Actor* a) {
	if (a->isAgent()) {
		a->setState(false); 
	}
}

Landmine::Landmine(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_LANDMINE, x, y, 1, right) {
	m_landmineState = 30; 
}

void Landmine::doSomething() {
	if (getState() == false) {
		return; 
	}
	if (getLandmineState() != 0) {
		decrementLandmineState();
		return;
	}
	getWorld()->activateOnAppropriateActors(this); 
}

void Landmine::activateIfAppropriate(Actor* a) {
	if (a->isAgent() || a->isFlame()) {
		setState(false);
		getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
		//introduce 9 flame objects, and pit object at same location as landmine
		if (a->isAgent()) getWorld()->addFlame(getX(), getY(), up);
		
		//hardcoded 8 squares around getX(), getY()
		if (!getWorld()->isFlameBlockedAt(getX() + SPRITE_WIDTH, getY())) {
			getWorld()->addFlame(getX() + SPRITE_WIDTH, getY(), up);
		}
		if (!getWorld()->isFlameBlockedAt(getX() + SPRITE_WIDTH, getY() + SPRITE_HEIGHT)) {
			getWorld()->addFlame(getX() + SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up);
		}
		if (!getWorld()->isFlameBlockedAt(getX() + SPRITE_WIDTH, getY() - SPRITE_HEIGHT)) {
			getWorld()->addFlame(getX() + SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up);
		}
		if (!getWorld()->isFlameBlockedAt(getX() - SPRITE_WIDTH, getY())) {
			getWorld()->addFlame(getX() - SPRITE_WIDTH, getY(), up);
		}
		if (!getWorld()->isFlameBlockedAt(getX() - SPRITE_WIDTH, getY() - SPRITE_HEIGHT)) {
			getWorld()->addFlame(getX() - SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up);
		}
		if (!getWorld()->isFlameBlockedAt(getX() - SPRITE_WIDTH, getY() + SPRITE_HEIGHT)) {
			getWorld()->addFlame(getX() - SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up);
		}
		if (!getWorld()->isFlameBlockedAt(getX(), getY()+SPRITE_HEIGHT)) {
			getWorld()->addFlame(getX(), getY() + SPRITE_HEIGHT, up);
		}
		if (!getWorld()->isFlameBlockedAt(getX(), getY()-SPRITE_HEIGHT)) {
			getWorld()->addFlame(getX(), getY()-SPRITE_HEIGHT, up);
		}
		getWorld()->addPit(getX(), getY()); 
	}
	
}

Flame::Flame(StudentWorld* w, double x, double y, int dir)
	: ActivatingObject(w, IID_FLAME, x, y, 0, dir) {
	m_flameState = 2; 
}

void Flame::doSomething() {
	getWorld()->activateOnAppropriateActors(this);
	decrementFlame(); //after 2 ticks, destroy flame
}

void Flame::activateIfAppropriate(Actor* a) {
	if (getFlameState() == 0) {
		setState(false); //set the state of flame to dead 
		return; //return if the flame is already dead 
	}
	if (a->canGetFlamed()) {

		a->setState(false); //kills actor 
	}
}

Vomit::Vomit(StudentWorld* w, double x, double y,int dir)
	: ActivatingObject(w, IID_VOMIT, x, y, 0, dir) {
	getWorld()->playSound(SOUND_ZOMBIE_VOMIT); 
	m_vomitState = 2; 
}

void Vomit::doSomething() {
	if (!getState())
		return;
	getWorld()->activateOnAppropriateActors(this); 
	decrementVomitState(); 
	if (getVomitState() == 0)
		setState(false); 
}

void Vomit::activateIfAppropriate(Actor* a) {
	if (a->canExit() && a->getState()){
		if (!a->canPickUp() && a->getState())
			getWorld()->playSound(SOUND_CITIZEN_INFECTED); 
		a->setInfectionStatus(true); 
		
	}
}



//goodies
Goodie::Goodie(StudentWorld* w, int imageID, double x, double y)
	: ActivatingObject(w, imageID, x, y, 1, right) {}

bool Goodie::activateHelper(Actor* a) {
	if (a->canPickUp()) {
		getWorld()->increaseScore(50); 
		setState(false); //set state of goodie to dead 
		getWorld()->playSound(SOUND_GOT_GOODIE);
		return true; //we picked it up
	}
	return false; //we didn't pick it up
}

VaccineGoodie::VaccineGoodie(StudentWorld* w, double x, double y)
	: Goodie(w, IID_VACCINE_GOODIE, x, y) {}

void VaccineGoodie::activateIfAppropriate(Actor* a) {
	if (activateHelper(a)) 
		getWorld()->changeGoodies('v', 1); //increase vaccines by 1
}

void VaccineGoodie::doSomething() {
	getWorld()->activateOnAppropriateActors(this); 
}

LandmineGoodie::LandmineGoodie(StudentWorld* w, double x, double y)
	: Goodie(w, IID_LANDMINE_GOODIE, x, y) {}

void LandmineGoodie::doSomething() {
	getWorld()->activateOnAppropriateActors(this);
}

void LandmineGoodie::activateIfAppropriate(Actor* a) {
	if (activateHelper(a))
		getWorld()->changeGoodies('l', 2); //increase landmines by 2
}

GasCanGoodie::GasCanGoodie(StudentWorld* w, double x, double y)
	: Goodie(w, IID_GAS_CAN_GOODIE, x, y) {}

void GasCanGoodie::doSomething() {
	getWorld()->activateOnAppropriateActors(this);
}

void GasCanGoodie::activateIfAppropriate(Actor* a) {
	if (activateHelper(a))
		getWorld()->changeGoodies('g', 5); //increase gas cans by 5
}


//agents
Agent::Agent(StudentWorld* w, int imageID, double x, double y, int dir) :
	Actor(imageID, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, dir,0, w) {}

Human::Human(StudentWorld* w, int imageID, double x, double y)
	: Agent(w, imageID, x, y, right) {
	m_infectionCount = 0; 
}

Citizen::Citizen(StudentWorld* w, double x, double y)
	: Human(w, IID_CITIZEN, x, y) { 
	setInfectionStatus(false); 
	paralyzed = 0; 
}

void Citizen::getRekt() {
	getWorld()->playSound(SOUND_CITIZEN_DIE); 
	getWorld()->increaseScore(-1000);
	getWorld()->citizenDec(); 
}

void Citizen::doSomething() {
	if (getInfectionStatus())
		incrementInfectionCount(1);
	if (paralyzed % 2 == 1) {
		paralyzed++;
		return;
	}
	else
		paralyzed++; 

	if (!getState())
		return;
 
	if (getInfectionCount() == 500) {
		setState(false); 
		getWorld()->playSound(SOUND_ZOMBIE_BORN); 
		getWorld()->increaseScore(-1000); 
		getWorld()->citizenDec(); 
		int x = randInt(1, 10); 
		if (x >= 4)
			getWorld()->addActor(new DumbZombie(getWorld(), getX()/16, getY()/16));
		else
			getWorld()->addActor(new SmartZombie(getWorld(), getX()/16, getY()/16));
		return; 
	}
	//determine distance to penelope
	double dist_p = getWorld()->distPenelope(getX(), getY()); 
	double dist_z = getWorld()->distanceNearestZombie(getX(), getY(),this); 

	//number 6 :0

	if (dist_p < dist_z && dist_p <= 80) {
		double penelopex = getWorld()->getPenelope()->getX(); 
		double penelopey = getWorld()->getPenelope()->getY(); 
		if (getX() == penelopex || getY() == penelopey) {
			if (getX() ==penelopex) { //in the same x 
				if (penelopey - getY() > 0 && getWorld()->nointersect(getX(), getY() + 2, this)) { //we go up
					setDirection(up); 
					moveTo(getX(), getY() + 2); 
					return; 
				}
				else if (penelopey - getY() < 0 && getWorld()->nointersect(getX(), getY() - 2, this)) { //we go down
					setDirection(down); 
					moveTo(getX(), getY() - 2);
					return;
				}
			}
			else { //in the same y 
					if (penelopex - getX() > 0 && getWorld()->nointersect(getX() + 2, getY(), this)) { //we go right
						setDirection(right); 
						moveTo(getX() + 2, getY()); 
						return; 
					}
					else if (getX() - penelopex> 0 && getWorld()->nointersect(getX() - 2, getY(), this)) { //we go left
						setDirection(left); 
						moveTo(getX() - 2, getY());
						return; 
					}
				}
			}
		else{
			//determine on horizontal and one vertical direction
			int dog = randInt(1, 2); 
			if (dog == 1) { //try vertical 
				if (getY() - penelopey > 0) { //go down
					//check if being blocked 
					if (getWorld()->nointersect(getX(), getY() - 2, this)) {
						setDirection(down);
						moveTo(getX(), getY() - 2);
						return;
					}
					else { //we are being blocked, check left and right
						double left1 = 100000; 
						double right1 = 100000; 
						if (getWorld()->nointersect(getX() - 2, getY(), this)) //right
							left1 = getWorld()->distPenelope(getX() - 2, getY()); 
						if (getWorld()->nointersect(getX() + 2, getY(), this))
							right1 = getWorld()->distPenelope(getX() + 2, getY()); 
						if (right < left) { //go right 
							setDirection(right);
							moveTo(getX() + 2, getY());
							return;
						}
						else if (left1 < right1) {
							setDirection(left);
							moveTo(getX() - 2, getY());
							return;
						}
						else
							return; 
					}

				}
				else { //try to go up
					if (getWorld()->nointersect(getX(), getY() + 2, this)) {
						setDirection(up);
						moveTo(getX(), getY() + 2);
						return;
					}
					else { //we are being blocked, check left and right 
						double left1 = 100000;
						double right1 = 100000;
						if (getWorld()->nointersect(getX() - 2, getY(), this)) //right
							left1 = getWorld()->distPenelope(getX() - 2, getY());
						if (getWorld()->nointersect(getX() + 2, getY(), this))
							right1 = getWorld()->distPenelope(getX() + 2, getY());
						if (right1 < left1) { //go right 
							setDirection(right);
							moveTo(getX() + 2, getY());
							return;
						}
						else if (left1 < right1) {
							setDirection(left);
							moveTo(getX() - 2, getY());
							return;
						}
						else
							return;
					}
				}
			}
			else { //try horizontal 
				if (getX() - penelopex > 0) {//move left 
					//check if being blocked
					if (getWorld()->nointersect(getX() - 2, getY(), this)) {
						setDirection(left);
						moveTo(getX() - 2, getY());
						return;
					}
					else { //we are being blocked, check up and down 
						double up1 = 100000;
						double down1 = 100000;
						if (getWorld()->nointersect(getX(), getY() - 2, this)) //downs
							down1 = getWorld()->distPenelope(getX(), getY() - 2);
						if (getWorld()->nointersect(getX(), getY() + 2, this))
							up1 = getWorld()->distPenelope(getX(), getY() + 2);
						if (up1 < down1) { //go up
							setDirection(up);
							moveTo(getX(), getY() + 2);
							return;
						}
						else if (down1 < up1) {
							setDirection(down);
							moveTo(getX(), getY() - 2);
							return;
						}
						else
							return;
					}
				}
				else { //try to go right 
					if (getWorld()->nointersect(getX() + 2, getY(), this)) {
						setDirection(right);
						moveTo(getX() + 2, getY());
						return;
					}
					else { //we are being blocked, check up and down 
						double up1 = 100000;
						double down1 = 100000;
						if (getWorld()->nointersect(getX(), getY() - 2, this)) //downs
							down1 = getWorld()->distPenelope(getX(), getY() - 2);
						if (getWorld()->nointersect(getX(), getY() + 2, this))
							up1 = getWorld()->distPenelope(getX(), getY() + 2);
						if (up1 < down1) { //go up
							setDirection(up);
							moveTo(getX(), getY() + 2);
							return;
						}
						else if (down1 < up1) { //go down
							setDirection(down);
							moveTo(getX(), getY() - 2);
							return;
						}
						else
							return;
					}
				}
			}
			}
		}

		
		//NUMBER 7 ESKETIT
		if (dist_z <= 80) {
			//for each of the 4 directions
			//for up
			double distzup = 0;
			double distzdown = 0; 
			double distzright = 0; 
			double distzleft = 0; 
			if (getWorld()->nointersect(getX(), getY() + 2, this)) { //up
				distzup = getWorld()->distanceNearestZombie(getX(), getY() + 2,this);
			}
			if (getWorld()->nointersect(getX(), getY() - 2, this)) { //down
				distzdown = getWorld()->distanceNearestZombie(getX(), getY() - 2,this);
			}
			if (getWorld()->nointersect(getX()-2, getY(), this)) { //left
				distzleft = getWorld()->distanceNearestZombie(getX()-2, getY(),this);
			}
			if (getWorld()->nointersect(getX() + 2, getY(), this)) { //right
				distzright = getWorld()->distanceNearestZombie(getX() + 2, getY(),this);
			}
			if (dist_z > distzup && dist_z > distzdown && dist_z > distzright && dist_z > distzleft)
				return; //we are already in an optimal position
			double lowest = max(max(distzup, distzdown), max(distzright, distzleft));
 
			if (lowest == distzup) {
				setDirection(up); 
				moveTo(getX(), getY() + 2);
				return;
			}
			else if (lowest == distzdown) {
				setDirection(down); 
				moveTo(getX(), getY() - 2); 
				return; 
			}
			else if (lowest == distzright) {
				setDirection(right); 
				moveTo(getX() + 2, getY());
				return; 
			}
			else if (lowest == distzleft) {
				setDirection(left); 
				moveTo(getX() - 2, getY()); 
				return; 
			}

		}
		
	}



Zombie::Zombie(StudentWorld* w, double x, double y) :
	Agent(w, IID_ZOMBIE, x, y, right) {
	m_movementPlanDistance = 0; 
} //assuming direction as right

SmartZombie::SmartZombie(StudentWorld* w, double x, double y)
	: Zombie(w, x, y) {}

void SmartZombie::getRekt() {
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	getWorld()->increaseScore(2000);
}

void SmartZombie::doSomething() {
	if (!getState()) {
		return;
	}
	if (paralyzed % 2 == 1) {
		paralyzed++;
		return;
	}
	else
		paralyzed++;
	//compute vomit coordinates 
	double vomitx = getX(); double vomity = getY();
	switch (getDirection()) {
	case 0: //right
		vomitx += 16;
		break;
	case 90: //up
		vomity += 16;
		break;
	case 180: //left
		vomitx -= 16;
		break;
	case 270: //down
		vomity -= 16;
		break;
	}
	if (getWorld()->findActorToVomitOn(vomitx, vomity)) {
		int prob = randInt(1, 3);
		if (prob == 1) {
			Actor* a = new Vomit(getWorld(), vomitx / 16, vomity / 16, getDirection()); 
			getWorld()->addActor(a);
			a->setInfectionStatus(true); 
			return;
		}
	}

	//get new movement plan
	if (getMovementPlan() == 0) {
		modifyMovementPlan(randInt(3, 10));
		double nearX; double nearY; 
		double distClosestHuman = getWorld()->distanceNearestHuman(getX(), getY(), nearX, nearY); 
		if (distClosestHuman > 80) {
			setDirection(randInt(0, 3) * 90);
		}
		else { //<=80
			if (nearX == getX()) { //same row/ vertical line
				if (nearY > getY())
					setDirection(up);
				else
					setDirection(down); 
			}
			else if (nearY == getY()) { //same horizontal line
				if (nearX > getX())
					setDirection(right);
				else
					setDirection(left); 
			}
			else {//choose randomly between the two directions that get the zombie closer
				int direction1; 
				int direction2;
				if (nearX > getX() && nearY > getY()) { //quadrant I 
					direction1 = up; direction2 = right; 
				}
				else if (nearX > getX() && nearY < getY()) { //quadrant IV
					direction1 = down; direction2 = right; 
				}
				else if (nearX < getX() && nearY > getY()) { //quadrant II
					direction1 = up; direction2 = left; 
				}
				else { //quadrant III
					direction1 = down; direction2 = left; 
				}
				if (randInt(1, 2) == 1)
					setDirection(direction1);
				else
					setDirection(direction2); 

			}
		}
	}

	double dest_x = getX(); double dest_y = getY();
	switch (getDirection()) {
	case 0:
		dest_x += 1;
		break;
	case 90:
		dest_y += 1;
		break;
	case 180:
		dest_x -= 1;
		break;
	case 270:
		dest_y -= 1;
		break;
	}
	if (getWorld()->nointersect(dest_x, dest_y, this)) {
		moveTo(dest_x, dest_y);
		modifyMovementPlan(-1);
	}
	else {
		modifyMovementPlan(-1 * getMovementPlan()); //set movement plan to 0
	}
}

DumbZombie::DumbZombie(StudentWorld* w, double x, double y)
	: Zombie(w, x, y) {}

void DumbZombie::getRekt() {
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	getWorld()->increaseScore(1000);
	if (randInt(1, 10) == 1) {
		getWorld()->addActor(new VaccineGoodie(getWorld(), getX()/16, getY()/16)); 
	}
	//setState(false); is this necessary? 
}

void DumbZombie::doSomething() {
	if (!getState()) {
		return; 
	}
	if (paralyzed % 2 == 1) {
		paralyzed++;
		return;
	}
	else
		paralyzed++;
	//compute vomit coordinates 
	double vomitx = getX(); double vomity = getY(); 
	switch (getDirection()) {
	case 0: //right
		vomitx += 16;
		break;
	case 90: //up
		vomity += 16;
		break;
	case 180: //left
		vomitx -= 16;
		break;
	case 270: //down
		vomity -= 16;
		break;
	}
	if (getWorld()->findActorToVomitOn(vomitx, vomity)) {
		int prob = randInt(1, 3); 
		if (prob == 1) {
			Actor* a = new Vomit(getWorld(), vomitx / 16, vomity / 16, getDirection());
			getWorld()->addActor(a); 
			a->setInfectionStatus(true); 
			return; 
		}
	}
	//get new movement plan
	if (getMovementPlan() == 0) {
		modifyMovementPlan(randInt(3, 10)); 
		setDirection(randInt(0, 3) * 90); 
	}
	double dest_x = getX(); double dest_y = getY(); 
	switch (getDirection()) {
	case 0: 
		dest_x += 1; 
		break;
	case 90: 
		dest_y += 1; 
		break; 
	case 180: 
		dest_x -= 1; 
		break;
	case 270: 
		dest_y -= 1; 
		break;
	}
	if (getWorld()->nointersect(dest_x, dest_y, this)) {
		moveTo(dest_x, dest_y); 
		modifyMovementPlan(-1); 
	}
	else {
		modifyMovementPlan(-1 * getMovementPlan()); //set movement plan to 0
	}
}

void Penelope::doSomething() {
	if (!getState())
		return; 
	if (getInfectionStatus())
		incrementInfectionCount(1); 
	if (getInfectionCount() == 500) {
		setState(false); 
		getWorld()->playSound(SOUND_PLAYER_DIE); 
		return; 
	}



	int ch; 
	if (getWorld()->getKey(ch)) {
		//user hit a key
		switch (ch) {
		case KEY_PRESS_LEFT:
			setDirection(left);
			if (getWorld()->nointersect(getX() - 4, getY(), this))
				moveTo(getX() - 4, getY());
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if (getWorld()->nointersect(getX() + 4, getY(),this))
				moveTo(getX() + 4, getY());
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (getWorld()->nointersect(getX(), getY() - 4,this))
				moveTo(getX(), getY() - 4);
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (getWorld()->nointersect(getX(), getY() + 4,this))
				moveTo(getX(), getY() + 4);
			break;
		case KEY_PRESS_ENTER:
			if (getGoodies('v') > 0) {
				setInfectionStatus(false);
				incrementInfectionCount(-1*getInfectionCount()); 
				getWorld()->changeGoodies('v', -1); 
			}
		case KEY_PRESS_TAB:
			if (getGoodies('l') == 0) {
				break; //if we have landmines in inventory 
			}
			getWorld()->addLandmine(getX(), getY());
			getWorld()->changeGoodies('l', -1); //decrement landmines
			break;

		case KEY_PRESS_SPACE: //flame
			if (getGoodies('g') == 0)
				break; //break if 0 fire charges left
			getWorld()->changeGoodies('g', -1); //decrement flamethrower charge
			//check if flame will overlap with anything
			//start with up only
			
			int x; int y;
			switch (getDirection())
			{
			case up: //CASE UP
				for (int k = 1; k <= 3; k++) {
					x = 0; y = k;
					double posix = getX() + x * SPRITE_WIDTH;
					double posiy = getY() + y * SPRITE_HEIGHT;
					if (getWorld()->isFlameBlockedAt(posix, posiy))
						break; //or return? 
					getWorld()->addFlame(posix, posiy, up);

				}
				break;
			case down:
				for (int k = 1; k <= 3; k++) {
					x = 0; y = k;
					double posix = getX() + x * SPRITE_WIDTH;
					double posiy = getY() - y * SPRITE_HEIGHT;
					if (getWorld()->isFlameBlockedAt(posix, posiy))
						break; //or return? 
					getWorld()->addFlame(posix, posiy, down);
				}
				break;
			case right:
				for (int k = 1; k <= 3; k++) {
					x = k; y = 0;
					double posix = getX() + x * SPRITE_WIDTH;
					double posiy = getY() - y * SPRITE_HEIGHT;
					if (getWorld()->isFlameBlockedAt(posix, posiy))
						break; //or return? 
					getWorld()->addFlame(posix, posiy, right); 

				}
				break;
			case left:
				for (int k = 1; k <= 3; k++) {
					x = k; y = 0;
					double posix = (getX()) - x * SPRITE_WIDTH;
					double posiy = (getY()) - y * SPRITE_HEIGHT;
					if (getWorld()->isFlameBlockedAt(posix, posiy))
						break; //or return? 
					getWorld()->addFlame(posix, posiy, left);
				}
				break;
			}


			getWorld()->playSound(SOUND_PLAYER_FIRE);
			break;

		}
	}
}


Wall::Wall (StudentWorld* world, double startX, double startY)
	: Actor(IID_WALL, SPRITE_WIDTH*startX, SPRITE_HEIGHT*startY, 0, 0, world) {}
