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
using namespace glm;

// ����� �������� ������ � ��������� ��� ����������� 

class SpaceObject;
class District;
class Collisions;
class DistrictNet;

using Vector = glm::vec3;
//#define Vector glm::vec3

// ����� ��������, ����������� � ������������
enum CollisionType
{
	COL_LINE,					// �����
	COL_CIRCLE,					// ����
	COL_RECTANGLE,				// �������������
};

// ����� �������
enum ObjectFormType
{
	FORM_CIRCLE = COL_CIRCLE,		// ����
	FORM_RECTANGLE = COL_RECTANGLE// �������������
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

	vec2&& getSize(float angle);
	float getMaxSize();

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

// ������������ ����� ���� �� ��������� �� ������ �
// ���������, ������������ �� ���������� ������� ������
template <typename Type>
class ListPElementInfo
{
public:
	ListPElementInfo();
	// ��������� ��������� �� ������� pval � ������ m_plist. 
	// ���� ������� ����� �������� ������ ������� � ������
	// ������, ��� �������������� ������� ���� ��������� �� 
	// ������� �� ����������� ������ 
	void insert(list<Type*>* plist, Type *pval);
	// ������� ��������� �� ������� �� ������, � ������� ��� 
	// ��� �������� �������� insert
	void remove();
	// ���������� ����� ������������ �������� insert ��������
	Type* getElement();
	virtual ~ListPElementInfo();

private:
	list<Type*>* m_plist;
	list<Type*>::iterator m_iterator;
};

template <typename Type>
class ListElementMatrix {
public:
	ListElementMatrix(int row_amount, int column_amount);
	~ListElementMatrix();
	
	void shiftLeft();
	void shiftRight();
	void shiftTop();
	void shiftBottom();

	ListPElementInfo<Type>* getInfo(int row, int column);

private:
	ListPElementInfo<Type>*** m_buffer;
	int m_column_amount;
	int m_row_amount;
};

// ������ �������, ����� x, y ���������� � ������
class SpaceObject
{
	friend class Collisions;
	friend class District;
public:
	// ������ ��������� ������ � ���������� �����������
	SpaceObject(
		bool is_moveable,
		ObjectForm&& form = ObjectForm(0.5),
		float rotation = 0.f,
		float rotation_speed = 0.f,
		float max_rotation_speed = 0.f,
		Vector&& speed_direction = Vector(1.f, 0.f, 0.f),
		float max_speed = 1.0f,
		float acceleration = 0);
	// ������ ��������� ������, �������� ��� �� �����
	SpaceObject(FILE* f);
	// ���������� �����������
	SpaceObject(SpaceObject& lo);
	// Move-�����������
	SpaceObject(SpaceObject&& lo);

	void rotate(float da);				// ������� ������ �� da ������ (������ ��?)
	void moveTo(float x, float y);
	void moveTo(const District* district, float x, float y);
		
	virtual void load(FILE* f);				// ��������� ������ �� �����
	virtual void save(FILE* f);				// ��������� ������ � ����

	Vector getPosition();					// ���������� ������ �������
	glm::vec2 getRenderOrigin( );

	// ��������� ������ � ������ �������� �������
	// ���� ������ ����� ��������� � ������ �������,
	// �������������� ������� ��� �� � ������� 
	void insertToDistrictList();
	// ������� ������ �� ������� �������� �������
	void removeFromDistrictList();

	void move(float dt);					// ���������� ������ � ��������� ����� dt ��
	void setSpeedDirection(Vector& speed_direction); // ������������� ����������� ��������
	void setCurrentSpeed(float current_speed); // ������������� ������� �������� ��������
	Vector getFuturePosition(float dt); // ���������� ��������� ������� ����� ����� dt

	bool isMoveable();

protected:
	void initInNewDistrictNet();

	bool m_is_moveable;					// �������� �� ���������� ��������

	const District* m_current_district;	// �������, � ������� ��������� ������
	float m_rotation;					// ���� ��������
	Vector m_position;					// ������� �������
	float m_rotation_speed;				// �������� ��������, ������������ ������ ��� �������� �������
	float m_max_rotation_speed;			// ������������ �������� ��������
	float m_current_speed;				// ������� �������� �������
	Vector m_speed_direction;			// ������� ����������� �������� �������
	float m_max_speed;					// ������������ ��������
	float m_acceleration;				// ��������� ������� (�������� ������ ��������)
	ObjectForm m_form;					// ������ � ����� �������

	ListPElementInfo<SpaceObject> m_cell_info;
	ListPElementInfo<SpaceObject> m_district_info;
	ListPElementInfo<SpaceObject> m_district_moveable_info;
	// ������ ���������� � ���, ��� � � ����� ������� ���������
	// ������ ��� �������� �� ��� � ������ �������������
	// ���������� �������� ��������� ��������� �� ������,
	// ����� ��� ���������� � ������ ����� ������ 
	// ������� �� � ���� ����������� ����, �, ��� �������� -
	// ������.		
};

class Actor : SpaceObject, IDownloadable
{
public:
	Actor(ObjectForm&& form,
		float max_speed = 1.0f, 
		glm::vec2&& speed_direction = glm::vec2(0, 0),
		float acceleration = 0, 
		bool global = false);
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
	DistrictCell() = default;
private:
	// �������, � ������� ���������� ������
	District* m_owner_district;

	DistrictCell* m_left;
	DistrictCell* m_right;
	DistrictCell* m_top;
	DistrictCell* m_bottom;
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
	District(DistrictNet* net, int col_index, int row_index);
	~District();

	//void addObject(SpaceObject* obj, float x, float y);
		
	void load(FILE* f);					// ��������� ������� �� �����
	void save(FILE* f);					// ��������� ������� � ����

	// ���������� ������, ���������� ����� � ������������ (x; y)
	DistrictCell& getCell(float x, float y);
	// ���������� ������ [i, j] ������, ����������
	// ����� � ������������ (x; y)
	ivec2&& getCellIndex(float x, float y);

private:	
	// ������ ������ �� ��� ���������������� �������
	list<SpaceObject*> m_space_objects; 
	// ������ ������ �� ��� ��������� ��������� �������
	list<SpaceObject*> m_moveable_obje�ts;
		
	DistrictCell** m_cells;				// ������ �������� �� (x, y)
	DistrictNet* m_net;					// ��������� �� ����-���������

	int m_row_index;	// ������ ������ � ���� ��������
	int m_col_index;	// ������ ������� � ���� ��������

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

	void moveObjects(float dt);

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

	// ����� ��������� ��������
	uint32_t m_districts_amount;
	
	// ����� � ������ ������� ����������� ��������
	vec2 m_min_load_index;
	// ������� � ������ ������� ����������� ��������
	vec2 m_max_load_index;	

	// ����� �����, �� ������� ������ ������� 
	// ����������� �� ����� � �� ������
	uint32_t m_cells_partition;
	
	float m_cells_size; // ������ ������ ����
};

class Player : TitledObject
{
	bool m_computer;
};