#include "Actor.h"
#include <cmath>

#include "StudentWorld.h"

//Actor ================================================================================================
Actor::Actor(int ImageID, double startX, double startY, Direction startDirection, int depth, StudentWorld* petriDish, double HP, bool damageable)
	: GraphObject(ImageID, startX, startY, startDirection, depth)
{
	m_alive = true;
	m_world = petriDish;
	m_ImageID = ImageID;
	m_damageable = damageable;
	m_health = HP;
}

Actor::~Actor()
{

}

void Actor::doSomething()/////////////////////////
{

}

bool Actor::deadOrAlive()   //true for alive, false for dead
{
	return m_alive;
}

void Actor::setDead()  //sets an actor as dead
{
	m_alive = false;
}

StudentWorld* Actor::getWorld()  //return pointer to the world
{
	return m_world;
}

void Actor::setObjectType(string s) //mark an actor as a certain type (can be "Salmonella", "Food", etc.)
{
	type = s;
}

string Actor::returnObjectType()  //return the type that was specified by the above function
{
	return type;
}

double Actor::getHP()  //return how many health points an actor has
{
	return m_health;
}


double Actor::euclideanDistance(const Actor* actor1)  //finds the distance betwwen two actors
{
	double x1 = this->getX();
	double y1 = this->getY();
	double x2 = actor1->getX();
	double y2 = actor1->getY();

	double foo = (double)sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
	
	return foo;
}


bool Actor::isItDamageable() const  //lets the program know if something can be damaged or not
{
	return m_damageable;
}

void Actor::takeDamage(int damage)  //actor will take a specified amount of damage
{
	m_health = m_health - damage;

	if (getHP() <= 0)
	{
		setDead();
	}

}

void Actor::setHP(double hp)   //sets actor's health points to a specified amount
{  
	m_health = hp;
}

bool Actor::isEdible()  //notifies whether or not something is a food object
{
	return false;
}

bool Actor::isDirty()  //notifies whether or not something is a dirt object
{
	return false;
}

bool Actor::isBacteria()  //notifies whether or not something is a bacteria object
{
	return false;
}

bool Actor::isGoodie()  //notifies whether or not something is a goodie object
{
	return false;
}


//Socrates ==================================================================================================================
Socrates::Socrates(StudentWorld* petriDish)
	:Actor(IID_PLAYER, 0, 128, 0, 0, petriDish, 100, true) 
{
	socratesPositionalAngle = 3.1415926535;
	m_sprayAmmo = 20;
	m_flameAmmo = 5;///////////////////////////
	m_lives = 3;
	setHP(100);
	setObjectType("Socrates");
}

Socrates::~Socrates()
{
	
}

void Socrates::doSomething()  //allows Socrates to do something based on different key presses
{
	if (deadOrAlive() == false)
	{
		return;
	}

	int x;
	if (getWorld()->getKey(x))
	{
		switch (x)
		{
		case KEY_PRESS_SPACE:  //shoot spray
			if (m_sprayAmmo > 0)
			{
				shootSpray();	
			}
			break;

		case KEY_PRESS_ENTER:  //shoot flame
			if (m_flameAmmo >= 1)
			{
				shootFlame();				
			}
			break;

		case KEY_PRESS_LEFT:    //move counterclockwise
		{
			socratesPositionalAngle += ((3.1415926535 / 180) * 5);    //convert 5 degrees to radians	
			moveTo((128 * cos(socratesPositionalAngle) + 128), (128 * sin(socratesPositionalAngle) + 128));
			setDirection(getDirection() + 5);
			
			break;
		}		
		break; 

		case KEY_PRESS_RIGHT:  //move clockwise
		{
			socratesPositionalAngle -= ((3.1415926535 / 180) * 5);    //convert 5 degrees to radians
			moveTo((128 * cos(socratesPositionalAngle)+128), (128 * sin(socratesPositionalAngle))+128);
			setDirection(getDirection() - 5);
			
			break;
		}
		break;
		}		
	}

	else
	{
		if (m_sprayAmmo < 20)  //reloading spray ammo if no key was pressed 
			m_sprayAmmo++;
	}
}

void Socrates::addHP(int amount)  //add specified amount of health points to Socrates (cannot exceed 100 health points)
{
	if (getHP() + amount > 100) 
	{
		setHP(100);
	}
	else 
	{		
		setHP(getHP() + amount);
	}
}


void Socrates::shootSpray()  //Socrates shoots a spray in the direction he is facing, spray dies if it goes too far
{
	double endX;
	double endY;

	getPositionInThisDirection(getDirection(), SPRITE_RADIUS, endX, endY);

	Spray* newSpray = new Spray(endX, endY, getDirection(), getWorld());

	getWorld()->addActor(newSpray);
	getWorld()->playSound(SOUND_PLAYER_SPRAY);
	m_sprayAmmo--;	
}


int Socrates::getSprayAmount()  //returns amount of spray Socrates has
{
	return m_sprayAmmo;
}

void Socrates::shootFlame()  // add the 16 flame actors into the studentworld surrounding Socrates
{
	double endX;
	double endY;

	for (int i = 0; i < 16; i++)
	{
		getPositionInThisDirection(getDirection() + (i * 22), SPRITE_RADIUS, endX, endY);
		Flame* newFlame = new Flame(endX, endY, getDirection() + (i * 22), getWorld());
		getWorld()->addActor(newFlame);
	}
		
	getWorld()->playSound(SOUND_PLAYER_FIRE);
	m_flameAmmo--;
}

int Socrates::getFlameAmount()  //returns amount of flames Socrates has left
{
	return m_flameAmmo;
}

void Socrates::increaseFlameAmmo()  //increase the amount of flames Socrates has by 5
{
	m_flameAmmo += 5;
}

//Dirt =======================================================================================================
Dirt::Dirt(double startX, double startY, StudentWorld* petriDish)
	:Actor(IID_DIRT, startX, startY, 0, 1, petriDish, 0, true)
{
	setObjectType("Dirt");
}

void Dirt::doSomething()
{
	
}

Dirt::~Dirt()  //decrement the amount of Dirt 
{
	getWorld()->accessMapActors("Dirt");
}

bool Dirt::isDirty()  //indicates that the object is a dirt
{
	return true;
}


//Projectiles ========================================================================================================
Projectile::Projectile(int ImageID, double startX, double startY, int direction, int depth, StudentWorld* petriDish)
	:Actor(ImageID, startX, startY, direction, depth, petriDish, 0, false)
{

}
Projectile::~Projectile()
{

}



Flame::Flame(double startX, double startY, int direction,  StudentWorld* petriDish)
	:Projectile(IID_FLAME, startX, startY, direction, 1, petriDish)
{
	startingX = startX;
	startingY = startY;
	flameMoveDistance = 32;
	setObjectType("Flame");
}

Flame::~Flame()
{
	//getWorld()->accessMapActors("Flame");
}

void Flame::doSomething()   //allows flame to do things such as damage actors, disappear after travelling certain distance
{
	if (deadOrAlive() == false)
	{
		return;
	}
	
	moveForward(SPRITE_RADIUS * 2);

	if (Flame::distanceTraveled(startingX, startingY) == false)
	{
		setDead();
	}

	if (getWorld()->flameDamage(this) == true)
	{
		setDead();
		return;
	}
}

bool Flame::distanceTraveled(double a, double b)  //returns false if flame has travelled too far
{
	double x1 = getX();
	double y1 = getY();
	
	double distance = sqrt(pow((x1 - a), 2) + pow((y1 - b), 2));

	if (distance >= 32)
	{
		return false;
	}
	return true;
}


Spray::Spray(double startX, double startY, int direction,StudentWorld* petriDish)
	:Projectile(IID_SPRAY, startX, startY, direction, 1, petriDish)
{
	startingX = startX;
	startingY = startY;
	sprayMoveDistance = 112;
	setObjectType("Spray");
}

Spray::~Spray()
{
	
}

void Spray::doSomething()  //allows spray to do things such as damage actors, disappear after travelling certain distance
{
	if (!deadOrAlive())
	{
		return;
	}

	moveForward(SPRITE_RADIUS * 2);

	if (getWorld()->sprayDamage(this))
	{
		//cout << " spray damage true\n";
		setDead();
		return;
	}

	if (!distanceTraveled(startingX, startingY))
	{
		setDead();	
	}

}

bool Spray::distanceTraveled(double a, double b)  //returns false if the spray has travelled too far
{
	double x1 = getX();
	double y1 = getY();

	double distance = sqrt(pow((x1 - a), 2) + pow((y1 - b), 2));

	if (distance >= 112)
	{
		return false;
	}
	return true;
}



//Goodies ================================================================================================================
Goodies::Goodies(int ImageID, double angle, StudentWorld* petriDish)
	:Actor(ImageID, 128 * cos(angle) + 128, 128 * sin(angle) + 128, 0, 1, petriDish, 0, true)
{
	lifetime = max(rand() % (300 - 10 * getWorld()->getLevel()), 50);
	livingTime = 0;
}

Goodies::~Goodies()
{

}

void Goodies::function()
{

}


void Goodies::doSomething()   //sets goodie as dead if it has exceeded its alloted lifetime, does its function if it comes in contact with Socrates
{
	if (livingTime >= lifetime)
	{
		setDead();
	}

	if (deadOrAlive() == false)
	{
		return;
	}

	if (this->euclideanDistance(getWorld()->returnSocrates()) <= 8)
	{
		function();
	}

	livingTime++;   //increase living time every tick; if living time is greater than or equal to lifetime the goodie will die
}

bool Goodies::isGoodie()  //indicates that the object is a goodie
{
	return true;
}



Fungi::Fungi(StudentWorld* petriDish)
	:Goodies(IID_FUNGUS, (rand() % 360 * (3.1415926535 / 180)), petriDish)
{
	setObjectType("Fungi");
}

Fungi::~Fungi()
{

}


void Fungi::function() 
{
	//this subtracts 50 points; reduces Socrates's health by 20
	getWorld()->increaseScore(-50);
	setDead();
	getWorld()->returnSocrates()->addHP(-20);
	return;
}



HealthPack::HealthPack(StudentWorld* petriDish)
	:Goodies(IID_RESTORE_HEALTH_GOODIE, (rand() % 360 * (3.1415926535 / 180)), petriDish)
{
	setObjectType("HealthPack");
	
}

HealthPack::~HealthPack()
{

}


void HealthPack::function()
{
	//this adds 250 points; restores Socrates's health
	getWorld()->increaseScore(250);
	setDead();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->returnSocrates()->setHP(100);
}


ExtraLifePack::ExtraLifePack(StudentWorld* petriDish)
	:Goodies(IID_EXTRA_LIFE_GOODIE, (rand() % 360 * (3.1415926535 / 180)), petriDish)
{
	setObjectType("ExtraLifePack");
}

ExtraLifePack::~ExtraLifePack()
{
	
}

void ExtraLifePack::function()
{
	//this adds 500 points; gives Socrates an extra life
	getWorld()->increaseScore(500);
	setDead();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->incLives();
}


FlamePack::FlamePack(StudentWorld* petriDish)
	:Goodies(IID_FLAME_THROWER_GOODIE, (rand() % 360 * (3.1415926535 / 180)), petriDish)
{
	setObjectType("FlamePack");
}

FlamePack::~FlamePack()
{
	
}


void FlamePack::function()
{
	//gives Socrates five additional flame charges, increases score by 300
	getWorld()->increaseScore(300);
	setDead();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->returnSocrates()->increaseFlameAmmo();
}


//Pit =====================================================================================================================
Pit::Pit(double startX, double startY, StudentWorld* petriDish)
	:Actor(IID_PIT, startX, startY, 0, 1, petriDish, 0, false)
{
	setObjectType("Pit");
	m_salmonella = 5;
	m_aggressiveSalmonella = 3;
	m_Ecoli = 2;
}

void Pit::doSomething()   //details what the pit should do (emit bacteria at random time intervals based on what bacteria are still left in the pit)
{	
	if (isDepleted() == true)
	{
		setDead();
	}
	
	int x = randInt(1, 50);
	if (x == 10)
	{
		if (returnSalmonella() > 0 && returnAggressive() > 0 && returnEcoli() > 0)   //still have at least one of each bacteria
		{
			int y = randInt(1, 3);
			if (y == 1)
			{
				getWorld()->addActor(new Salmonella(getX(), getY(), getWorld()));
				m_salmonella--;
				getWorld()->playSound(SOUND_BACTERIUM_BORN);
			}

			if (y == 2)
			{
				getWorld()->addActor(new aggressiveSalmonella(getX(), getY(), getWorld()));
				m_aggressiveSalmonella--;
				getWorld()->playSound(SOUND_BACTERIUM_BORN);
			}

			if (y == 3)
			{
				getWorld()->addActor(new Ecoli(getX(), getY(), getWorld()));
				m_Ecoli--;
				getWorld()->playSound(SOUND_BACTERIUM_BORN);
			}
		}

		
		else if (!(returnSalmonella() > 0) && returnAggressive() > 0 && returnEcoli() > 0)  //no more regular Salmonella remaining
		{
			int z = randInt(1, 2);
			if (z == 1)
			{
				getWorld()->addActor(new aggressiveSalmonella(getX(), getY(), getWorld()));
				m_aggressiveSalmonella--;
				getWorld()->playSound(SOUND_BACTERIUM_BORN);
			}

			if (z == 2)
			{
				getWorld()->addActor(new Ecoli(getX(), getY(), getWorld()));
				m_Ecoli--;
				getWorld()->playSound(SOUND_BACTERIUM_BORN);
			}
		}

		else if (returnSalmonella() > 0 && !(returnAggressive() > 0) && returnEcoli() > 0)  //no more aggressive Salmonella remaining
		{
			int a = randInt(1, 2);
			if (a == 1)
			{
				getWorld()->addActor(new Salmonella(getX(), getY(), getWorld()));
				m_salmonella--;
				getWorld()->playSound(SOUND_BACTERIUM_BORN);
			}

			if (a == 2)
			{
				getWorld()->addActor(new Ecoli(getX(), getY(), getWorld()));
				m_Ecoli--;
				getWorld()->playSound(SOUND_BACTERIUM_BORN);
			}
		}

		else if (returnSalmonella() > 0 && returnAggressive() > 0 && !(returnEcoli() > 0))  //no more Ecoli remaining
		{
			int b = randInt(1, 2);
			if (b == 1)
			{
				getWorld()->addActor(new Salmonella(getX(), getY(), getWorld()));
				m_salmonella--;
				getWorld()->playSound(SOUND_BACTERIUM_BORN);
			}

			if (b == 2)
			{
				getWorld()->addActor(new aggressiveSalmonella(getX(), getY(), getWorld()));
				m_aggressiveSalmonella--;
				getWorld()->playSound(SOUND_BACTERIUM_BORN);
			}
		}

		else if (returnSalmonella() > 0 && !(returnAggressive() > 0) && !(returnEcoli() > 0))  //only regular Salmonella remaining
		{
			getWorld()->addActor(new Salmonella(getX(), getY(), getWorld()));
			m_salmonella--;
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
		}

		else if (!(returnSalmonella() > 0) && returnAggressive() > 0 && !(returnEcoli() > 0)) //only aggressive Salmonella remaining
		{
			getWorld()->addActor(new aggressiveSalmonella(getX(), getY(), getWorld()));
			m_aggressiveSalmonella--;
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
		}

		else if (!(returnSalmonella() > 0) && !(returnAggressive() > 0) && returnEcoli() > 0)  //only Ecoli remaning
		{
			getWorld()->addActor(new Ecoli(getX(), getY(), getWorld()));
			m_Ecoli--;
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
		}		
	}	
}

Pit::~Pit()
{
	getWorld()->accessMapActors("Pit");
	
}

bool Pit::isDepleted()   //returns true if the pit has released all of its bacteria already
{
	if (m_salmonella == 0 && m_aggressiveSalmonella == 0 && m_Ecoli == 0)
	{
		return true;
	}
	return false;
}

int Pit::returnSalmonella()  //returns amount of salmonella still in the pit
{
	return m_salmonella;
}

int Pit::returnAggressive()  //returns amount of aggressive salmonella still in the pit
{
	return m_aggressiveSalmonella;
}

int Pit::returnEcoli()  //returns amount of E coli still in the pit
{
	return m_Ecoli;
}


//Food =====================================================================================================================
Food::Food(double startX, double startY, StudentWorld* petriDish)
	:Actor(IID_FOOD, startX, startY, 90, 1, petriDish, 0, false)
{
	setObjectType("Food");
	damageable = false;
}

bool Food::isEdible()  //indicates that the object is a food
{
	return true;
}

void Food::doSomething()
{
	
}

Food::~Food()
{
	getWorld()->accessMapActors("Food");
}

// Bacteria =====================================================================================================

Bacteria::Bacteria(int ImageID, double startX, double startY, int HP, StudentWorld* petriDish)
	:Actor(ImageID, startX, startY, 90, 0, petriDish, HP, true)
{
	mvmtPlan = 0;
	m_foodEaten = 0;
}

Bacteria::~Bacteria()
{
	
}

int Bacteria::returnFoodEaten()  //returns the amount of food a bacteria has eaten
{
	return m_foodEaten;
}

void Bacteria::eatFood() //increases the amount of food the bacteria has eaten by one
{
	m_foodEaten += 1;	
}

int Bacteria::returnMvmtPlanDist()  //returns the bacterias current movement plan distance
{
	return mvmtPlan;
}

void Bacteria::setMvmtPlan(int x)  //sets the bacterias movement plan distance
{
	mvmtPlan = x;
}

void Bacteria::setFoodEaten(int amount)  //sets the amount of food the bacteria has eaten
{
	m_foodEaten = amount;
}

bool Bacteria::isBacteria()  //indicates that the actor is a bacteria
{
	return true;
}




Salmonella::Salmonella(double startX, double startY, StudentWorld* petriDish)
	:Bacteria(IID_SALMONELLA, startX, startY, 4, petriDish)
{
	setObjectType("Salmonella");
	
}

Salmonella::~Salmonella()
{
	getWorld()->accessMapActors("Salmonella");
}

void Salmonella::doSomething()   //details what the salmonella does on each tick (looks for food, changes direction if blocked, etc)
{
	//step 1
	if (deadOrAlive() == false)
	{
		return;
	}

	
	//step 2
	else if (euclideanDistance(getWorld()->returnSocrates()) <= SPRITE_WIDTH)  //hurts Socrates if they come in contact
	{
		getWorld()->hurtSocrates(1);
	}

	
	//step 3
	else if (returnFoodEaten() == 3)  //reproduce if it has eaten three food
	{
		double newXCoord = getX();
		double newYCoord = getY();

		if (newXCoord < VIEW_WIDTH / 2)
		{
			newXCoord += (SPRITE_WIDTH / 2);
		}

		else if (newXCoord > VIEW_WIDTH / 2)
		{
			newXCoord -= (SPRITE_WIDTH / 2);
		}

		if (newYCoord < VIEW_HEIGHT / 2)
		{
			newYCoord += (SPRITE_WIDTH / 2);
		}

		else if (newYCoord > VIEW_HEIGHT / 2)
		{
			newYCoord -= (SPRITE_WIDTH / 2);
		}

		getWorld()->addActor(new Salmonella(newXCoord, newYCoord, getWorld()));
		setFoodEaten(0);
	}

	
	//step 4
	if (getWorld()->foodOverlap(this) == true)  //eat the food if they come in contact
	{
		eatFood();
	}

	
	//step 5
	if (returnMvmtPlanDist() > 0)  //move forward or change direction if movement is blocked
	{
		setMvmtPlan(returnMvmtPlanDist() - 1);

		if (getWorld()->movementBlockage(this))
		{		
			int x = randInt(0, 359);
			setDirection(x);
			setMvmtPlan(10);
		}

		else
		{
			moveForward(3);
		}
		return;
	}
	
	
	//step 6
	if (getWorld()->isFoodNearby(this) == false)  //choosing a new direction to move in if there's no food around
	{
		int x = randInt(0, 359);
		setDirection(x);
		setMvmtPlan(10);
		return;
	}

	else if (getWorld()->isFoodNearby(this) == true)  //try to reach the food if it's nearby
	{
		if (getWorld()->movementBlockage(this) == true)
		{
			int x = randInt(0, 359);
			setDirection(x);
			setMvmtPlan(10);
		}

		else
		{
			moveForward(3);
		}
		return;
	}
 
}




aggressiveSalmonella::aggressiveSalmonella(double startX, double startY, StudentWorld* petriDish)
	:Bacteria(IID_SALMONELLA, startX, startY, 10, petriDish)
{
	setObjectType("aggressiveSalmonella");
}

aggressiveSalmonella::~aggressiveSalmonella()
{
	getWorld()->accessMapActors("aggressiveSalmonella");
}

void aggressiveSalmonella::doSomething()  //details what an aggressive salmonella does
{
	bool flag = false;

	//step 1
	if (deadOrAlive() == false)
	{
		return;
	}

	
	//step 2
	if (getWorld()->closeToSocrates(this) == true)  //try to follow Socrates if he is nearby
	{
		if (!getWorld()->attachedToSocrates(this) && getWorld()->movementBlockage(this) == true)   //MY GUY IS SPINNING 
		{
			int x = randInt(0, 359);
			setDirection(x);
			setMvmtPlan(10);
		}

		else if (getWorld()->movementBlockage(this) == false)
		{
			moveForward(3);
		}
		flag = true;
		
	}
	
	
	//step 3
	if (this->euclideanDistance(getWorld()->returnSocrates()) <= 8)  //hurt Socrates if coming in contact with him
	{
		getWorld()->hurtSocrates(2);
	}

	//step 4
	else if (returnFoodEaten() == 3)  //reproduce if it has eaten three foods
	{
		double newXCoord = getX();
		double newYCoord = getY();

		if (newXCoord < VIEW_WIDTH / 2)
		{
			newXCoord += (SPRITE_WIDTH / 2);
		}

		else if (newXCoord > VIEW_WIDTH / 2)
		{
			newXCoord -= (SPRITE_WIDTH / 2);
		}

		if (newYCoord < VIEW_HEIGHT / 2)
		{
			newYCoord += (SPRITE_WIDTH / 2);
		}

		else if (newYCoord > VIEW_HEIGHT / 2)
		{
			newYCoord -= (SPRITE_WIDTH / 2);
		}

		getWorld()->addActor(new aggressiveSalmonella(newXCoord, newYCoord, getWorld()));
		setFoodEaten(0);
	}

	//step 5
	else if (getWorld()->foodOverlap(this) == true)  //eat the food if it comes in contact with a food
	{
		eatFood();
	}

	if (flag == true)  //if step two was carried out, don't continue on to steps 6 and 7, just return
	{
		return;
	}

	//step 6
	if (returnMvmtPlanDist() > 0)  //change direction if movement is blocked
	{
		setMvmtPlan(returnMvmtPlanDist() - 1);

		if (getWorld()->movementBlockage(this))
		{
			int x = randInt(0, 359);
			setDirection(x);
			setMvmtPlan(10);
		}

		else
		{
			moveForward(3);
		}
		return;
	}

	//step 7
	else if (getWorld()->isFoodNearby(this) == false)  //choosing a new direction to move in if there's no food around
	{
		int x = randInt(0, 359);
		setDirection(x);
		setMvmtPlan(10);
		return;
	}

	else if (getWorld()->isFoodNearby(this) == true)  //try to reach the food if it's nearby
	{
		if (getWorld()->movementBlockage(this) == true)
		{
			int x = randInt(0, 359);
			setDirection(x);
			setMvmtPlan(10);
		}

		else
		{
			moveForward(3);
		}
		return;
	}
	
	
}


Ecoli::Ecoli(double startX, double startY, StudentWorld* petriDish)
	:Bacteria(IID_ECOLI, startX, startY, 5, petriDish)
{
	setObjectType("Ecoli");
}

Ecoli::~Ecoli()
{
	getWorld()->accessMapActors("Ecoli");
}

void Ecoli::doSomething()  //details what an E coli does in a tick
{
	//step 1
	if (deadOrAlive() == false)
	{
		return;
	}
	
	//step 2
	if (this->euclideanDistance(getWorld()->returnSocrates()) <= 8)  //hurt Socrates if they come in contact with each other
	{
		getWorld()->hurtSocrates(4);
	}
	
	//step 3
	else if (returnFoodEaten() == 3)  //reproduce if it has eaten three foods
	{
		double newXCoord = getX();
		double newYCoord = getY();

		if (newXCoord < VIEW_WIDTH / 2)
		{
			newXCoord += (SPRITE_WIDTH / 2);
		}

		else if (newXCoord > VIEW_WIDTH / 2)
		{
			newXCoord -= (SPRITE_WIDTH / 2);
		}

		if (newYCoord < VIEW_HEIGHT / 2)
		{
			newYCoord += (SPRITE_WIDTH / 2);
		}

		else if (newYCoord > VIEW_HEIGHT / 2)
		{
			newYCoord -= (SPRITE_WIDTH / 2);
		}

		getWorld()->addActor(new aggressiveSalmonella(newXCoord, newYCoord, getWorld()));
		setFoodEaten(0);
	}


	//step 4
	else if (getWorld()->foodOverlap(this) == true)  //eat the food if they come in contact with each other
	{
		eatFood();
	}

	
	//step 5
	if (getWorld()->isEcoliCloseToSocrates(this) == true)  //E coli will try to follow Socrates around 
	{
		//get direction/angle towards Socrates up to ten times
		for (int i = 0; i <= 9; i++)
		{
			if (getWorld()->movementBlockage(this) == true)
			{
				setDirection(getDirection() + 10);
			}

			else
			{
				moveForward(2);
				return;
			}
		}
		moveForward(0);
	}	
}