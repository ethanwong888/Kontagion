#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "Actor.h"
#include "GameWorld.h"
#include <string>
#include <vector>
using namespace std;

class Actor;
class Bacteria;
class Socrates;


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	virtual ~StudentWorld();
    int init();
    int move();
    void cleanUp();
	void accessMapActors(string actor);
	const vector <Actor*> & returnVec_Actors();
	Socrates* returnSocrates();
	double findDistance(double x1, double y1, double x2, double y2);
	bool overlap(double x, double y);
	bool sprayDamage(Actor* a1);
	bool flameDamage(Actor* a2);
	void addGoodies();
	void addDirt();
	void addPit();
	void addFood();
	bool otherCollision(Actor* a);
	void hurtSocrates(int damage);
	void addActor(Actor* a);
	bool foodOverlap(Actor* a);
	bool movementBlockage(Actor* a);
	bool isFoodNearby(Actor* a);
	bool closeToSocrates(Actor* a);
	bool isEcoliCloseToSocrates(Actor* a);
	double randomDecimal();

	bool attachedToSocrates(Actor* a);
	int returnMapActors(string n);
	bool wonLevel();

private:
	vector <Actor*> vec_Actors;
	map <string, int> map_Actors;
	Socrates* m_Socrates;
};

#endif // STUDENTWORLD_H_
