#pragma once
#include <glm/glm.hpp>
//#include <stdio.h>
#include <fstream>
#include <list>
#include <vector>
#include <cinttypes>
#include <unordered_map>
#include "../texts/texts.hpp"
//#include "QuestEvents.h"

using namespace std;

// ����� �������� ������ � ��������� ��� ����������� 

class SpaceObject;
class MoveableObject;
class District;
class Collisions;
class DistrictNet;

using Vector = glm::vec3;
//#define Vector glm::vec3

// ����� ��������, ����������� � ������������
enum CollisionType
{
	m_col_line,					// �����
	m_col_circle,					// ����
	m_col_rectangle,				// �������������
};

// ����� �������
enum ObjectFormType
{
	m_form_circle = m_col_circle,		// ����
	form_rectangle = m_col_rectangle// �������������
};

struct CircleFormData
{
	float m_radius;
};

struct RectangleFormData
{
	float m_height;
	float m_width;
};

union ObjectFormData
{
	CircleFormData* m_circle;
	RectangleFormData* m_rectangle;
};

struct ObjectForm
{
public:
	ObjectForm();
	ObjectForm(ObjectForm& form);
	ObjectForm(float radius);
	ObjectForm(float width, float height);
	~ObjectForm();

	void setData(float radius);
	void setData(float width, float height);
	void releaseData();

	ObjectFormData m_data;
	ObjectFormType m_type;
};

// ������� �������
struct CircleArea
{
	glm::vec2 m_pos;
	float m_radius;
};

// ������������� �������
struct RectangleArea
{
	glm::vec2 m_point1;
	glm::vec2 m_point2;
};

// ������ �������, ����� x, y ���������� � ������
class SpaceObject
{
	friend class Collisions;
	friend class MoveableObject;
public:
	// ������ ��������� ������ � ���������� �����������
	SpaceObject(const District* current_district, 
		ObjectForm&& form = ObjectForm(0.5),
		Vector&& position = Vector(0, 0, 0), float scale = 1.f, 
		float rotation = 0.f, float rotation_speed = 0.f, 
		float max_rotation_speed = 0.f);
	// ������ ��������� ������, �������� ��� �� �����
	SpaceObject(const District* currentDistrict, FILE* f);
	// ���������� �����������
	SpaceObject(SpaceObject& lo);
	// Move-�����������
	SpaceObject(SpaceObject&& lo);

	void rotate(float da);				// ������� ������ �� da ������ (������ ��?)
	void moveTo(float x, float y);
	void moveTo(float x, float y, float level);
	void moveTo(const District* district, float x, float y, float level);
		
	virtual void load(FILE* f);				// ��������� ������ �� �����
	virtual void save(FILE* f);				// ��������� ������ � ����

protected:
	const District* m_current_district;	// �������, � ������� ��������� ������
	float m_rotation;						// ���� ��������
	Vector m_position;					// ������� �������
	float m_rotation_speed;				// �������� ��������, ������������ ������ ��� �������� �������
	float m_max_rotation_speed;				// ������������ �������� ��������
	float m_scale;						// ������� �������
	ObjectForm m_form;					// ������ � ����� �������
	
	// ������ ���������� � ���, ��� � � ����� ������� ���������
	// ������ ��� �������� �� ��� � ������ �������������
	// ���������� �������� ��������� ��������� �� ������,
	// ����� ��� ���������� � ������ ����� ������ 
	// ������� �� � ���� ����������� ����, �, ��� �������� -
	// ������.		
};

class MoveableObject : public SpaceObject
{
	friend class Collisions;
public:
	MoveableObject(const District* district, 
		ObjectForm&& form = ObjectForm(0.5),
		Vector&& position = Vector(0., 0., 0.),
		float scale = 1.f, float rotation = 0.f, 
		float rotation_speed = 0.f, float max_rotation_speed = 0.f,
		Vector&& speed_direction = Vector(0., 0., 0.), 
		float current_speed = 0,
		float max_speed = 1.0f, float acceleration = 0);
	void move(float dt);					// ���������� ������ � ��������� ����� dt ��
	virtual void load(const FILE* f);		// ��������� ������ �� �����
	virtual void save(const FILE* f);		// ��������� ������ � ����
	void setSpeedDirection(Vector& speed_direction); // ������������� ����������� ��������
	void setCurrentSpeed(float current_speed); // ������������� ������� �������� ��������
	Vector getPosition();					// ���������� ������ �������
protected:

	float m_current_speed;						// ������� �������� �������
	Vector m_speed_direction;					// ������� ����������� �������� �������
	float m_max_speed;							// ������������ ��������
	float m_acceleration;						// ��������� ������� (�������� ������ ��������)
};

class Actor : MoveableObject, IDownloadable
{
public:
	Actor(const District* district, Vector&& position, float scale, ObjectForm&& form,
		float max_speed = 1.0f, glm::vec2&& speed_direction = glm::vec2(0, 0),
		float current_speed = 0, float acceleration = 0, bool global = false);
	void load(FILE* f);	// ��������� ������ �� �����
	void save(FILE* f);	// ��������� ������ � ����

	virtual void onLoadArea();			// ����������� ��� �������� �������, �� ������� ��������� ����
	virtual void onUnloadArea();		// ����������� ��� �������� �������, �� ������� ��������� ����
	virtual void onAreaChange();        // ����������� ��� ����� �������

	//void* questEvents;
protected:
	virtual void loadv(FILE* f) {}		// ��������� ������ �� �����
	virtual void savev(FILE* f) {}		// ��������� ������ � ����
};

class Decoration : SpaceObject
{

};

// ���� ������ �������. ������ ��������� �� �������, �������� ��� ��������� ����������� �� � ����������
class DistrictCell
{
	friend District;
public:
	// DistrictCell();
private:
	// �������, � ������� ���������� ������
	District* m_owner_district;

	//District* m_bind_district;	// �������, � ������� ����� ������� �� ���� ������
	
	// ������ ������������ � ������ ��������
	std::list<SpaceObject*> m_objects;	
};

/*	�������(����� ������������ � ����������� �� / � ���� �� ������������� ��� ������
	������ ���� �������� � �� ����� */
class District
{
	friend DistrictNet;
	friend SpaceObject;
public:
	// �����������. ������ �������, ������������� ������� location
	District(DistrictNet* net);
	~District();

	void addSpaceObject(SpaceObject* obj, float x, float y);
	void addMoveableObject(MoveableObject* obj, float x, float y);
		
	void load(FILE* f);					// ��������� ������� �� �����
	void save(FILE* f);					// ��������� ������� � ����

	// ���������� ������, ���������� ����� � ������������ (x; y)
	DistrictCell& getCell(float x, float y);

private:	
	

	// ������ ������ �� ��� ���������������� �������
	list<SpaceObject*> m_space_objects; 
	// ������ ������ �� ��� ��������� ��������� �������
	list<MoveableObject*> m_moveable_obje�ts;
		
	DistrictCell** m_cells;				// ������ �������� �� (x, y)
	DistrictNet* m_net;					// ��������� �� ����-���������

	//vector<District*> m_near_districts;	// ������ ���������� �� �������, � ������� ����� ������� �� ���� (�� ������� ��������)
	//vector<string> m_portal_nets;		// ����� �����, � ������� �������� ������� �� ������� ������� 
};

// ������ ���� ��������
class DistrictNet : TitledObject
{
	friend District;
public:
	DistrictNet(
		uint32_t width,
		uint32_t height,
		float district_size,
		uint32_t cells_partion);

	DistrictNet(FILE* f);

	~DistrictNet();

	// ��������� ����� ������� � ����
	District* addDistrict(uint32_t x, uint32_t y);
	District* getDistrict(uint32_t x, uint32_t y);

	void load(FILE* f);		// ��������� ���� ������� �� �����
	void save(FILE* f);		// ��������� ���� ������� � ����

private:
	// ��������� ������ ���������� �� �������
	// ���� ������� �� ����������, �� ������ 
	// ������ nullptr 
	District*** m_districts;	
	uint32_t m_width, m_height;	// ������ � ������ ���� (� ��������)
	float m_district_size;		// ������ ������� ����

	// ����� �����, �� ������� ������ ������� 
	// ����������� �� ����� � �� ������
	uint32_t m_cells_partition;
	
	float m_cells_size; // ������ ������ ����
};

class Player : TitledObject
{
	bool m_computer;
};