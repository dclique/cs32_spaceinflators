 #include "actor.h"
#include "StudentWorld.h"
#include "GameController.h"
#include <cstdlib>

// Students:  Add code to this file (if you wish), actor.h, StudentWorld.h, and StudentWorld.cpp


//ACTOR///////////////////////////////////////

Actor::Actor(int imageID, int startX, int startY, StudentWorld* object)
:GraphObject(imageID, startX, startY), m_dead(true){
    m_world = object;
    setVisible(true);
}

Actor::~Actor(){
}

void Actor::doSomething(){
}

StudentWorld* Actor::getWorld(){
    return m_world;
}

bool Actor::isDead(){
    return m_dead; 
}

void Actor::setDead(){
    m_dead = false;
}

Star::Star(StudentWorld* object)
:Actor(IID_STAR, (rand() % VIEW_WIDTH), VIEW_HEIGHT-1, object)
{}

//STAR////////////////////////////////////////////

Star::~Star()
{}

void Star::doSomething()
{
    moveTo(getX(), getY()-1);
    if(getY() < 0)
        setDead;
}


//SHIP////////////////////////////////////////////

Ship::Ship(StudentWorld* object)
:Actor(IID_PLAYER_SHIP, VIEW_WIDTH/2, 1, object), m_hitpoints(50)
{
    m_fired = false;
    m_numberOfTorpedoes = 0;
}

Ship::~Ship()
{
    if(m_hitpoints < 0)
    {
        delete this;
    }
}

int Ship::currentHP()
{
    return m_hitpoints;
}

void Ship::addTorpedoes(int k)
{
    m_numberOfTorpedoes += k;
}

int Ship::shipEnergy()
{
    int percentageHP;
    return percentageHP = (m_hitpoints*2);
}

bool Ship::fired()
{
    m_fired = !m_fired;
    return m_fired;
}

int Ship::numberOfTorpedoes()
{
    return m_numberOfTorpedoes;
}

void Ship::doSomething()
{
    list <Actor*>::iterator it;

	//check if player collided with an enemy
    for(it = getWorld()->getList().begin(); it != getWorld()->getList().end(); it++)
    {
        Actor* iterptr = *it;
        if(iterptr != NULL)
        {
            Alien* temp = dynamic_cast<Alien*>(*it);
            if(temp != NULL)
            {
                if(temp->getX() == getX() && temp->getY() == getY())
                {
                    damage(15);
                    temp->setDead();
                }
            }
        }
    }
        int keypress;
        if(getWorld()->getKey(keypress))
        {
            switch(keypress)
            {
                case KEY_PRESS_LEFT:
                    if(getX() > 0)
                        moveTo(getX()-1,getY());
                    break;
                case KEY_PRESS_RIGHT:
                    if(getX() < VIEW_WIDTH-1)
                        moveTo(getX()+1,getY());
                    break;
                case KEY_PRESS_UP:
                    if(getY() < VIEW_HEIGHT-1)
                        moveTo(getX(),getY()+1);
                    break;
                case KEY_PRESS_DOWN:
                    if(getY() > 0)
                        moveTo(getX(),getY()-1);
                    break;
                case KEY_PRESS_SPACE:
                    if(fired())
                    {
                        getWorld()->getList().push_front(new Bullet(getWorld(), getX(), getY()+1, false));
                        Game().playSound(SOUND_PLAYER_FIRE);
                    }
                    break;
                case KEY_PRESS_TAB:
                    if(fired() && (numberOfTorpedoes() > 0))
                    {
                        getWorld()->getList().push_front(new Torpedo(getWorld(), getX(), getY()+1, false));
                        Game().playSound(SOUND_PLAYER_TORPEDO);
                    }
                    break; 
                default:
                    fired();
                    
                    list <Actor*>::iterator iter;
                    for(iter = getWorld()->getList().begin(); iter != getWorld()->getList().end(); iter++)
                    {
                        Actor* iterptr = *iter;
                        if(iterptr != NULL)
                        {
                            Alien* tempalienptr = dynamic_cast<Alien*>(*iter);
                            if(tempalienptr != NULL)
                            {
                                if(tempalienptr->getX() == getX() && tempalienptr->getY() == getY())
                                {
                                    Game().playSound(SOUND_ENEMY_PLAYER_COLLISION);
                                    damage(15);
                                    tempalienptr->setDead();
                                }
                            }
                        }
                    }
            }
    }
    list <Actor*>::iterator it;
    for(it = getWorld()->getList().begin(); it != getWorld()->getList().end(); it++)
    {
        Actor* projectileptr = *it;
        if(projectileptr != NULL)
        {
            Projectile* projectileptr = dynamic_cast<Projectile*>(*it);
            if(projectileptr != NULL)
            {
                if(projectileptr->damagePoint() == 2)
                {
                    damage(2);
                }
                if(projectileptr->damagePoint() == 8)
                {
                    damage(8);
                }
            }
        }
    }
}

int Ship::damage(int k)
{
    if(k == 2)
    {
        Game().playSound(SOUND_PLAYER_HIT);
    }
    m_hitpoints -= k;
    if(shipEnergy() <= 0)
    {
        Game().playSound(SOUND_PLAYER_DIE);
        delete this;
        return GWSTATUS_PLAYER_DIED;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void Ship::heal()
{
    m_hitpoints = 50;
}

//ALIEN/////////////////////////////////////////

Alien::Alien(int imageID, int startX, int startY, StudentWorld* object, int health)
:Actor(imageID, (rand() % VIEW_WIDTH), (VIEW_HEIGHT-1), object)
{
    m_health = health;
}

Alien::~Alien()
{
}

int Alien::getHealth()
{
    return m_health;
}

void Alien::damage(int k)
{
    m_health -= k;
}


//SMALLBOT/////////////////////////////////////

Smallbots::Smallbots(StudentWorld* object, int HP)
:Alien(IID_SMALLBOT, (rand() % VIEW_WIDTH), (VIEW_HEIGHT-1), object, HP)
{
    m_sEveryOther = false;
    m_justAttacked = false;
}

Smallbots::~Smallbots()
{
}

bool Smallbots::justAttacked()
{
    list <Actor*>::iterator iter;
    for(iter = getWorld()->getList().begin(); iter != getWorld()->getList().end(); iter++)
    {
        Actor* iterptr = *iter;
        if(iterptr != NULL)
        {
            Projectile* projectileptr = dynamic_cast<Projectile*>(*iter);
            if(projectileptr != NULL)
            {
                if(projectileptr->getX() == getX() && projectileptr->getY() == getY() && projectileptr->getPlayerFired() == false)
                {
                    m_justAttacked = true;
                }
            }
        }
    }
    return m_justAttacked;
}

void Smallbots::doSomething()
{
    if(!m_sEveryOther)
    {
        if(getX() == 29)
        {
            moveTo(getX()-1, getY());
        }
        if(getX() == 0)
        {
            moveTo(getX()+1, getY());
        }
        if(!justAttacked())
        {
            if(((rand() % 3) == 1))
            {
                if(getX() == 0)
                {
                    moveTo(getX()+1, getY()-1);
                }
                else if(getX() == 29)
                {
                    moveTo(getX()-1, getY()-1);
                }
                else
                {
                    if(rand() % 2 == 1)
                    {
                        moveTo(getX()-1, getY()-1);
                    }
                    else
                    {
                        moveTo(getX()+1, getY()-1);
                    }
                }
            }
        }
        else
        {
            moveTo(getX(), getY()-1);
        }
        if(getX() == getWorld()->getShipPtr()->getX() && (getY() > getWorld()->getShipPtr()->getY()))
        {
            int q;
            q = (100.0/(getWorld()->roundNumber())+1);
            if((rand() % q) == 1)
            {
                if(getWorld()->getNumAlienFiredProjectiles() < (2*(getWorld()->roundNumber())))
                {
                    getWorld()->getList().push_front(new Torpedo(getWorld(), getX(), (getY()-1), true));
                    getWorld()->addprojectile();
                    Game().playSound(SOUND_ENEMY_FIRE);
                }
            }
            else
            {
                if(getWorld()->getNumAlienFiredProjectiles() < (2*(getWorld()->roundNumber())))
                {
                    
                    getWorld()->getList().push_front(new Bullet(getWorld(), getX(), (getY()-1), true));
                    getWorld()->addprojectile();
                    Game().playSound(SOUND_ENEMY_FIRE);
                }
            }
        }
    }
    if(getX() == getWorld()->getShipPtr()->getX() && getY() == getWorld()->getShipPtr()->getY())
    {
        Game().playSound(SOUND_ENEMY_DIE);
        getWorld()->getShipPtr()->damage(15);
        setDead();
        getWorld()->decreaseRound();
    }
    
    list <Actor*>::iterator iter;
    for(iter = getWorld()->getList().begin(); iter != getWorld()->getList().end(); iter++)
    {
        Actor* iterptr = *iter;
        if(iterptr != NULL)
        {
            Projectile* projectileptr = dynamic_cast<Projectile*>(*iter);
            if(projectileptr != NULL)
            {
                if(projectileptr->getX() == getX() && projectileptr->getY() == getY() && projectileptr->getPlayerFired() == false)
                {
                    damage(projectileptr->damagePoint());
                    if(getHealth() <= 0)
                    {
                        getWorld()->increaseScore(1500);
                        Game().playSound(SOUND_ENEMY_DIE);
                        setDead();
                        getWorld()->decreaseRound();
                        if((rand() % 3) == 1)
                        {
                            getWorld()->getList().push_front(new FreeShipGoodie(getWorld(), getX(), getY()));
                        }
                    }
                    else
                        Game().playSound(SOUND_ENEMY_HIT);
                }
            }
        }
    }
    m_sEveryOther = !m_sEveryOther;
}


//NACHLING//////////////////////////////////

Nachling::Nachling(StudentWorld* object, int HP)
:Alien(IID_NACHLING, (rand() % VIEW_WIDTH), (VIEW_HEIGHT-1), object, HP)
{
    everyOther = false;
}

Nachling::~Nachling()
{
}

int MDB, HMD, HMR;
void Nachling::doSomething()
{
    int state = 0;
    int BASE_CHANCE_OF_FIRING = 10;
    string HMDir;
    int leftside = getX();
    int rightside = ((VIEW_WIDTH-1)-getX());
    if(!everyOther)
    {
        if(getX() == 29)
        {
            moveTo(getX()-1, getY());
        }
        if(getX() == 0)
        {
            moveTo(getX()+1, getY());
        }
        
        if(state == 0)
        {
            if(getX() != getWorld()->getShipPtr()->getX())
            {
                if((rand() % 3) < 2)
                {
                    moveTo(getX(), getY()-1);
                }
                else
                {
                    moveTo(getX()-1, getY()-1);
                }
            }
        }
        else if((getX() == getWorld()->getShipPtr()->getX()) && (getX() != 0 || getX() != (VIEW_WIDTH-1)))
        {
            state = 1;
            if(leftside < rightside)
            {
                MDB = leftside;
            }
            else
            {
                MDB = rightside;
            }
            if(MDB > 3)
            {
                HMD = (rand() % 3);
            }
            else
            {
                HMD = MDB;
            }
            if((rand() % 2) == 1)
            {
                HMDir = "left";
            }
            else
            {
                HMDir = "right";
            }
            //choose random horizontal direction
            HMR = HMD;
            moveTo(getX(), getY()-1);
            if(getY() < 0)
            {
                setDead();
                getWorld()->decreaseRound();
            }
        }
        else if(state == 1)
        {
            if(getWorld()->getShipPtr()->getY() <= getY())
            {
                if(HMR == 0)
                {
                    if(HMDir == "left")
                    {
                        HMDir = "right";
                    }
                    if(HMDir == "right")
                    {
                        HMDir = "left";
                    }
                    HMR = (2*HMD);
                    if(HMDir == "left")
                    {
                        moveTo(getX()-1, getY());
                    }
                    if(HMDir == "right")
                    {
                        moveTo(getX()+1, getY());
                    }
                    int chanceOfFiring = (BASE_CHANCE_OF_FIRING/(getWorld()->roundNumber()))+1;
                    int randomFiring = (rand() % chanceOfFiring);
                    if(randomFiring == 0)
                    {
                        if(getWorld()->getNumAlienFiredProjectiles() < (2*(getWorld()->roundNumber())))
                        {
                            getWorld()->getList().push_front(new Bullet(getWorld(), getX(), (getY()-1), true));
                            getWorld()->addprojectile();
                            Game().playSound(SOUND_ENEMY_FIRE);
                        }
                    }
                }
                else
                {
                    HMR--;
                }
                if((rand() % 20) == 1)
                {
                    state = 2;
                }
            }
            state = 2;
        }
        else if(state == 2)
        {
            if(getY() == 39)
            {
                state = 0;
            }
            else
            {
                if(getX() == 0)
                {
                    HMDir = "right";
                    moveTo(getX()+1, getY()+1);
                }
                else if(getX() == 29)
                {
                    HMDir = "left";
                    moveTo(getX()-1, getY()+1);
                }
                else
                {
                    if((rand() % 2) == 1)
                    {
                        HMDir = "left";
                    }
                    else
                    {
                        HMDir = "right";
                    }
                    if(HMDir == "right")
                    {
                        moveTo(getX()+1, getY());
                    }
                    if(HMDir == "left")
                    {
                        moveTo(getX()-1, getY());
                    }
                }
                if(HMDir == "right")
                {
                    moveTo(getX()+1, getY());
                }
                if(HMDir == "left")
                {
                    moveTo(getX()-1, getY());
                }
            }
        }
    }
    else
    {
        return;
    }
    if(getX() == getWorld()->getShipPtr()->getX() && getY() == getWorld()->getShipPtr()->getY())
    {
        getWorld()->getShipPtr()->damage(15);
        setDead();
        getWorld()->decreaseRound();
    }
    
    list <Actor*>::iterator iter;
    for(iter = getWorld()->getList().begin(); iter != getWorld()->getList().end(); iter++)
    {
        Actor* iterptr = *iter;
        if(iterptr != NULL)
        {
            Projectile* projectileptr = dynamic_cast<Projectile*>(*iter);
            if(projectileptr != NULL)
            {
                if(projectileptr->getX() == getX() && projectileptr->getY() == getY() && projectileptr->getPlayerFired() == false)
                {
                    damage(projectileptr->damagePoint());
                    if(getHealth() <= 0)
                    {
                        getWorld()->increaseScore(1000);
                        Game().playSound(SOUND_ENEMY_DIE);
                        setDead();
                        getWorld()->decreaseRound();
                    }
                    else
                        Game().playSound(SOUND_ENEMY_HIT);
                }
            }
        }
    }
}

//WEALTHYNACHLING////////////////////////////////////

WealthyNachling::WealthyNachling(StudentWorld* object, int HP)
:Nachling(object, HP)
{
    m_state = 0;
    m_mal = false;
    m_stillmal = false;
    m_malCount = 0;
}

WealthyNachling::~WealthyNachling()
{
}

void WealthyNachling::doSomething()
{
    if(m_malCount != 30)       
    {
        if(getX() == 29)
        {
            moveTo(getX()-1, getY());
        }
        if(getX() == 0)
        {
            moveTo(getX()+1, getY());
        }
        if(!m_mal)
        {
            if(rand() % 200 == 1)
            {
                m_mal = true;
            }
            else
            {
                Nachling::doSomething();
            }
        }
        else
        {
            return;
        }
        m_malCount++;
    }
    else
    {
        m_malCount = 0;
        m_mal = false;
    }
    if(getX() == getWorld()->getShipPtr()->getX() && getY() == getWorld()->getShipPtr()->getY())
    {
        getWorld()->getShipPtr()->damage(15);
        setDead();
        getWorld()->decreaseRound();
    }
    list <Actor*>::iterator iter;
    for(iter = getWorld()->getList().begin(); iter != getWorld()->getList().end(); iter++)
    {
        Actor* iterptr = *iter;
        if(iterptr != NULL)
        {
            Projectile* projectileptr = dynamic_cast<Projectile*>(*iter);
            if(projectileptr != NULL)
            {
                if(projectileptr->getX() == getX() && projectileptr->getY() == getY() && projectileptr->getPlayerFired() == false)
                {
                    damage(projectileptr->damagePoint());
                    if(getHealth() <= 0)
                    {
                        getWorld()->increaseScore(1200);
                        Game().playSound(SOUND_ENEMY_DIE);
                        if((rand() % 2) == 1)
                        {
                            getWorld()->getList().push_front(new EnergyGoodie(getWorld(), getX(), getY()));
                        }
                        else
                            getWorld()->getList().push_front((new TorpedoGoodie(getWorld(), getX(), getY())));
                        setDead();
                        getWorld()->decreaseRound();
                    }
                    else
                        Game().playSound(SOUND_ENEMY_HIT);
                }
            }
        }
    }
}


//PROJECTILE///////////////////////////////////////////////

Projectile::Projectile(int imageID, int startX, int startY, StudentWorld* object, bool playerFired, int damagePoints)
:Actor(imageID, startX, startY, object)
{
    m_ptrToWorld = object;
    m_playerFired = playerFired;
    m_damagePoints = damagePoints;
}

Projectile::~Projectile()
{
}

int Projectile::damagePoint()
{
    return m_damagePoints;
}

bool Projectile::getPlayerFired()
{
    return m_playerFired;
}

bool Projectile::didItFire()
{
    return !m_playerFired;
}


//BULLET////////////////////////////////////

Bullet::Bullet(StudentWorld* object, int startX, int startY, bool playerFired)
:Projectile(IID_BULLET, startX, startY, object, 2, playerFired)
{
}

Bullet::~Bullet()
{
}

void Bullet::doSomething()
{
    //true means alien fired
    if(getX() == getWorld()->getShipPtr()->getX() && getPlayerFired() == true && getY() == getWorld()->getShipPtr()->getY())
    {
        getWorld()->getShipPtr()->damage(2);
        getWorld()->decrementprojectile();
        setDead();
    }
    else
    {
        list <Actor*>::iterator iter;
        for(iter = getWorld()->getList().begin(); iter != getWorld()->getList().end(); iter++)
        {
                Alien* tempalienptr = dynamic_cast<Alien*>(*iter);
                if(tempalienptr != NULL)
                {
                    if(tempalienptr->getX() == getX() && tempalienptr->getY() == getY())
                    {
                        tempalienptr->damage(2);
                        getWorld()->decrementprojectile();
                        setDead();
                    }
                }
        }
    }
    
    if(getPlayerFired())
    {
        moveTo(getX(), getY()+1);
    }
    else
    {
        moveTo(getX(), getY()-1);
    }
    
    //after moving, check if off grid
    if(getY() >= VIEW_HEIGHT || getY() < 0)
    {
        getWorld()->decrementprojectile();
        setDead();
    }
    
    else if(getX() == getWorld()->getShipPtr()->getX() && getPlayerFired() == true && getY() == getWorld()->getShipPtr()->getY())
    {
        getWorld()->getShipPtr()->damage(2);
        getWorld()->decrementprojectile();
        setDead();
    }
    
    if(getPlayerFired() == false)
    {
        list <Actor*>::iterator iter;
        for(iter = getWorld()->getList().begin(); iter != getWorld()->getList().end(); iter++)
        {
            Actor* iterptr = *iter;
            if(iterptr != NULL)
            {
                Alien* tempalienptr = dynamic_cast<Alien*>(*iter);
                if(tempalienptr != NULL)
                {
                    if(tempalienptr->getX() == getX() && tempalienptr->getY() == getY())
                    {
                        tempalienptr->damage(2);
                        getWorld()->decrementprojectile();
                        setDead();
                    }
                }
            }
        }
    }
}


//TORPEDO//////////////////////////////////////

Torpedo::Torpedo(StudentWorld* object, int startX, int startY, bool playerFired)
:Projectile(IID_TORPEDO, startX, startY, object, 8, playerFired)
{
}

Torpedo::~Torpedo()
{
}

void Torpedo::doSomething()
{
    //true means alien fired
    if(getX() == getWorld()->getShipPtr()->getX() && getPlayerFired() == true && getY() == getWorld()->getShipPtr()->getY())
    {
        getWorld()->getShipPtr()->damage(8);
        getWorld()->decrementprojectile();
        setDead();
    }
    else
    {
        list <Actor*>::iterator iter;
        for(iter = getWorld()->getList().begin(); iter != getWorld()->getList().end(); iter++)
        {
            Alien* tempalienptr = dynamic_cast<Alien*>(*iter);
            if(tempalienptr != NULL)
            {
                if(tempalienptr->getX() == getX() && tempalienptr->getY() == getY())
                {
                    tempalienptr->damage(8);
                    getWorld()->decrementprojectile();
                    setDead();
                }
            }
        }
    }
    
    if(getPlayerFired())
    {
        moveTo(getX(), getY()+1);
    }
    else
    {
        moveTo(getX(), getY()-1);
    }

    if(getY() >= VIEW_HEIGHT || getY() < 0)
    {
        getWorld()->decrementprojectile();
        setDead();
    }
    
    else if(getX() == getWorld()->getShipPtr()->getX() && getPlayerFired() == true && getY() == getWorld()->getShipPtr()->getY())
    {
        getWorld()->getShipPtr()->damage(8);
        getWorld()->decrementprojectile();
        setDead();
    }
    
    if(getPlayerFired() == false)
    {
        list <Actor*>::iterator iter;
        for(iter = getWorld()->getList().begin(); iter != getWorld()->getList().end(); iter++)
        {
            Actor* iterptr = *iter;
            if(iterptr != NULL)
            {
                Alien* tempalienptr = dynamic_cast<Alien*>(*iter);
                if(tempalienptr != NULL)
                {
                    if(tempalienptr->getX() == getX() && tempalienptr->getY() == getY())
                    {
                        tempalienptr->damage(8);
                        getWorld()->decrementprojectile();
                        setDead();
                    }
                }
            }
        }
    }
}


//GOODIES////////////////////////////////////////

Goodies::Goodies(int imageID, int startX, int startY, StudentWorld* object)
:Actor(imageID, startX, startY, object)
{
    m_everythird = 0;
    m_goodieTickLifetime = (100/((getWorld()->roundNumber())))+30;
    m_ticksLeftToLive = m_goodieTickLifetime;
}

Goodies::~Goodies()
{
}

int Goodies::everythird()
{
    m_everythird++;
    return m_everythird;
}

double Goodies::goodieTickLifetime()
{
    return m_goodieTickLifetime;
}

double Goodies::ticksLeftToLive()
{
    return m_ticksLeftToLive;
}

void Goodies::resetthird()
{
    m_everythird = 0;
}

void Goodies::decrementticks()
{
    m_ticksLeftToLive--;
}


//FREESHIPGOODIE///////////////////////////////////////

FreeShipGoodie::FreeShipGoodie(StudentWorld* object, int startX, int startY)
:Goodies(IID_FREE_SHIP_GOODIE, startX, startY, object)
{
}

FreeShipGoodie::~FreeShipGoodie()
{
}

void FreeShipGoodie::doSomething()
{
    everythird();
    decrementticks();
    if(getWorld()->getShipPtr()->getX() == getX() && getWorld()->getShipPtr()->getY())
    {
        getWorld()->incLives();
        getWorld()->increaseScore(5000);
        Game().playSound(SOUND_GOT_GOODIE);
        setDead();
    }
    else
    {
        setBrightness((ticksLeftToLive()/goodieTickLifetime())+.2);
    }
    if(ticksLeftToLive() <= 0 || getY() < 0)
    {
        setDead();
    }
    else
    {
        if(everythird() == 3)
        {
            resetthird();
            moveTo(getX(), getY()-1);
            if(getX() == getWorld()->getShipPtr()->getX() && getY())
            {
                getWorld()->incLives();
                getWorld()->increaseScore(5000);
                Game().playSound(SOUND_GOT_GOODIE);
                setDead();
            }
        }
    }
}

//ENERGYGOODIE//////////////////////////////////////

EnergyGoodie::EnergyGoodie(StudentWorld* object, int startX, int startY)
:Goodies(IID_ENERGY_GOODIE, startX, startY, object)
{
    
}

EnergyGoodie::~EnergyGoodie()
{
}

void EnergyGoodie::doSomething()
{
    everythird();
    decrementticks();
    if(getWorld()->getShipPtr()->getX() == getX() && getWorld()->getShipPtr()->getY())
    {
        getWorld()->getShipPtr()->heal();
        getWorld()->increaseScore(5000);
        Game().playSound(SOUND_GOT_GOODIE);
        setDead();
    }
    else
    {
        setBrightness((ticksLeftToLive()/goodieTickLifetime())+.2);
    }
    if(ticksLeftToLive() <= 0 || getY() < 0)
    {
        setDead();
    }
    else
    {
        if(everythird() == 3)
        {
            resetthird();
            moveTo(getX(), getY()-1);
            if(getX() == getWorld()->getShipPtr()->getX() && getY())
            {
                getWorld()->getShipPtr()->heal();
                getWorld()->increaseScore(5000);
                Game().playSound(SOUND_GOT_GOODIE);
                setDead();
            }
        }
    }
}


//TORPEDOGOODIE///////////////////////////////////////

TorpedoGoodie::TorpedoGoodie(StudentWorld* object, int startX, int startY)
:Goodies(IID_TORPEDO_GOODIE, startX, startY, object)
{
    
}

void TorpedoGoodie::doSomething()
{
    if(getX() == getWorld()->getShipPtr()->getX() && getY() == getWorld()->getShipPtr()->getY())
    {
        getWorld()->increaseScore(5000);
        Game().playSound(SOUND_GOT_GOODIE);
        setDead();
        getWorld()->getShipPtr()->addTorpedoes(5);
    }
    else
    {
        setBrightness((ticksLeftToLive()/goodieTickLifetime())+.2);
    }
    if(ticksLeftToLive() <= 0 || getY() < 0)
    {
        setDead();
    }
    else
    {
        if(everythird() == 3)
        {
            resetthird();
            moveTo(getX(), getY()-1);
            if(getX() == getWorld()->getShipPtr()->getX() && getY())
            {
                getWorld()->getShipPtr()->heal();
                getWorld()->increaseScore(5000);
                Game().playSound(SOUND_GOT_GOODIE);
                setDead();
            }
        }
    }
}

TorpedoGoodie::~TorpedoGoodie()
{
}







