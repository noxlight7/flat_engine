#include "collisions.hpp"
#include "factory/definitions.hpp"

using namespace math;

auto func = Collisions::Line0Line0;

typedef bool (*CollisionFunc) (SpaceObject* o1, SpaceObject* o2, float dt, float* colTime);

CollisionFunc funcs[3][3] = {
	{Collisions::Line0Line0, Collisions::Line0Circle1, Collisions::Line0Rectangle2},
	{Collisions::Circle1Line0, Collisions::Circle1Circle1, Collisions::Circle1Rectangle2},
	{Collisions::Rectangle2Line0, Collisions::Rectangle2Circle1, Collisions::Rectangle2Rectangle2}
};

bool Collisions::Collision(SpaceObject* o1, SpaceObject* o2, float dt, float* colTime)
{
	bool rv = funcs[o1->getForm().m_type][o2->getForm().m_type](o1, o2, dt, colTime);
	
	if (rv) {
		o1->onCollision(o2);
		o2->onCollision(o1);
	}

	return rv;
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
	auto o1_center = o1->getPosition().m_coords;
	auto o2_center = o2->getPosition().m_coords;
	auto o1_future_pos = o1->getFutureCoords(dt);
	Vector o2_future_pos;
	float r1 = o1->getForm().m_data.m_circle->m_radius;
	float r2 = o2->getForm().m_data.m_circle->m_radius;
	Vector v1 = o1->getSpeedDirection() * o1->getCurrentSpeed();

	SpaceObject* o2_move = nullptr;
	Vector v2;
	if (o2->isMoveable()) {
		o2_move = o2;
		o2_future_pos = o2_move->getFutureCoords(dt);
		v2 = o2_move->getSpeedDirection() * o2_move->getCurrentSpeed();
	}
	else {
		o2_future_pos = o2_center;
		v2 = Vector(0, 0);
	}

	float r122 = r1 + r2;
	r122 *= r122;
	float dx = o2_center.x - o1_center.x;
	float dy = o2_center.y - o1_center.y;
	float dvx = v2.x - v1.x;
	float dvy = v2.y - v1.y;
	float dvx2 = dvx * dvx;
	float dvy2 = dvy * dvy;

	if (fcompare(dvx2 + dvy2, 0))
		return false;

	float t = -(dvx * dx + dvy * dy) / (dvx2 + dvy2);

	float min_dist;
	if (t > 0 && t < dt) {
		Vector pos2;
		if (o2_move)
			pos2 = o2_move->getFutureCoords(t);
		else
			pos2 = o2_center;

		min_dist = glm::distance(
			o1->getFutureCoords(t), pos2);
	}
	else if (t >= dt)
		min_dist = glm::distance(
			o1_future_pos, o2_future_pos);
	else
		min_dist = glm::distance(
			o1_center, o2_center);

	if (min_dist > r1 + r2)
		return false;

	float a = dvx2 + dvy2;
	float b_2 = dx * dvx + dy * dvy;
	float c = dx * dx + dy * dy - r122;

	float discriminant1 = b_2 * b_2 - a * c;

	if (discriminant1 <= 0)
		return false;

	discriminant1 = sqrt(discriminant1);

	float t1 = (-b_2 - discriminant1) / a;
	float t2 = (-b_2 + discriminant1) / a;
	if (t1 >= 0)
		t = t1;
	else
		t = t2;

	*colTime = t;

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
	//auto o1_coords = o1->getPosition();
	//auto o2_coords = o2->getPosition();
	auto o1_future_coords = o1->getFutureCoords(dt);
	Vector o2_future_coords;
	float height1 = o1->getForm().m_data.m_rectangle->m_height;
	float width1 = o1->getForm().m_data.m_rectangle->m_width;
	if (fcompare(o1->getRotation(), k_rotation_left) || fcompare(o1->getRotation(), k_rotation_right)) {
		std::swap(width1, height1);
	}
	float height2 = o2->getForm().m_data.m_rectangle->m_height;
	float width2 = o2->getForm().m_data.m_rectangle->m_width;
	if (fcompare(o2->getRotation(), k_rotation_left) || fcompare(o2->getRotation(), k_rotation_right)) {
		std::swap(width2, height2);
	}

	SpaceObject* o2_move = nullptr;
	if (o2->isMoveable()) {
		o2_move = o2;
		o2_future_coords = o2_move->getFutureCoords(dt);
	}
	else {
		o2_future_coords = o2->getPosition().m_coords;
	}

	float x1 = o1_future_coords.x - width1 / 2;
	float y1 = o1_future_coords.y - height1 / 2;
	float x2 = o2_future_coords.x - width2 / 2;
	float y2 = o2_future_coords.y - height2 / 2;

	if (!(x1 + width1 >= x2 && x1 <= x2 + width2 &&
		y1 + height1 >= y2 && y1 <= y2 + height2))
		return false;
	
	*colTime = dt;

	return true;
}
