#pragma once
#include "physics.hpp"

// Моделирует столкновение объектов через dt, возвращает true, если столкновение произошло
// В переменную colTime записывается время столкновения
class Collisions
{
	friend SpaceObject;
public:
	static bool Collision(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);

	static bool Line0Line0(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Line0Circle1(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Line0Rectangle2(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Circle1Line0(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Circle1Circle1(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Circle1Rectangle2(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Rectangle2Line0(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Rectangle2Circle1(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Rectangle2Rectangle2(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);
};
