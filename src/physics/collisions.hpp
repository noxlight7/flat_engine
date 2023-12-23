#pragma once
#include "physics.hpp"

// Моделирует столкновение объектов через dt, возвращает true, если столкновение произошло
// В переменную colTime записывается время столкновения
class Collisions
{
	friend SpaceObject;
public:
	static bool Collision(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);

	static bool Line0Line0(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Line0Circle1(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Line0Rectangle2(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Circle1Line0(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Circle1Circle1(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Circle1Rectangle2(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Rectangle2Line0(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Rectangle2Circle1(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);
	static bool Rectangle2Rectangle2(MoveableObject* o1, SpaceObject* o2, float dt, float* colTime);
};
