#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <cmath>
#include <random>
#include <stdio.h> 
#include <stdlib.h> 
#include<time.h> 

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath)
{
	//map of actors??? stores the number of each actor (
}

int StudentWorld::init()    //initializes the dirt, pit, food, and Socrates character
{
	map_Actors["Dirt"] = 0;
	map_Actors["Pit"] = 0;
	map_Actors["Food"] = 0;
	map_Actors["Salmonella"] = 0;
	map_Actors["aggressiveSalmonella"] = 0;
	map_Actors["Fungi"] = 0;
	map_Actors["HealthPack"] = 0;
	map_Actors["FlamePack"] = 0;
	map_Actors["ExtraLifePack"] = 0;

	m_Socrates = new Socrates(this);
	addDirt();
	addPit();
	addFood();
	//addBacteria(new Salmonella(128, 128, this));
	//addBacteria(new aggressiveSalmonella(120, 120, this));
	//addActor(new Ecoli(120, 120, this));

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	// This code is here merely to allow the game to build, run, and terminate after you hit enter.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.

	addGoodies();   //used to place Goodies in the game
	vector<Actor*>::iterator it = vec_Actors.begin();
	for (it = vec_Actors.begin(); it != vec_Actors.end();)  //remove dead actors
	{
		if ((*it)->deadOrAlive() == false)
		{
			if ((*it)->isBacteria())
			{
				int x = randInt(1, 2);
				if (x == 2)
				{
					addActor(new Food((*it)->getX(), (*it)->getY(), this));
				}
			}
			delete* it;
			it = vec_Actors.erase(it);
		}
		else
		{
			it++;
		}
	}

	for (it = vec_Actors.begin(); it != vec_Actors.end();)  //checking for collisions
	{
		if ((*it)->returnObjectType() == "Dirt" && it != vec_Actors.end())   //mitigate lag by not checking collision for objects that don't need to check
		{
			it++;
		}

		if (otherCollision((*it)) == true)
		{
			(*it)->setDead();
		}
		it++;
	}

	m_Socrates->doSomething();   //allow Socrates to do something 

	if (m_Socrates->getHP() <= 0)   //end level if Socrates dies
	{
		m_Socrates->setDead();
		decLives();

		return GWSTATUS_PLAYER_DIED;
	}

	for (it = vec_Actors.begin(); it != vec_Actors.end(); it++)  //allows actors to do something
	{
		if ((*it)->deadOrAlive())
		{
			(*it)->doSomething();
		}

	}


	if (wonLevel())  //finished level if all the bacteria have been killed 
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

	ostringstream oss;     //displaying the header at the top of the game screen
	oss.fill('0');
	oss.setf(ios::fixed);
	oss.precision(0);
	if (getScore() >= 0)
	{
		oss << "Score: " << setw(6) << getScore() << "  ";
	}

	else
	{
		oss << "Score: -" << setw(5) << (getScore() * -1) << "  ";
	}
	oss << "Level: " << getLevel() << "  ";
	oss << "Lives: " << getLives() << "  ";
	oss << "Health: " << setw(2) << m_Socrates->getHP() << "  ";
	oss << "Sprays: " << setw(2) << m_Socrates->getSprayAmount() << "  ";
	oss << "Flames: " << m_Socrates->getFlameAmount();

	setGameStatText(oss.str());


	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()    //deletes all the actors, sets m_socrates to point to nullptr
{
	vector<Actor*>::iterator it;   
	for (it = vec_Actors.begin(); it < vec_Actors.end(); )
	{
		delete *it;
		it = vec_Actors.erase(it);
	}

	if (m_Socrates != nullptr)
	{
		delete m_Socrates;
		m_Socrates = nullptr;
	}
}


StudentWorld::~StudentWorld()
{
	cleanUp();
}


void StudentWorld::accessMapActors(string actor)    //decrement count of a certain type of actor
{
	map_Actors[actor] -= 1;
}

const vector <Actor*> & StudentWorld::returnVec_Actors()
{
	return vec_Actors;
}

Socrates* StudentWorld::returnSocrates()
{
	return m_Socrates;
}

double StudentWorld::findDistance(double x1, double y1, double x2, double y2)   //find distance between actors based on their coordinates
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void StudentWorld::addGoodies()   //adds Goodies randomly into the game
{
	int chanceFungus = min(510 - getLevel() * 10, 200);
	int randomFungus = randInt(0, chanceFungus);
	if (randomFungus == 0)
	{
		//create a new fungus
		Fungi* newFungi = new Fungi(this);
		addActor(newFungi);
	}

	int chanceGoodie = min(510 - getLevel() * 10, 250);
	int randomGoodie = randInt(0, chanceGoodie);
	if (randomGoodie == 0)
	{
		int random = randInt(1, 10);

		if (random <= 6)
		{
			//create a new restore health goodie
			HealthPack* newHealthPack = new HealthPack(this);
			addActor(newHealthPack);
		}

		else if (random <= 9 && random > 6)
		{
			//create a new flame thrower goodie
			FlamePack* newFlamePack = new FlamePack(this);
			addActor(newFlamePack);
		}

		else
		{
			//create a new extra life goodie
			ExtraLifePack* newExtraLifePack = new ExtraLifePack(this);
			addActor(newExtraLifePack);
		}
	}
}

bool StudentWorld::sprayDamage(Actor* a1)   //deals with how spray damages different types of actors in the game
{
	vector<Actor*>::iterator it = vec_Actors.begin();
	while (it != vec_Actors.end())
	{
		//instantly destroy goodies or dirt on contact
		if ((*it)->isItDamageable() && (findDistance((*it)->getX(), (*it)->getY(), (a1)->getX(), (a1)->getY()) <= 2.0 * SPRITE_RADIUS)
			&& ((*it)->isDirty() || (*it)->isGoodie()))
		{
			(*it)->setDead();
			return true;
		}

		//damages salmonella and aggressive salmonella, kills them if health points are depleted
		else if ((*it)->isItDamageable() && (findDistance((*it)->getX(), (*it)->getY(), (a1)->getX(), (a1)->getY()) <= 2.0 * SPRITE_RADIUS)
			&& ((*it)->returnObjectType() == "aggressiveSalmonella" || (*it)->returnObjectType() == "Salmonella"))
			{
			(*it)->takeDamage(2);

			if ((*it)->getHP() <= 0)
			{
				(*it)->setDead();
				playSound(SOUND_SALMONELLA_DIE);
				increaseScore(100);
			}
			playSound(SOUND_SALMONELLA_HURT);
			return true;
			
		}

		//damages Ecoli, kills them if health points are depleted
		else if ((*it)->isItDamageable() && findDistance((*it)->getX(), (*it)->getY(), (a1)->getX(), (a1)->getY()) <= 2.0 * SPRITE_RADIUS
			&& (*it)->returnObjectType() == "Ecoli")
		{
			(*it)->takeDamage(5);

			if ((*it)->getHP() <= 0)
			{
				(*it)->setDead();
				playSound(SOUND_ECOLI_DIE);
				increaseScore(100);

			}
			playSound(SOUND_ECOLI_HURT);
			return true;
		}

		else
		{
			it++;
		}
	}
	return false;
}




bool StudentWorld::flameDamage(Actor* a1)
{
	vector<Actor*>::iterator it = vec_Actors.begin();
	while (it != vec_Actors.end())
	{
		//instantly destroys goodies and dirt objects upon impact
		if ((*it)->isItDamageable() && (findDistance((*it)->getX(), (*it)->getY(), (a1)->getX(), (a1)->getY()) <= 2.0 * SPRITE_RADIUS)
			&& ((*it)->isDirty() || (*it)->isGoodie()))
		{
			(*it)->setDead();
			return true;
		}

		//damages salmonella and aggressive salmonella, kills them if health points are depleted
		else if ((*it)->isItDamageable() && (findDistance((*it)->getX(), (*it)->getY(), (a1)->getX(), (a1)->getY()) <= 2.0 * SPRITE_RADIUS
			&& ((*it)->returnObjectType() == "aggressiveSalmonella" || (*it)->returnObjectType() == "Salmonella")))
		{
			(*it)->takeDamage(5);

			if ((*it)->getHP() <= 0)
			{
				(*it)->setDead();
				playSound(SOUND_SALMONELLA_DIE);
				increaseScore(100);

			}
			playSound(SOUND_SALMONELLA_HURT);
			return true;

		}

		//damages Ecoli, kills them if health points are depleted
		else if ((*it)->isItDamageable() && findDistance((*it)->getX(), (*it)->getY(), (a1)->getX(), (a1)->getY()) <= 2.0 * SPRITE_RADIUS
			&& (*it)->returnObjectType() == "Ecoli")
		{
			(*it)->takeDamage(5);

			if ((*it)->getHP() <= 0)
			{
				(*it)->setDead();
				playSound(SOUND_ECOLI_DIE);
				increaseScore(100);

			}
			playSound(SOUND_ECOLI_HURT);

			return true;
		}

		else
		{
			it++;
		}
	}
	return false;
}



bool StudentWorld::overlap(double x, double y)  //checks whether or not two actors are overlapping with each other
{
	vector<Actor*>::iterator it;
	for (it = vec_Actors.begin(); it != vec_Actors.end(); it++)
	{
		if (findDistance((*it)->getX(), (*it)->getY(), x, y) <= SPRITE_WIDTH)
			return true;
	}
	return false;
}

double StudentWorld::randomDecimal()  //generates a random decimal between 0 and 1
{
	std::random_device rd;
	std::default_random_engine generator(rd()); // rd() provides a random seed
	std::uniform_real_distribution<double> distribution(0.00000000001, 0.99999999);
	double number = distribution(generator);
	return number;
}



void StudentWorld::addDirt()   //add Dirt randomly in the level
{
	for (int i = 0; i < max(180 - 20 * getLevel(), 20); i++)   //adding in the piles of dirt
	{
		double r = 120 * sqrt(randomDecimal());         //random radius
		double theta = (randomDecimal()) * 2 * 3.1415;  //random angle
		
		double xBound = r * cos(theta) + 128;   //setting the bounds for x
		double yBound = r * sin(theta) + 128;   //setting the bounds for y

		addActor(new Dirt(xBound, yBound, this));
	}
}

//add pit(s) randomly in the level
void StudentWorld::addPit()
{
	for (int i = 0; i < getLevel(); i++)   //adding in the pit(s)
	{
		double r = randInt(0, 120);       //random radius
		double theta = randInt(0, 360);   //random angle (in degrees)

		double xBound = r * cos(theta) + 128;   //setting the bounds for x
		double yBound = r * sin(theta) + 128;   //setting the bounds for y

		while (overlap(xBound, yBound) == true)   //while it's overlapping, create new coordinates
		{
			r = randInt(0, 120);       //random radius
			theta = randInt(0, 360);   //random angle (in degrees)

			xBound = r * cos(theta) + 128;   //setting the bounds for x
			yBound = r * sin(theta) + 128;   //setting the bounds for y
		}

		addActor(new Pit(xBound, yBound, this));	
	}
}

//add food randomly in the level
void StudentWorld::addFood()
{
	for (int i = 0; i < min(5 * getLevel(), 25); i++)   //adding in the food(s)
	{
		double r = randInt(0, 120);       //random radius
		double theta = randInt(0, 360);   //random angle (in degrees)

		double xBound = r * cos(theta) + 128;   //setting the bounds for x
		double yBound = r * sin(theta) + 128;   //setting the bounds for y

		while (overlap(xBound, yBound) == true)   //while it's overlapping, create new coordinates
		{
			r = randInt(0, 120);       //random radius
			theta = randInt(0, 360);   //random angle (in degrees)

			xBound = r * cos(theta) + 128;   //setting the bounds for x
			yBound = r * sin(theta) + 128;   //setting the bounds for y
		}
		addActor(new Food(xBound, yBound, this));
	}
}


int StudentWorld::returnMapActors(string n)  //returns how many of a certain type of actor there are currently in the game
{
	return map_Actors[n];
}


bool StudentWorld::otherCollision(Actor* a)     //checking for collision throughout the entire vector of actors pointers
{
	vector<Actor*>::const_iterator it = returnVec_Actors().begin();
	while (it != returnVec_Actors().end())
	{
		if ((((*it)->deadOrAlive() == true && a->deadOrAlive() == true) &&
			!((*it)->returnObjectType() == "Dirt" && a->returnObjectType() == "Dirt")) &&	
			(((*it)->returnObjectType() == "Dirt" && a->returnObjectType() == "Flame") ||
			((*it)->returnObjectType() == "Dirt" && a->returnObjectType() == "Spray")) &&
				(*it)->isItDamageable() == true && a->isItDamageable() == false)

		{
			if (a->euclideanDistance(*it) <= 8)
			{
				(*it)->setDead();
				return true;
			}
		}
		it++;
	}
	return false;
}

void StudentWorld::hurtSocrates(int damage)  //reduces Socrate's health points by specified amount
{
	m_Socrates->takeDamage(damage);
	playSound(SOUND_PLAYER_HURT);

	if (m_Socrates->getHP() <= 0)
	{
		m_Socrates->setDead();
	}
}

void StudentWorld::addActor(Actor* a)  //adds an actor into the game
{
	vec_Actors.push_back(a);
	string name = a->returnObjectType();
	map_Actors[name]++;
}



bool StudentWorld::foodOverlap(Actor* a)  //checks for overlaps with food objects
{
	vector<Actor*>::iterator it = vec_Actors.begin();
	for (it = vec_Actors.begin(); it != vec_Actors.end(); it++)  //checking for collisions
	{
		if ((findDistance(a->getX(), a->getY(), (*it)->getX(), (*it)->getY()) <= 2 * SPRITE_RADIUS) && (*it)->isEdible())
		{
			(*it)->setDead();
			return true;
		}
	}
	return false;
}


bool StudentWorld::movementBlockage(Actor* a)  //check if movement is blocked by either dirt or circle
{
	//is movement blocked by dirt?
		double xCoord;
		double yCoord;
		a->getPositionInThisDirection(a->getDirection(), 3, xCoord, yCoord);

		vector<Actor*>::iterator it;
		for (it = vec_Actors.begin(); it != vec_Actors.end(); it++)
		{
			if ((*it)->isDirty() && findDistance((*it)->getX(), (*it)->getY(), xCoord, yCoord) <= SPRITE_WIDTH / 2)
			{
				return true;	
			}		
		}

	//does movement go outside of the circle?
	for (int i = 1; i <= 3; i++)  
	{
		double xCoord;
		double yCoord;
		a->getPositionInThisDirection(a->getDirection(), i, xCoord, yCoord);
		if (findDistance(xCoord, yCoord, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) >= VIEW_RADIUS)
		{
			return true;
		}
	}
	return false;
}


bool StudentWorld::isFoodNearby(Actor* a)  //returns true if there is food nearby a bacteria, false otherwise
{
	vector<Actor*>::iterator it;
	for (it = vec_Actors.begin(); it != vec_Actors.end(); it++)
	{
		if (findDistance((*it)->getX(), (*it)->getY(), a->getX(), a->getY()) <= 128 && (*it)->isEdible())
		{
			double p = (180/3.1415) * atan2((*it)->getY() - a->getY(), (*it)->getX() - a->getX());
			a->setDirection(p);
			return true;
		}
	}
	return false;
}

bool StudentWorld::closeToSocrates(Actor* a)  //returns true if bacteria is close to socrates, false otherwise
{
	if (findDistance(a->getX(), a->getY(), returnSocrates()->getX(), returnSocrates()->getY()) <= 72)
	{
		double p = (180 / 3.1415) * atan2(returnSocrates()->getY() - a->getY(), returnSocrates()->getX() - a->getX());
		a->setDirection(p);
		return true;
	}
	return false;
}


bool StudentWorld::isEcoliCloseToSocrates(Actor* a)  //returns true if Ecoli is close to Socrates; false otherwise
{
	if (findDistance(a->getX(), a->getY(), returnSocrates()->getX(), returnSocrates()->getY()) <= 256)
	{
		double p = (180 / 3.1415) * atan2(returnSocrates()->getY() - a->getY(), returnSocrates()->getX() - a->getX());
		a->setDirection(p);
		return true;
	}
	return false;
}


bool StudentWorld::attachedToSocrates(Actor* a)   //returns true if a bacteria gets stuck on Socrates (aggressive or Ecoli)
{
	if (findDistance(a->getX(), a->getY(), returnSocrates()->getX(), returnSocrates()->getY() <= SPRITE_WIDTH))
	{
		return true;
	}
	return false;
}

bool StudentWorld::wonLevel()  //returns true if user has defeated all the bacteria, false otherwise
{
	if (returnMapActors("Pit") == 0 && returnMapActors("Salmonella") == 0 &&
		returnMapActors("aggressiveSalmonella") == 0 && returnMapActors("Ecoli") == 0)
	{
		return true;
	}
	return false;
}



