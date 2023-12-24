#include "collisions.hpp"

auto func = Collisions::Line0Line0;

typedef bool (*CollisionFunc) (SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);

CollisionFunc funcs[3][3] = {
	{Collisions::Line0Line0, Collisions::Line0Circle1, Collisions::Line0Rectangle2},
	{Collisions::Circle1Line0, Collisions::Circle1Circle1, Collisions::Circle1Rectangle2},
	{Collisions::Rectangle2Line0, Collisions::Rectangle2Circle1, Collisions::Rectangle2Rectangle2}
};

bool Collisions::Collision(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	return funcs[o1->m_form.m_type][o2->m_form.m_type](o1, o2, dt, colTime);
}

bool Collisions::Line0Line0(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	return false;
}
bool Collisions::Line0Circle1(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	return false;
}
bool Collisions::Line0Rectangle2(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	return false;
}
bool Collisions::Circle1Line0(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	return false;
}
bool Collisions::Circle1Circle1(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	auto o1_center = o1->getPosition();
	auto o2_center = o2->getPosition();
	auto o1_future_pos = o1->getFuturePosition(dt);
	Vector o2_future_pos;
	float r1 = o1->m_form.m_data.m_circle->m_radius;
	float r2 = o2->m_form.m_data.m_circle->m_radius;
	
	if (o2->m_is_moveable)
		o2_future_pos = ((SpaceObject*)o2)->getFuturePosition(dt);
	else
		o2_future_pos = o2_center;

	float x11 = o1_center.x;
	float y11 = o1_center.y;
	float x12 = o1_future_pos.x;
	float y12 = o1_future_pos.y;

	float x21 = o2_center.x;
	float y21 = o2_center.y;
	float x22 = o2_future_pos.x;
	float y22 = o2_future_pos.y;

	float a = x11 * x11 + x12 * x12 + y11 * y11 + y12 * y12 + x21 * x21 + x22 * x22 + y21 * y21 + y22 * y22 
		+ 2 * (-x11 * x12 - x21 * x22 - y11 * y12 - y21 * y22 - x11 * x21 - y11 * y21 + x11 * x22 
			+ y11 * y22 + x12 * x21 + y12 * y21 - x12 * x22 - y12 * y22);
	float b = 2 * (-x11 * x11 - x21 * x21 - y11 * y11 - y21 * y21 + x11 * x12 + y11 * y12
		+ x21 * x22 + y21 * y22 - x11 * x22 - y11 * y22 - x12 * x21 - y12 * y21 
		+ 2 * x11 * x21 + 2 * y11 * y21);
	float c = x11 * x11 - 2 * x11 * x21 + x21 * x21 + y11 * y11 - 2 * y11 * y21 + y21 * y21
		- (r1 + r2) * (r1 + r2);

	float discriminant = b * b - 4 * a * c;

	if (discriminant <= 0)
		return false;
	
	discriminant = sqrt(discriminant);

	float t;
	if (a != 0) {
		float t1 = (-b - discriminant) / (2 * a);
		float t2 = (-b + discriminant) / (2 * a);
		if (t1 >= 0)
			t = t1;
		else
			t = t2;
	}
	else if (b != 0) {
		t = -c / b;
	}
	else
		return false;

	*colTime = t * dt;

	return true;
}
bool Collisions::Circle1Rectangle2(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	return false;
}
bool Collisions::Rectangle2Line0(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	return false;
}
bool Collisions::Rectangle2Circle1(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	return false;
}
bool Collisions::Rectangle2Rectangle2(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	return false;
}