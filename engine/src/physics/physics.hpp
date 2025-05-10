#pragma once
#include <glm/glm.hpp>
#include <fstream>
#include <list>
#include <cinttypes>
#include "../texts/texts.hpp"
#include "utils/common.hpp"
#include "display/renderer_world.hpp"
#include "display/game_camera.hpp"
#include "base_physics.hpp"
#include "renderer.hpp"
#include "display/display_system.hpp"
#include "entities/base_entity.hpp"
#include "display/display_objects.hpp"
#include "utils/constants.hpp"

// Здесь хранятся классы и структуры для организации 

class SpaceObject;
class Actor;
class District;
class Collisions;
class DistrictCell;
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
	typename list<Type*>::iterator m_iterator;
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

constexpr float k_rotation_up = 0.f;
constexpr float k_rotation_left = math::g_pi / 2.f;
constexpr float k_rotation_down = math::g_pi;
constexpr float k_rotation_right = math::g_pi * 3.f / 2.f;

// Позиционный объект. Имеет x, y координаты
class LocatableObject : public BaseEntity{
public:
	LocatableObject() = delete;
	LocatableObject(LocatableObject&) = delete;
	LocatableObject(LocatableObject&&) noexcept ;
	explicit LocatableObject(uint32_t id, uint32_t type_id, ObjectForm& form);
	explicit LocatableObject(PoolID* pool_id, uint32_t type_id, ObjectForm& form);
	LocatableObject(uint32_t id, LocatableObject&);
	LocatableObject(PoolID* pool_id, LocatableObject&);
	~LocatableObject() override;

	// Обновляет индексы клеток в соответствии с координатами
	// void normalizeCoords();

	ObjectForm& getForm() { return m_form; }

	[[nodiscard]] uint32_t getTypeID() const { return m_type_id; }
	// [[nodiscard]] Position& getPosition() { return m_position; }

	void rotate(float da);				// Вращает объект на da радиан
	void setRotation(float angle);
	[[nodiscard]] float getRotation() const {return m_rotation;}

	Position& getPosition();					// Возвращает вектор позиции
	void setPosition(Position pos) { this->m_position = pos; }
	[[nodiscard]] glm::dvec3 getRenderOrigin( ) const;

	float getRotation() { return m_rotation; }

protected:
	float m_rotation{};				// Угол поворота
	Position m_position{};			// Позиция объекта
	ObjectForm m_form{};				// Данные о форме объекта
	District* m_current_district{};	// Область, в которой находится объект
	uint32_t m_type_id{};				// Идентификатор типа объекта

private:
	static uint32_t c_next_id;	// Следующий идентификатор объекта
	static uint32_t nextID() { return c_next_id++; }
};

using LocatableObject2Predict = std::function<bool(
	LocatableObject& subject, LocatableObject& object)>;
using LocatableObject1Predict = std::function<bool(LocatableObject& object)>;

inline bool alwaysTrue2(LocatableObject& subject, LocatableObject& object){ return true; }
inline bool alwaysTrue1(LocatableObject& obj){ return true; }

// class TerrainMap {
// public:
// 	TerrainMap();
//
// private:
// 	Matrix<TerrainID> m_matrix;
// };


// Объект локации, имеет x, y координаты и размер
class SpaceObject: public LocatableObject
{
public:
	SpaceObject() = delete;
	SpaceObject(SpaceObject&) = delete;
	// Создаёт локальный объект с указанными параметрами
	SpaceObject(
		PoolID* pool_id,
		bool is_moveable,
		uint32_t type_id,
		ObjectForm& form,
		float rotation = 0.f,
		float rotation_speed = 0.f,
		float max_rotation_speed = 0.f,
		Vector&& speed_direction = Vector(1.f, 0.f),
		float max_speed = 1.0f,
		float acceleration = 0);
	SpaceObject(
		uint32_t id,
		bool is_moveable,
		uint32_t type_id,
		ObjectForm& form,
		float rotation = 0.f,
		float rotation_speed = 0.f,
		float max_rotation_speed = 0.f,
		Vector&& speed_direction = Vector(1.f, 0.f),
		float max_speed = 1.0f,
		float acceleration = 0);
	// Создаёт локальный объект, загружая его из файла
	// SpaceObject(FILE* f);
	// Копирующий конструктор
	SpaceObject(PoolID* pool_id, SpaceObject& lo);
	SpaceObject(uint32_t id, SpaceObject& lo);
	// Move-конструктор
	SpaceObject(SpaceObject&& lo) noexcept ;

	~SpaceObject() override;

	void moveTo(double x, double y);
	void moveTo(District* district, double x, double y);

	void initInCell(DistrictCell* cell);

	// Вызывается, когда объект сталкивается с col_obj
	virtual void onCollision(SpaceObject* col_obj) {};
		
	virtual void load(FILE* f);				// Загружает объект из файла
	virtual void save(FILE* f);				// Сохраняет объект в файл

	// Добавляет объект в списки объектов локации
	// Если объект ранее находился в другой локации,
	// предварительно удаляет его из её списков 
	void insertToDistrictList();
	// Удаляет объект из списков объектов локации
	void removeFromDistrictList();

	void move(float dt);					// Перемещает объект в состояние через dt мс
	void setSpeedDirection(Vector& speed_direction); // Устанавливает направление скорости
	void setCurrentSpeed(float current_speed); // Устанавливает текущее значение скорости
	void setMaxSpeed(float max_speed);
	[[nodiscard]] float getMaxSpeed() const { return m_max_speed; }
	Vector getFutureCoords(float dt); // Возвращает положение объекта через время dt
	Position getFuturePosition(float dt);

	bool isMoveable() const;

	// Обновляет данные о текущей ячейке
	void updateCell();

	DistrictCell* getCurrentCell() { return m_cell; }

	float getCurrentSpeed() const { return m_current_speed; }
	Vector getSpeedDirection() const { return m_speed_direction; }

protected:
	void RemoveFromOldDistrict();

	bool m_is_moveable{};					// Является ли движущимся объектом
	
	float m_rotation_speed{};				// Скорость поворота, используется только для круглого объекта
	float m_max_rotation_speed{};			// Максимальная скорость поворота
	float m_current_speed{};				// Текущая скорость объекта
	Vector m_speed_direction{};				// Текущее направление скорости объекта
	float m_max_speed{};					// Максимальная скорость
	float m_acceleration{};					// Ускорение объекта (скорость набора скорости)

	DistrictCell* m_cell{};
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

class Actor {
	float m_current_hit_points;
	float m_max_hit_points;
	std::unique_ptr<SpaceObject> m_physical_body;
public:
	virtual ~Actor() = default;
	Actor(std::unique_ptr<SpaceObject> physical_body, float max_hit_points)
		: m_current_hit_points(max_hit_points), m_max_hit_points(max_hit_points),
		  m_physical_body(std::move(physical_body)) {
	}

	[[nodiscard]] SpaceObject* getPhysicalBody() const { return m_physical_body.get(); }
	[[nodiscard]] float getCurrentHitPoints() const { return m_current_hit_points; }
	[[nodiscard]] float getMaxHitPoints() const { return m_max_hit_points; }

	void setCurrentHitPoints(float current_hit_points){ m_current_hit_points = current_hit_points; }
	void setMaxHitPoints(float max_hit_points){ m_max_hit_points = max_hit_points; }
	void setPhysicalBody(std::unique_ptr<SpaceObject> physical_body){ m_physical_body = std::move(physical_body); }

	std::list<LocatableObject*> getAreaInterestObjects();

	void load(FILE* f);	// Загружает объект из файла
	void save(FILE* f);	// Сохраняет объект в файл

	void moveTo(double x, double y);
	void moveTo(District* district, double x, double y);

	virtual void onLoadArea();			// Срабатывает при загрузке области, на которой находится актёр
	virtual void onUnloadArea();		// Срабатывает при выгрузке области, на которой находится актёр
	virtual void onAreaChange();        // Срабатывает при смене области
	virtual void onTick();				// Срабатывает на каждом тике

	virtual float getAreaInterestRadius() { return constants::actors::k_default_visible_radius; }

	[[nodiscard]] virtual bool isNetRelevantFor(const Actor& actor) const { return true; }
	virtual void onDamaged(Actor& damager, float damage) {m_current_hit_points -= damage;}
	virtual void onDie() {}
	virtual void onSpawn() {}

	//void* questEvents;
protected:
	virtual void loadv(FILE* f) {}		// Загружает объект из файла
	virtual void savev(FILE* f) {}		// Сохраняет объект в файл
};

// class Decoration : SpaceObject
// {
//
// };

// Сеть Ячейки области. Хранит указатели на объекты, частично или полностью находящиеся на её территории
class DistrictCell
{
public:
	DistrictCell();

	// Проводит инициализацию ячейки, стирает
	// указатели на соседние ячейки, устанавливает
	// границы в соответствии с положением ячейки
	// в области (x; y)
	void init(glm::ivec2 index_in_district);

	std::list<SpaceObject*>& getInnerObjects();
	glm::ivec2 getIndexInDistrict() const { return m_index_in_district; }
	std::list<SpaceObject*>& getObjects() { return m_objects; }

	[[nodiscard]] District* getOwnerDistrict() const {return m_owner_district;}

	District *getOwnerDistrict() {return m_owner_district;}
	void setOwnerDistrict(District* m_owner_district) { this->m_owner_district = m_owner_district;}

	[[nodiscard]] bool isFill() const { return m_fill_object != nullptr; }
	[[nodiscard]] SpaceObject &getFillObject() const {return *m_fill_object;}
	void addFillObject(std::unique_ptr<SpaceObject> fill_object);

	bool isBorder() { return m_is_border; }

protected:

	bool m_is_border;
	glm::ivec2 m_index_in_district;

	// Область, в которой содержится ячейка
	District* m_owner_district;

	// Список содержащихся в ячейке объектов
	std::list<SpaceObject*> m_objects;

	std::unique_ptr<SpaceObject> m_fill_object;
};

/*	Область(может подгружаться и выгружаться из / в файл по необходимости для игрока
	Хранит сеть входящих в неё ячеек */
class District
{
public:
	// Конструктор. Создаёт область
	District(int width, int height, const TerrainMap& terrain_map);
	~District();

	//void addObject(SpaceObject* obj, float x, float y);

	DistrictCell* getCell(int x, int y);
	DistrictCell* getCell(glm::ivec2 index);
	Matrix<TerrainID>& getTerrainMatrix() { return m_terrain_matrix; }
	list<SpaceObject*>& getSpaceObjects() { return m_space_objects; }
	list<SpaceObject*>& getMoveableObjects() { return m_moveable_objects; }
	DistrictCell& getCellWithXY(const int y, const int x) { return m_cells(y, x); }

	[[nodiscard]] int getCellsXAmount() const { return m_cells.colCount(); }
	[[nodiscard]] int getCellsYAmount() const { return m_cells.rowCount(); }
		
	void load(FILE* f);					// Загружает область из файла
	void save(FILE* f);					// Сохраняет область в файл

	void moveObjects(float dt);

	bool isCellExist(glm::ivec2 index) const;

	DistrictRenderer* getRenderer() { return m_renderer; }
	void setRenderer(int width, int height);

	class RectangleAreaRange {
		District& m_district;
		int m_row_start, m_row_end;
		int m_col_start, m_col_end;

	public:
		class Iterator {
			District& m_district;
			std::list<SpaceObject*>::iterator m_list_it;
			int m_row_start, m_row_end;
			int m_col_start, m_col_end;
			int m_current_row, m_current_col;

			void advanceCell();
			void skipEmpty();

		public:
			Iterator(District& district,
				int row_start, int col_start,
				int row_end, int col_end, bool end = false);

			bool operator!=(const Iterator&) const;
			SpaceObject& operator*() const;
			Iterator& operator++();
		};

		RectangleAreaRange(District& district, int row_start, int col_start, int row_end, int col_end);
		Iterator begin();
		Iterator end();
	};

	RectangleAreaRange getRectangleAreaObjects(const RectangleArea& area);
	std::list<LocatableObject*> getCircleAreaObjects(
		const CircleArea& area, const LocatableObject1Predict& func = alwaysTrue1);
	std::list<LocatableObject*> getCircleAreaObjects(
		LocatableObject& subject, const CircleArea& area,
		const LocatableObject2Predict& func = alwaysTrue2);

	bool isPosInFreeCell(double x, double y);

private:
	Matrix<TerrainID> m_terrain_matrix;
	// Хранит ссылки на все пространственные объекты
	list<SpaceObject*> m_space_objects; 
	// Хранит ссылки на все способные двигаться объекты
	list<SpaceObject*> m_moveable_objects;
		
	Matrix<DistrictCell> m_cells;				// Ячейки хранятся по (x, y)

	void f() {
		for (auto& x : m_cells(0, 0).getObjects()) {}
	}

	RectangleArea m_borders;

	DistrictRenderer *m_renderer;

	const TerrainMap& m_terrain_map;

	//vector<District*> m_near_districts;	// Массив указателей на области, в которые можно попасть из этой (не включая соседние)
	//vector<string> m_portal_nets;			// Имена сетей, в которые возможно перейти из текущей области
};

class DistrictRenderer : public IRendererWorld {
public:
	DistrictRenderer(District* district, int out_width, int out_height);
	void drawWorld(
		DisplaySystem& display_system,
		const DisplayObjects& object_types_textures,
		const TerrainMap& terrain_map,
		IRenderer* renderer) override;

	const int getWidth() { return m_width; }
	const int getHeight() { return m_height; }

	void setWidth(const int width) { this->m_width = width; }
	void setHeight(const int height) { this->m_height = height; }


private:
	District* m_district;
	int m_width;
	int m_height;
};
