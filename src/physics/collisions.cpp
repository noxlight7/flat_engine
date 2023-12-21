#include "collisions.hpp"

auto func = Collisions::Line0Line0;

typedef bool (*CollisionFunc) (SpaceObject* o1, SpaceObject* o2, int dt, float* colTime);

CollisionFunc funcs[3][3] = {
	{Collisions::Line0Line0, Collisions::Line0Circle1, Collisions::Line0Rectangle2},
	{Collisions::Circle1Line0, Collisions::Circle1Circle1, Collisions::Circle1Rectangle2},
	{Collisions::Rectangle2Line0, Collisions::Rectangle2Circle1, Collisions::Rectangle2Rectangle2}
};

bool Collisions::Collision(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return funcs[o1->m_form.m_type][o2->m_form.m_type](o1, o2, dt, colTime);
}

bool Collisions::Line0Line0(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return false;
}
bool Collisions::Line0Circle1(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return false;
}
bool Collisions::Line0Rectangle2(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return false;
}
bool Collisions::Circle1Line0(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return false;
}
bool Collisions::Circle1Circle1(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return false;
}
bool Collisions::Circle1Rectangle2(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return false;
}
bool Collisions::Rectangle2Line0(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return false;
}
bool Collisions::Rectangle2Circle1(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return false;
}
bool Collisions::Rectangle2Rectangle2(SpaceObject* o1, SpaceObject* o2, int dt, float* colTime)
{
	return false;
}

