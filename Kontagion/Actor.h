#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
#include "StudentWorld.h"
#include <string>
class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int ImageID, double startX, double startY, int startDirection, int depth, StudentWorld* petriDish, double HP, bool damageable);
	virtual ~Actor();
	bool deadOrAlive();
	virtual void doSomething() = 0;
	void setDead();
	StudentWorld* getWorld();
	void setObjectType(std::string s);
	std::string returnObjectType();
	bool isItDamageable() const;
	void takeDamage(int damage);
	double getHP();
	void setHP(double hp);
	virtual bool isEdible();
	virtual bool isDirty();
	double euclideanDistance(const Actor* actor1);  //get the distance between two Actors, use distance in overlap function
	virtual bool isBacteria();
	virtual bool isGoodie();

private:
	bool m_alive;
	StudentWorld* m_world;
	int m_ImageID;
	std::string type;
	bool m_damageable;
	double m_health;
};

//===============================================================================================//

class Socrates : public Actor
{
public:
	Socrates(StudentWorld* petriDish);
	virtual ~Socrates();
	virtual void doSomething();
	virtual void addHP(int amount);
	int getSprayAmount();
	void shootSpray();
	int getFlameAmount();
	void shootFlame();
	void increaseFlameAmmo();

private:
	int m_sprayAmmo;
	int m_flameAmmo;
	int m_lives;
	double socratesPositionalAngle;
	
};


//===============================================================================================//
class Projectile : public Actor
{
public:
	Projectile(int ImageID, double startX, double startY, int direction, int depth, StudentWorld* petriDish);
	virtual ~Projectile();
	virtual void doSomething() = 0;
	virtual bool distanceTraveled(double a, double b) = 0;
private:


};

class Flame : public Projectile
{
public:
	Flame(double startX, double startY, int direction, StudentWorld* petriDish);
	virtual ~Flame();
	virtual void doSomething();
	virtual bool distanceTraveled(double a, double b);

private:
	int flameMoveDistance;
	double startingX;
	double startingY;
};

class Spray : public Projectile
{
public:
	Spray(double startX, double startY, int direction, StudentWorld* petriDish);
	virtual ~Spray();
	virtual void doSomething();
	virtual bool distanceTraveled(double a, double b);
	//bool collision();

private:
	int sprayMoveDistance;
	double startingX;
	double startingY;
};


//===============================================================================================//


class Goodies : public Actor
{
public:
	Goodies(int ImageID, double angle, StudentWorld* petriDish);
	virtual ~Goodies();
	virtual void function() = 0;
	virtual void doSomething();
	virtual bool isGoodie();

private:
	int livingTime;
	int lifetime;
};

class Fungi : public Goodies
{
public:
	Fungi(StudentWorld* petriDish);
	virtual ~Fungi();
	virtual void function();   //this subtracts 50 points; reduces Socrates's health by 20 
private:

};

class HealthPack : public Goodies
{
public:
	HealthPack(StudentWorld* petriDish);
	virtual ~HealthPack();
	virtual void function();    //this adds 250 points; restores Socrates's health

private:

};

class ExtraLifePack : public Goodies
{
public:
	ExtraLifePack(StudentWorld* petriDish);
	virtual ~ExtraLifePack();
	virtual void function();     //this adds 500 points; gives Socrates an extra life
private:

};

class FlamePack : public Goodies
{
public:
	FlamePack(StudentWorld* petriDish);
	virtual ~FlamePack();
	virtual void function();   //adds five additional flame charges
private:

};


//===============================================================================================//


class Dirt : public Actor
{
public:
	Dirt(double startX, double startY, StudentWorld* petriDish);
	virtual void doSomething();
	virtual ~Dirt();
	virtual bool isDirty();

private:
};

class Food : public Actor
{
public: 
	Food(double startX, double startY, StudentWorld* petriDish);
	virtual void doSomething();
	virtual ~Food();
	bool isEdible();

private:
	bool damageable;
};

class Pit : public Actor
{
public:
	Pit(double startX, double startY, StudentWorld* petriDish);
	virtual void doSomething();
	virtual ~Pit();
	bool isDepleted();
	int returnSalmonella();
	int returnAggressive();
	int returnEcoli();

private:
	int m_salmonella;
	int m_aggressiveSalmonella;
	int m_Ecoli;
};

//Bacteria =============================================================================================================
class Bacteria : public Actor
{
public:
	Bacteria(int ImageID, double startX, double startY, int hp, StudentWorld* petriDish);
	virtual ~Bacteria();
	virtual void doSomething() = 0;
	void eatFood();
	int returnFoodEaten();
	int returnMvmtPlanDist();
	void setMvmtPlan(int x);
	void setFoodEaten(int amount);
	bool isBacteria();


private:
	int m_foodEaten;
	//int m_hp;
	int mvmtPlan;
};



class Salmonella : public Bacteria
{
public:
	Salmonella(double startX, double startY,  StudentWorld* petriDish);
	virtual ~Salmonella();
	virtual void doSomething();

private:

};

class aggressiveSalmonella : public Bacteria
{
public:
	aggressiveSalmonella(double startX, double startY, StudentWorld* petriDish);
	virtual ~aggressiveSalmonella();
	virtual void doSomething();
private:

};

class Ecoli : public Bacteria
{
public:
	Ecoli(double startX, double startY, StudentWorld* petriDish);
	virtual ~Ecoli();
	virtual void doSomething();
private:

};

#endif // ACTOR_H_
