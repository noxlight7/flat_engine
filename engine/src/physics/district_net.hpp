#pragma once

#include "space_objects.hpp"
#include <glm/glm.hpp>

// ���� ������ �������. ������ ��������� �� �������, �������� ��� ��������� ����������� �� � ����������
class DistrictCell
{
	bool m_is_border;
	glm::ivec2 m_index_in_district;

	// �������, � ������� ���������� ������
	District* m_owner_district;

	// ������ ������������ � ������ ��������
	std::list<SpaceObject*> m_objects;

	std::unique_ptr<SpaceObject> m_fill_object;
public:
	DistrictCell();

	// �������� ������������� ������, �������
	// ��������� �� �������� ������, �������������
	// ������� � ������������ � ���������� ������
	// � ������� (x; y)
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

};

/*	�������(����� ������������ � ����������� �� / � ���� �� ������������� ��� ������
	������ ���� �������� � �� ����� */
class District
{
	Matrix<TerrainID> m_terrain_matrix;
	// ������ ������ �� ��� ���������������� �������
	list<SpaceObject*> m_space_objects;
	// ������ ������ �� ��� ��������� ��������� �������
	list<SpaceObject*> m_moveable_objects;

	Matrix<DistrictCell> m_cells;				// ������ �������� �� (x, y)

	void f() {
		for (auto& x : m_cells(0, 0).getObjects()) {}
	}

	RectangleArea m_borders;

	const PhysicTerrainMap& m_terrain_map;
public:
	// �����������. ������ �������
	District(int width, int height, const PhysicTerrainMap& terrain_map);
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

	void load(FILE* f);					// ��������� ������� �� �����
	void save(FILE* f);					// ��������� ������� � ����

	void moveObjects(float dt);

	bool isCellExist(glm::ivec2 index) const;

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

	//vector<District*> m_near_districts;	// ������ ���������� �� �������, � ������� ����� ������� �� ���� (�� ������� ��������)
	//vector<string> m_portal_nets;			// ����� �����, � ������� �������� ������� �� ������� �������
};
