#include "Physics.hpp"
#include "collisions.hpp"

void DistrictCell::init(int x, int y) {
	clearBorders();
	float cell_size = m_owner_district->m_net->m_cells_size;
	
	m_borders.m_left =
		m_owner_district->m_borders.m_left + x * cell_size;

	m_borders.m_right = m_borders.m_left + cell_size;

	m_borders.m_bottom =
		m_owner_district->m_borders.m_bottom + y * cell_size;

	m_borders.m_top = m_borders.m_bottom + cell_size;

}

void DistrictCell::clearBorders(){
	this->m_is_border = false;
	memset(&m_cells, 0, sizeof(CellNeighbors));
}

District::District(DistrictNet* net, int col_index, int row_index)
	: m_net(net), m_row_index(row_index), m_col_index(col_index)
{
	uint32_t n = net->m_cells_partition;

	m_borders.m_left = col_index * net->m_district_size;
	m_borders.m_bottom = row_index * net->m_district_size;
	m_borders.m_right = m_borders.m_left + net->m_district_size;
	m_borders.m_top = m_borders.m_bottom + net->m_district_size;

	m_cells = new DistrictCell * [n * n];

	for (uint32_t i = 0; i < n; i++) {
		m_cells[i] = new DistrictCell[n];

		for (uint32_t j = 0; j < n; j++) {
			m_cells[i][j].m_owner_district = this;
			m_cells[i][j].init(i, j);
			m_cells[i][j].m_cells.titles.m_self = &m_cells[i][j];
		}
	}

	uint32_t m = n - 1;
	for (uint32_t i = 1; i < m; i++) {
		for (uint32_t j = 1; j < m; j++) {
			m_cells[i][j].m_cells.titles.m_left = &m_cells[i - 1][j];
			m_cells[i][j].m_cells.titles.m_right = &m_cells[i + 1][j];
			m_cells[i][j].m_cells.titles.m_bottom = &m_cells[i][j - 1];
			m_cells[i][j].m_cells.titles.m_top = &m_cells[i][j + 1];

			m_cells[i][j].m_cells.titles.m_left_top = &m_cells[i - 1][j + 1];
			m_cells[i][j].m_cells.titles.m_left_bottom = &m_cells[i - 1][j - 1];
			m_cells[i][j].m_cells.titles.m_right_top = &m_cells[i + 1][j + 1];
			m_cells[i][j].m_cells.titles.m_right_bottom = &m_cells[i + 1][j - 1];
		}
	}

	District* district;
	
	district = m_net->getDistrict(m_col_index - 1, m_row_index);
	for (int j = 1; j < m; j++) {
		m_cells[0][j].m_cells.titles.m_right = &m_cells[1][j];
		m_cells[0][j].m_cells.titles.m_bottom = &m_cells[0][j - 1];
		m_cells[0][j].m_cells.titles.m_top = &m_cells[0][j + 1];

		m_cells[0][j].m_cells.titles.m_right_top = &m_cells[1][j + 1];
		m_cells[0][j].m_cells.titles.m_right_bottom = &m_cells[1][j - 1];

		if (district) {
			district->m_cells[m][j].m_cells.titles.m_right = &m_cells[0][j];
			m_cells[0][j].m_cells.titles.m_left = &district->m_cells[m][j];

			m_cells[0][j].m_cells.titles.m_left_top = &district->m_cells[m][j + 1];
			m_cells[0][j].m_cells.titles.m_left_bottom = &district->m_cells[m][j - 1];

			district->m_cells[m][j].m_cells.titles.m_right_top = &m_cells[0][j + 1];
			district->m_cells[m][j].m_cells.titles.m_right_bottom = &m_cells[0][j - 1];
		}
	}

	district = m_net->getDistrict(m_col_index + 1, m_row_index);
	for (int j = 1; j < m; j++) {
		m_cells[m][j].m_cells.titles.m_left = &m_cells[m - 1][j];
		m_cells[m][j].m_cells.titles.m_bottom = &m_cells[m][j - 1];
		m_cells[m][j].m_cells.titles.m_top = &m_cells[m][j + 1];

		m_cells[m][j].m_cells.titles.m_left_top = &m_cells[m - 1][j + 1];
		m_cells[m][j].m_cells.titles.m_left_bottom = &m_cells[m - 1][j - 1];

		if (district) {
			district->m_cells[0][j].m_cells.titles.m_left = &m_cells[m][j];
			m_cells[m][j].m_cells.titles.m_right = &district->m_cells[0][j];

			m_cells[m][j].m_cells.titles.m_right_top = &district->m_cells[0][j + 1];
			m_cells[m][j].m_cells.titles.m_right_bottom = &district->m_cells[0][j - 1];

			district->m_cells[0][j].m_cells.titles.m_left_top = &m_cells[m][j + 1];
			district->m_cells[0][j].m_cells.titles.m_left_bottom = &m_cells[m][j - 1];
		}
	}

	district = m_net->getDistrict(m_col_index, m_row_index - 1);
	for (int i = 1; i < m; i++) {
		m_cells[i][0].m_cells.titles.m_left = &m_cells[i - 1][0];
		m_cells[i][0].m_cells.titles.m_right = &m_cells[i + 1][0];
		m_cells[i][0].m_cells.titles.m_top = &m_cells[i][1];

		m_cells[i][0].m_cells.titles.m_left_top = &m_cells[i - 1][1];
		m_cells[i][0].m_cells.titles.m_right_top = &m_cells[i + 1][1];
		
		if (district) {
			district->m_cells[i][m].m_cells.titles.m_top = &m_cells[i][0];
			m_cells[i][0].m_cells.titles.m_bottom = &district->m_cells[i][m];

			m_cells[i][0].m_cells.titles.m_left_bottom = &district->m_cells[i - 1][m];
			m_cells[i][0].m_cells.titles.m_right_bottom = &district->m_cells[i + 1][m];

			district->m_cells[i][m].m_cells.titles.m_left_top = &m_cells[i - 1][0];
			district->m_cells[i][m].m_cells.titles.m_right_top = &m_cells[i + 1][0];
		}
	}

	district = m_net->getDistrict(m_col_index, m_row_index + 1);
	for (int i = 1; i < m; i++) {
		m_cells[i][m].m_cells.titles.m_left = &m_cells[i - 1][m];
		m_cells[i][m].m_cells.titles.m_right = &m_cells[i + 1][m];
		m_cells[i][m].m_cells.titles.m_bottom = &m_cells[i][m - 1];

		m_cells[i][m].m_cells.titles.m_left_bottom = &m_cells[i - 1][m - 1];
		m_cells[i][m].m_cells.titles.m_right_bottom = &m_cells[i + 1][m - 1];

		if (district) {
			district->m_cells[i][0].m_cells.titles.m_bottom = &m_cells[i][n];

			m_cells[i][m].m_cells.titles.m_left_top = &district->m_cells[i - 1][0];
			m_cells[i][m].m_cells.titles.m_right_top = &district->m_cells[i + 1][0];

			district->m_cells[i][0].m_cells.titles.m_left_bottom = &m_cells[i - 1][m];
			district->m_cells[i][0].m_cells.titles.m_right_bottom = &m_cells[i + 1][m];
		}
	}

	// (0;0)
	m_cells[0][0].m_cells.titles.m_right = &m_cells[1][0];
	m_cells[0][0].m_cells.titles.m_top = &m_cells[0][1];
	m_cells[0][0].m_cells.titles.m_right_top = &m_cells[1][1];
	// (0;m)
	m_cells[0][m].m_cells.titles.m_right = &m_cells[1][m];
	m_cells[0][m].m_cells.titles.m_bottom = &m_cells[0][m - 1];
	m_cells[0][m].m_cells.titles.m_right_bottom = &m_cells[1][m - 1];
	// (m;0)
	m_cells[m][0].m_cells.titles.m_left = &m_cells[m - 1][0];
	m_cells[m][0].m_cells.titles.m_top = &m_cells[m][1];
	m_cells[m][0].m_cells.titles.m_left_top = &m_cells[m - 1][1];
	// (m;m)
	m_cells[m][m].m_cells.titles.m_left = &m_cells[m - 1][m];
	m_cells[m][m].m_cells.titles.m_bottom = &m_cells[m][m - 1];
	m_cells[m][m].m_cells.titles.m_left_bottom = &m_cells[m - 1][m - 1];

	district = m_net->getDistrict(m_col_index - 1, m_row_index);
	if (district) {
		m_cells[0][0].m_cells.titles.m_left = &district->m_cells[m][0];
		district->m_cells[m][0].m_cells.titles.m_right = &m_cells[0][0];

		m_cells[0][m].m_cells.titles.m_left = &district->m_cells[m][m];
		district->m_cells[m][m].m_cells.titles.m_right = &m_cells[0][m];
	}

	district = m_net->getDistrict(m_col_index + 1, m_row_index);
	if (district) {
		m_cells[m][0].m_cells.titles.m_right = &district->m_cells[0][0];
		district->m_cells[0][0].m_cells.titles.m_left = &m_cells[m][0];

		m_cells[m][m].m_cells.titles.m_right = &district->m_cells[0][m];
		district->m_cells[0][m].m_cells.titles.m_left = &m_cells[m][m];
	}

	district = m_net->getDistrict(m_col_index, m_row_index - 1);
	if (district) {
		m_cells[0][0].m_cells.titles.m_bottom = &district->m_cells[0][m];
		district->m_cells[0][m].m_cells.titles.m_top = &m_cells[0][0];

		m_cells[m][0].m_cells.titles.m_bottom = &district->m_cells[m][m];
		district->m_cells[m][m].m_cells.titles.m_top = &m_cells[m][0];
	}

	district = m_net->getDistrict(m_col_index, m_row_index + 1);
	if (district) {
		m_cells[0][m].m_cells.titles.m_top = &district->m_cells[0][0];
		district->m_cells[0][0].m_cells.titles.m_bottom = &m_cells[0][m];

		m_cells[m][m].m_cells.titles.m_top = &district->m_cells[m][0];
		district->m_cells[m][0].m_cells.titles.m_bottom = &m_cells[m][m];
	}

	district = m_net->getDistrict(m_col_index - 1, m_row_index - 1);
	if (district) {
		m_cells[0][0].m_cells.titles.m_left_bottom = &district->m_cells[m][m];
		district->m_cells[m][m].m_cells.titles.m_right_top = &m_cells[0][0];
	}

	district = m_net->getDistrict(m_col_index - 1, m_row_index + 1);
	if (district) {
		m_cells[0][m].m_cells.titles.m_left_top = &district->m_cells[m][0];
		district->m_cells[m][0].m_cells.titles.m_right_bottom = &m_cells[0][m];
	}

	district = m_net->getDistrict(m_col_index + 1, m_row_index - 1);
	if (district) {
		m_cells[m][0].m_cells.titles.m_right_bottom = &district->m_cells[0][m];
		district->m_cells[0][m].m_cells.titles.m_left_top = &m_cells[m][0];
	}

	district = m_net->getDistrict(m_col_index + 1, m_row_index + 1);
	if (district) {
		m_cells[m][m].m_cells.titles.m_right_top = &district->m_cells[0][0];
		district->m_cells[0][0].m_cells.titles.m_left_bottom = &m_cells[m][m];
	}

	for (uint32_t i = 0; i < n; i++)
		for (uint32_t j = 0; j < n; j++)
			for (uint32_t k = 0; k < 9; k++)
				if (m_cells[i][j].m_cells.buffer[k] == nullptr) {
					m_cells[i][j].m_is_border = true;
					break;
				}
}

District::~District() {
	// Дополнить функцию удалением ссылок на ячейки у
	// крайних ячеек соседних областей 

	for (int i = m_space_objects.size(); i; i--)
		(*m_space_objects.begin())->removeFromDistrictList();

	for (int i = m_moveable_objeсts.size(); i; i--)
		(*m_moveable_objeсts.begin())->removeFromDistrictList();

	uint32_t n = m_net->m_cells_partition;

	for (uint32_t i = 0; i < n; i++)
		delete[] m_cells[i];

	delete[] m_cells;
}

DistrictCell* District::getCell(float x, float y) {
	float cell_size = m_net->m_cells_size;
	int x_index = floor(x / cell_size);
	int y_index = floor(y / cell_size);

	return &m_cells[x_index][y_index];
}

DistrictCell* DistrictNet::getCell(float x, float y) {
	int district_x = floor(x / m_district_size);
	int district_y = floor(y / m_district_size);
	District* district = this->getDistrict(district_x, district_y);

	if (district == nullptr)
		return nullptr;
	else
		return district->getCell(
			x - district->m_borders.m_left,
			y - district->m_borders.m_bottom);
}

ivec2&& District::getCellIndex(float x, float y) {
	float m_size = m_net->m_cells_size;
	return ivec2(trunc(x / m_size), trunc(y / m_size));
}

DistrictNet::DistrictNet(
	uint32_t width,
	uint32_t height,
	float district_size,
	uint32_t cells_partion)
	: m_width(width), m_height(height),
	m_district_size(district_size), 
	m_cells_partition(cells_partion),
	m_districts_amount(0),
	m_min_load_index(),
	m_max_load_index() {

	m_cells_size = district_size / cells_partion;

	m_districts = new District * *[width];
	
	for (uint32_t i = 0; i < width; i++) {
		m_districts[i] = new District * [height];

		for (uint32_t j = 0; j < height; j++)
			m_districts[i][j] = nullptr;
	}
}

District* DistrictNet::addDistrict(uint32_t x, uint32_t y) {
	if (!m_districts_amount) {
		m_max_load_index.x = m_min_load_index.x = x;
		m_max_load_index.y = m_min_load_index.y = y;
	}

	if (m_districts[x][y] == nullptr)
		m_districts[x][y] = new District(this, y, x);

	return getDistrict(x, y);
}

District* DistrictNet::getDistrict(uint32_t x, uint32_t y) {
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return nullptr;
	else
		return m_districts[x][y];
}

void DistrictNet::moveObjects(float dt) {
	for (int i = m_min_load_index.x;
		i <= m_max_load_index.x; i++)
		for (int j = m_min_load_index.y;
			j <= m_max_load_index.y; j++) {
		District* district = m_districts[i][j];
		float col_time;

		vector<bool> m_collision_members(
			district->m_moveable_objeсts.size());
		int moveable_object_index = 0;

		if (district) {
			for (SpaceObject* m : district->m_moveable_objeсts) {
				// Нужно изменить логику и коллизить не все
				// объекты, а только те, которые находятся в 
				// соседних ячейках
				
				bool move_possible = true;
				for (int i = 0; i < 9; i++) {
					DistrictCell* current_cell = 
						m->m_cell->m_cells.buffer[i];

					if(current_cell)
						for (SpaceObject* s : 
							current_cell->m_objects)

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
				
				if (move_possible) {
					if (m->m_cell->m_is_border == true) {
						Vector future_pos = m->getFuturePosition(dt);
						DistrictCell* next_cell = getCell(future_pos.x, future_pos.y);

						if (next_cell == nullptr)
							continue;
						else
							m->m_position = future_pos;
					}
					else
						m->move(dt);
					
					m->updateCell();
				}
			}
		}
	}
}

DistrictNet::~DistrictNet() {
	for (uint32_t i = 0; i < m_width; i++) {
		for (uint32_t j = 0; j < m_height; j++)
			if (m_districts[i][j] != nullptr)
				delete m_districts[i][j];

		delete[] m_districts[i];
	}

	delete m_districts;
}