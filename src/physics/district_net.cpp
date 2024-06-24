#include "Physics.hpp"
#include "collisions.hpp"

DistrictCell::DistrictCell()
	:m_is_border(false),
	m_index_in_district(),
	m_owner_district(nullptr),
	m_objects() {}

DistrictCell* District::getCell(int x, int y) {
	return &m_cells(y, x);
}

DistrictCell* District::getCell(ivec2 index) {
	return &m_cells(index.y, index.x);
}

void DistrictCell::init(ivec2 index_in_district) {
	m_index_in_district = index_in_district;
	m_is_border = index_in_district.x == 0 || index_in_district.y == 0 ||
		index_in_district.x == m_owner_district->m_cells.colCount() - 1 ||
		index_in_district.y == m_owner_district->m_cells.rowCount() - 1;
}

District::District(int cells_x_amount, int cells_y_amount) 
	: m_cells(cells_y_amount, cells_x_amount)
{
	for (uint32_t y = 0; y < cells_y_amount; y++) {
		for (uint32_t x = 0; x < cells_x_amount; x++) {
			m_cells(y, x).m_owner_district = this;
			m_cells(y, x).init(ivec2(y, x));
		}
	}
}

District::~District() {
	// Дополнить функцию удалением ссылок на ячейки у
	// крайних ячеек соседних областей 

	for (int i = m_space_objects.size(); i; i--)
		(*m_space_objects.begin())->removeFromDistrictList();

	for (int i = m_moveable_objeсts.size(); i; i--)
		(*m_moveable_objeсts.begin())->removeFromDistrictList();
}

bool District::isCellExist(ivec2 index) const{
	return !(index.x < 0 || index.y < 0 || 
		index.x >= getCellsXAmount() || index.y >= getCellsYAmount());
}

void District::moveObjects(float dt) {
	float col_time;

	vector<bool> m_collision_members(m_moveable_objeсts.size());
	int moveable_object_index = 0;
	int width = getCellsXAmount();
	int height = getCellsYAmount();

	for (SpaceObject* m : m_moveable_objeсts) {
		if (m->m_current_speed == 0)
			continue;
		// Нужно изменить логику и коллизить не все
		// объекты, а только те, которые находятся в 
		// соседних ячейках

		bool move_possible = true;
		ivec2 mo_cell_index = m->m_position.m_index;
		for (int x_cell = std::max(mo_cell_index.x - 1, 0),
			x_cell_end = std::min(x_cell + 3, width - 1);
			x_cell < x_cell_end; x_cell++)
			for (int y_cell = std::max(mo_cell_index.y - 1, 0),
				y_cell_end = std::min(y_cell + 3, height - 1);
				move_possible && y_cell < y_cell_end; y_cell++) {
				DistrictCell* current_cell = &m_cells(y_cell, x_cell);
				m->m_position.shiftToCoordsSystem(ivec2(x_cell, y_cell));

				for (SpaceObject* s : current_cell->m_objects)
					if (m != s) {
						if (Collisions::Collision(m, s, dt, &col_time)) {
							move_possible = false;
							m->setCurrentSpeed(0);
							if (s->isMoveable())
								s->setCurrentSpeed(0);
							break;
						}
					}
			}
		m->m_position.normalizeCoords();
		if (move_possible) {
			if (m->m_cell->m_is_border == true) {
				Position future_pos = m->getFuturePosition(dt);
				future_pos.normalizeCoords();

				if (isCellExist(future_pos.m_index)) {
					m->m_position = future_pos;
					m->updateCell();
				}
				else {
					continue;
				}
			}
			else {
				m->move(dt);
			}

			m->updateCell();
		}
	}
}
