#pragma once
#include <glm/glm.hpp>
//#include <stdio.h>
#include <fstream>
#include <list>
#include <vector>
#include <cinttypes>
#include <unordered_map>
#include "../texts/texts.hpp"
#include "utils/common.hpp"
//#include "QuestEvents.h"

using namespace std;
using namespace glm;

// ����� �������� ������ � ��������� ��� ����������� 

class SpaceObject;
class District;
class Collisions;
class DistrictCell;
class Texture;

using Vector = glm::vec2;
using DVector = dvec2;
//#define Vector glm::vec3

struct Position
{
	Vector m_coords;			// ���������� � ������
	ivec2 m_index;				// ������ ������

	void normalizeCoords();
	DVector getGlobalCoords();
	void setFromGlobalCoords(double x, double y);
	void shiftToCoordsSystem(ivec2 cell_index);
	void shiftToCoordsSystem(Position pos);
};

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
	float m_left;
	float m_right;
	float m_bottom;
	float m_top;
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


// ����������� ������. ����� x, y ���������� � ���� ��������
class LocatableObject {
public:
	LocatableObject(ObjectForm &&form = ObjectForm(0.5));
	LocatableObject(LocatableObject&);

	// ��������� ������� ������ � ������������ � ������������
	void normalizeCoords();				
protected:
	float m_rotation;					// ���� ��������
	Position m_position;					// ������� �������
	ObjectForm m_form;					// ������ � ����� �������
	District* m_current_district;	// �������, � ������� ��������� ������
};


class TerrainMap {
public: 
	TerrainMap(District *distict);

private:

};


// ������ �������, ����� x, y ���������� � ������
class SpaceObject: LocatableObject
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
		Vector&& speed_direction = Vector(1.f, 0.f),
		float max_speed = 1.0f,
		float acceleration = 0);
	// ������ ��������� ������, �������� ��� �� �����
	SpaceObject(FILE* f);
	// ���������� �����������
	SpaceObject(SpaceObject& lo);
	// Move-�����������
	SpaceObject(SpaceObject&& lo);

	~SpaceObject();

	void rotate(float da);				// ������� ������ �� da ������
	void moveTo(double x, double y);
	void moveTo(District* district, double x, double y);

	void initInCell(DistrictCell* cell);

	// ����������, ����� ������ ������������ � col_obj
	virtual void onCollision(SpaceObject* col_obj) {};
		
	virtual void load(FILE* f);				// ��������� ������ �� �����
	virtual void save(FILE* f);				// ��������� ������ � ����

	Position getPosition();					// ���������� ������ �������
	glm::vec3 getRenderOrigin( );

	// ��������� ������ � ������ �������� �������
	// ���� ������ ����� ��������� � ������ �������,
	// �������������� ������� ��� �� � ������� 
	void insertToDistrictList();
	// ������� ������ �� ������� �������� �������
	void removeFromDistrictList();

	void move(float dt);					// ���������� ������ � ��������� ����� dt ��
	void setSpeedDirection(Vector& speed_direction); // ������������� ����������� ��������
	void setCurrentSpeed(float current_speed); // ������������� ������� �������� ��������
	Vector getFutureCoords(float dt); // ���������� ��������� ������� ����� ����� dt
	Position getFuturePosition(float dt);

	bool isMoveable();

	// ��������� ������ � ������� ������
	void updateCell();

protected:
	void initInNewDistrict();

	bool m_is_moveable;					// �������� �� ���������� ��������
	
	float m_rotation_speed;				// �������� ��������, ������������ ������ ��� �������� �������
	float m_max_rotation_speed;			// ������������ �������� ��������
	float m_current_speed;				// ������� �������� �������
	Vector m_speed_direction;			// ������� ����������� �������� �������
	float m_max_speed;					// ������������ ��������
	float m_acceleration;				// ��������� ������� (�������� ������ ��������)

	DistrictCell* m_cell;
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
	friend SpaceObject;
public:
	DistrictCell();

	// �������� ������������� ������, �������
	// ��������� �� �������� ������, �������������
	// ������� � ������������ � ���������� ������
	// � ������� (x; y)
	void init(ivec2 index_in_district);

protected:

	bool m_is_border;
	ivec2 m_index_in_district;

	// �������, � ������� ���������� ������
	District* m_owner_district;

	// ������ ������������ � ������ ��������
	std::list<SpaceObject*> m_objects;
};

/*	�������(����� ������������ � ����������� �� / � ���� �� ������������� ��� ������
	������ ���� �������� � �� ����� */
class District
{
	friend DistrictCell;
	friend SpaceObject;
public:
	// �����������. ������ �������
	District(int width, int height);
	~District();

	//void addObject(SpaceObject* obj, float x, float y);

	DistrictCell* getCell(int x, int y);
	DistrictCell* getCell(ivec2 index);

	int getCellsXAmount() const { return m_cells.colCount(); };
	int getCellsYAmount() const { return m_cells.rowCount(); };
		
	void load(FILE* f);					// ��������� ������� �� �����
	void save(FILE* f);					// ��������� ������� � ����

	void moveObjects(float dt);

	bool isCellExist(ivec2 index) const;

private:	
	// ������ ������ �� ��� ���������������� �������
	list<SpaceObject*> m_space_objects; 
	// ������ ������ �� ��� ��������� ��������� �������
	list<SpaceObject*> m_moveable_obje�ts;
		
	Matrix<DistrictCell> m_cells;				// ������ �������� �� (x, y)

	RectangleArea m_borders;

	//vector<District*> m_near_districts;	// ������ ���������� �� �������, � ������� ����� ������� �� ���� (�� ������� ��������)
	//vector<string> m_portal_nets;		// ����� �����, � ������� �������� ������� �� ������� ������� 
};

class Player : TitledObject
{
	bool m_computer;
};