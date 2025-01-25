#pragma once
#include <glm/glm.hpp>
#include <fstream>
#include <list>
#include <cinttypes>
#include "../texts/texts.hpp"
#include "utils/common.hpp"
#include "display/renderer.hpp"
#include "display/camera.hpp"
#include "display/draw_info.hpp"
#include "base_physics.hpp"

using namespace std;
using namespace glm;

// Здесь хранятся классы и структуры для организации 

class SpaceObject;
class District;
class Collisions;
class DistrictCell;
class Texture;
class DistrictRenderer;

// Представляет собой пару из указателя на список и
// итератора, указывающего на конкретный элемент списка
template <typename Type>
class ListPElementInfo
{
public:
	ListPElementInfo();
	// Вставляет указатель на элемент pval в список m_plist. 
	// Если функция ранее вставила другой элемент в другой
	// список, она предварительно удаляет этот указатель на 
	// элемент из предыдущего списка 
	void insert(list<Type*>* plist, Type *pval);
	// Удаляет указатель на элемент из списка, в который тот 
	// был вставлен функцией insert
	void remove();
	// Возвращает адрес вставленного функцией insert элемента
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


// Позиционный объект. Имеет x, y координаты
class LocatableObject {
public:
	explicit LocatableObject(ObjectForm &&form = ObjectForm(0.5));
	LocatableObject(LocatableObject&);
	~LocatableObject();

	// Обновляет индексы клеток в соответствии с координатами
	// void normalizeCoords();

	inline ObjectForm& getForm() { return m_form; }
	void addDrawInfo(Texture *texture);
	SpaceObjectDrawInfo* getDrawInfo() { return m_draw_info; }
protected:
	float m_rotation;				// Угол поворота
	Position m_position;			// Позиция объекта
	ObjectForm m_form;				// Данные о форме объекта
	District* m_current_district;	// Область, в которой находится объект
	SpaceObjectDrawInfo* m_draw_info;
};


struct Terrain {
	short terrain_index = 0;
};


class TerrainMap {
public:
	TerrainMap();

private:
	Matrix<short> m_matrix;
};


// Объект локации, имеет x, y координаты и размер
class SpaceObject: public LocatableObject
{
	friend class Collisions;
	friend class District;
public:
	// Создаёт локальный объект с указанными параметрами
	SpaceObject(
		bool is_moveable,
		ObjectForm&& form = ObjectForm(0.5),
		Texture* texture = nullptr,
		float rotation = 0.f,
		float rotation_speed = 0.f,
		float max_rotation_speed = 0.f,
		Vector&& speed_direction = Vector(1.f, 0.f),
		float max_speed = 1.0f,
		float acceleration = 0);
	// Создаёт локальный объект, загружая его из файла
	SpaceObject(FILE* f);
	// Копирующий конструктор
	SpaceObject(SpaceObject& lo);
	// Move-конструктор
	SpaceObject(SpaceObject&& lo);

	~SpaceObject();

	void rotate(float da);				// Вращает объект на da радиан
	void moveTo(double x, double y);
	void moveTo(District* district, double x, double y);

	void initInCell(DistrictCell* cell);

	// Вызывается, когда объект сталкивается с col_obj
	virtual void onCollision(SpaceObject* col_obj) {};
		
	virtual void load(FILE* f);				// Загружает объект из файла
	virtual void save(FILE* f);				// Сохраняет объект в файл

	Position getPosition();					// Возвращает вектор позиции
	glm::vec3 getRenderOrigin( );

	// Добавляет объект в списки объектов локации
	// Если объект ранее находился в другой локации,
	// предварительно удаляет его из её списков 
	void insertToDistrictList();
	// Удаляет объект из списков объектов локации
	void removeFromDistrictList();

	void move(float dt);					// Перемещает объект в состояние через dt мс
	void setSpeedDirection(Vector& speed_direction); // Устанавливает направление скорости
	void setCurrentSpeed(float current_speed); // Устанавливает текущее значение скорости
	Vector getFutureCoords(float dt); // Возвращает положение объекта через время dt
	Position getFuturePosition(float dt);

	bool isMoveable();

	// Обновляет данные о текущей ячейке
	void updateCell();

protected:
	void RemoveFromOldDistrict();

	bool m_is_moveable;					// Является ли движущимся объектом
	
	float m_rotation_speed;				// Скорость поворота, используется только для круглого объекта
	float m_max_rotation_speed;			// Максимальная скорость поворота
	float m_current_speed;				// Текущая скорость объекта
	Vector m_speed_direction;			// Текущее направление скорости объекта
	float m_max_speed;					// Максимальная скорость
	float m_acceleration;				// Ускорение объекта (скорость набора скорости)

	DistrictCell* m_cell;
	ListPElementInfo<SpaceObject> m_cell_info;
	ListPElementInfo<SpaceObject> m_district_info;
	ListPElementInfo<SpaceObject> m_district_moveable_info;
	// Хранит информацию о том, где и в каких списках находится
	// объект для удаления из них в случае необходимости
	// Необходимо написать аллокатор указателя на объект,
	// чтобы при добавлении в другие листы объект 
	// добалял их в свой собственный лист, а, при удалении -
	// удалял.		
};

class Actor : SpaceObject, IDownloadable
{
public:
	Actor(ObjectForm&& form,
		float max_speed = 1.0f, 
		glm::vec2&& speed_direction = glm::vec2(0, 0),
		float acceleration = 0, 
		bool global = false);
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
	friend SpaceObject;
public:
	DistrictCell();

	// Проводит инициализацию ячейки, стирает
	// указатели на соседние ячейки, устанавливает
	// границы в соответствии с положением ячейки
	// в области (x; y)
	void init(ivec2 index_in_district);

	std::list<SpaceObject*>& getInnerObjects();

protected:

	bool m_is_border;
	ivec2 m_index_in_district;

	// Область, в которой содержится ячейка
	District* m_owner_district;

	// Список содержащихся в ячейке объектов
	std::list<SpaceObject*> m_objects;

	TerrainMap m_map;
};

/*	Область(может подгружаться и выгружаться из / в файл по необходимости для игрока
	Хранит сеть входящих в неё ячеек */
class District
{
	friend DistrictCell;
	friend SpaceObject;
	friend DistrictRenderer;
public:
	// Конструктор. Создаёт область
	District(int width, int height);
	~District();

	//void addObject(SpaceObject* obj, float x, float y);

	DistrictCell* getCell(int x, int y);
	DistrictCell* getCell(ivec2 index);

	int getCellsXAmount() const { return m_cells.colCount(); };
	int getCellsYAmount() const { return m_cells.rowCount(); };
		
	void load(FILE* f);					// Загружает область из файла
	void save(FILE* f);					// Сохраняет область в файл

	void moveObjects(float dt);

	bool isCellExist(ivec2 index) const;

	DistrictRenderer* getRenderer() { return m_renderer; }
	void setRenderer(int width, int height);

private:	
	// Хранит ссылки на все пространственные объекты
	list<SpaceObject*> m_space_objects; 
	// Хранит ссылки на все способные двигаться объекты
	list<SpaceObject*> m_moveable_objects;
		
	Matrix<DistrictCell> m_cells;				// Ячейки хранятся по (x, y)

	RectangleArea m_borders;

	DistrictRenderer *m_renderer;

	//vector<District*> m_near_districts;	// Массив указателей на области, в которые можно попасть из этой (не включая соседние)
	//vector<string> m_portal_nets;		// Имена сетей, в которые возможно перейти из текущей области 
};

class DistrictRenderer : public IRendererWorld {
	friend District;
public:
	DistrictRenderer(District* district, int out_width, int out_height);
	void drawWorld() override;
	float getCameraHeight() override { return g_camera->getHeight(); }
	vec3 getCameraPosition() { return g_camera->getGlobalOrigin(); }
	void setCameraPosition(vec3 coords) { g_camera->setGlobalOrigin(coords); }
	void setCameraHeight(float height) { g_camera->setHeight(height); }
private:
	//Camera m_camera;
	District* m_district;
	int m_width;
	int m_height;
};
