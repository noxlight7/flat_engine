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

// Здесь хранятся классы и структуры для организации 

class SpaceObject;
class MoveableObject;
class District;
class Collisions;
class DistrictNet;

using Vector = glm::vec3;
//#define Vector glm::vec3

// Форма объектов, участвующих в столкновении
enum CollisionType
{
	m_col_line,					// Линия
	m_col_circle,					// Круг
	m_col_rectangle,				// Прямоугольник
};

// Форма объекта
enum ObjectFormType
{
	m_form_circle = m_col_circle,		// Круг
	form_rectangle = m_col_rectangle// Прямоугольник
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

// Круглая область
struct CircleArea
{
	glm::vec2 m_pos;
	float m_radius;
};

// Прямоугольная область
struct RectangleArea
{
	glm::vec2 m_point1;
	glm::vec2 m_point2;
};

// Объект локации, имеет x, y координаты и размер
class SpaceObject
{
	friend class Collisions;
	friend class MoveableObject;
public:
	// Создаёт локальный объект с указанными параметрами
	SpaceObject(const District* current_district, 
		ObjectForm&& form = ObjectForm(0.5),
		Vector&& position = Vector(0, 0, 0), float scale = 1.f, 
		float rotation = 0.f, float rotation_speed = 0.f, 
		float max_rotation_speed = 0.f);
	// Создаёт локальный объект, загружая его из файла
	SpaceObject(const District* currentDistrict, FILE* f);
	// Копирующий конструктор
	SpaceObject(SpaceObject& lo);
	// Move-конструктор
	SpaceObject(SpaceObject&& lo);

	void rotate(float da);				// Вращает объект на da радиан (против чс?)
	void moveTo(float x, float y);
	void moveTo(float x, float y, float level);
	void moveTo(const District* district, float x, float y, float level);
		
	virtual void load(FILE* f);				// Загружает объект из файла
	virtual void save(FILE* f);				// Сохраняет объект в файл

protected:
	const District* m_current_district;	// Область, в которой находится объект
	float m_rotation;						// Угол поворота
	Vector m_position;					// Позиция объекта
	float m_rotation_speed;				// Скорость поворота, используется только для круглого объекта
	float m_max_rotation_speed;				// Максимальная скорость поворота
	float m_scale;						// Масштаб объекта
	ObjectForm m_form;					// Данные о форме объекта
	
	// Хранит информацию о том, где и в каких списках находится
	// объект для удаления из них в случае необходимости
	// Необходимо написать аллокатор указателя на объект,
	// чтобы при добавлении в другие листы объект 
	// добалял их в свой собственный лист, а, при удалении -
	// удалял.		
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
	void move(float dt);					// Перемещает объект в состояние через dt мс
	virtual void load(const FILE* f);		// Загружает объект из файла
	virtual void save(const FILE* f);		// Сохраняет объект в файл
	void setSpeedDirection(Vector& speed_direction); // Устанавливает направление скорости
	void setCurrentSpeed(float current_speed); // Устанавливает текущее значение скорости
	Vector getPosition();					// Возвращает вектор позиции
protected:

	float m_current_speed;						// Текущая скорость объекта
	Vector m_speed_direction;					// Текущее направление скорости объекта
	float m_max_speed;							// Максимальная скорость
	float m_acceleration;						// Ускорение объекта (скорость набора скорости)
};

class Actor : MoveableObject, IDownloadable
{
public:
	Actor(const District* district, Vector&& position, float scale, ObjectForm&& form,
		float max_speed = 1.0f, glm::vec2&& speed_direction = glm::vec2(0, 0),
		float current_speed = 0, float acceleration = 0, bool global = false);
	void load(FILE* f);	// Загружает объект из файла
	void save(FILE* f);	// Сохраняет объект в файл

	virtual void onLoadArea();			// Срабатывает при загрузке области, на которой находится актёр
	virtual void onUnloadArea();		// Срабатывает при выгрузке области, на которой находится актёр
	virtual void onAreaChange();        // Срабатывает при смене области

	//void* questEvents;
protected:
	virtual void loadv(FILE* f) {}		// Загружает объект из файла
	virtual void savev(FILE* f) {}		// Сохраняет объект в файл
};

class Decoration : SpaceObject
{

};

// Сеть Ячейки области. Хранит указатели на объекты, частично или полностью находящиеся на её территории
class DistrictCell
{
	friend District;
public:
	// DistrictCell();
private:
	// Область, в которой содержится ячейка
	District* m_owner_district;

	//District* m_bind_district;	// Область, в которую можно перейти из этой ячейки
	
	// Список содержащихся в ячейке объектов
	std::list<SpaceObject*> m_objects;	
};

/*	Область(может подгружаться и выгружаться из / в файл по необходимости для игрока
	Хранит сеть входящих в неё ячеек */
class District
{
	friend DistrictNet;
	friend SpaceObject;
public:
	// Конструктор. Создаёт область, принадлежащую локации location
	District(DistrictNet* net);
	~District();

	void addSpaceObject(SpaceObject* obj, float x, float y);
	void addMoveableObject(MoveableObject* obj, float x, float y);
		
	void load(FILE* f);					// Загружает область из файла
	void save(FILE* f);					// Сохраняет область в файл

	// Возвращает ячейку, содержащую точку с координатами (x; y)
	DistrictCell& getCell(float x, float y);

private:	
	

	// Хранит ссылки на все пространственные объекты
	list<SpaceObject*> m_space_objects; 
	// Хранит ссылки на все способные двигаться объекты
	list<MoveableObject*> m_moveable_objeсts;
		
	DistrictCell** m_cells;				// Ячейки хранятся по (x, y)
	DistrictNet* m_net;					// Указатель на сеть-владельца

	//vector<District*> m_near_districts;	// Массив указателей на области, в которые можно попасть из этой (не включая соседние)
	//vector<string> m_portal_nets;		// Имена сетей, в которые возможно перейти из текущей области 
};

// Единая сеть областей
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

	// Добавляет новую область в сеть
	District* addDistrict(uint32_t x, uint32_t y);
	District* getDistrict(uint32_t x, uint32_t y);

	void load(FILE* f);		// Загружает сеть локаций из файла
	void save(FILE* f);		// Сохраняет сеть локаций в файл

private:
	// Двумерный массив указателей на области
	// Если области не существует, то массив 
	// хранит nullptr 
	District*** m_districts;	
	uint32_t m_width, m_height;	// Ширина и высота сети (в областях)
	float m_district_size;		// Размер области сети

	// Число ячеек, на которые каждая область 
	// разбивается по длине и по ширине
	uint32_t m_cells_partition;
	
	float m_cells_size; // Размер ячейки сети
};

class Player : TitledObject
{
	bool m_computer;
};