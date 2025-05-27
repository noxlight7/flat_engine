#pragma once

#include "space_objects.hpp"
#include "display/game_camera.hpp"
#include "display/display_system.hpp"
#include "display/renderer_world.hpp"
#include <glm/glm.hpp>

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

	[[nodiscard]] District* getOwnerDistrict() const { return m_owner_district; }

	District* getOwnerDistrict() { return m_owner_district; }
	void setOwnerDistrict(District* m_owner_district) { this->m_owner_district = m_owner_district; }

	[[nodiscard]] bool isFill() const { return m_fill_object != nullptr; }
	[[nodiscard]] SpaceObject& getFillObject() const { return *m_fill_object; }
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

	DistrictRenderer* m_renderer;

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
		IRenderer* renderer,
		uint64_t current_time) override;

	const int getWidth() { return m_width; }
	const int getHeight() { return m_height; }

	void setWidth(const int width) { this->m_width = width; }
	void setHeight(const int height) { this->m_height = height; }


private:
	District* m_district;
	int m_width;
	int m_height;
};