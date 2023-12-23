#include "Physics.hpp"
#include "collisions.hpp"

District::District(DistrictNet* net)
	: m_net(net)
{
	uint32_t n = net->m_cells_partition;

	m_cells = new DistrictCell * [n * n];

	for (uint32_t i = 0; i < n; i++) {
		m_cells[i] = new DistrictCell[n];

		for (uint32_t j = 0; j < n; j++)
			m_cells[i][j].m_owner_district = this;
	}
}

District::~District() {
	uint32_t n = m_net->m_cells_partition;

	for (uint32_t i = 0; i < n; i++)
		delete[] m_cells[i];

	delete[] m_cells;

	for (int i = m_space_objects.size(); i; i--)
		(*m_space_objects.begin())->removeFromDistrictList();

	for (int i = m_moveable_objeсts.size(); i; i--)
		(*m_moveable_objeсts.begin())->removeFromDistrictList();
}

DistrictCell& District::getCell(float x, float y) {
	ivec2 ind = getCellIndex(x, y);
	return m_cells[ind.x][ind.y];
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
		m_districts[x][y] = new District(this);

	return getDistrict(x, y);
}

District* DistrictNet::getDistrict(uint32_t x, uint32_t y) {
	return m_districts[x][y];
}

void DistrictNet::moveObjects(float dt) {
	for (int i = m_min_load_index.x;
		i <= m_max_load_index.x; i++)
		for (int j = m_min_load_index.y;
			j <= m_max_load_index.y; j++) {
		District* district = m_districts[i][j];
		float col_time;

		if (district) {
			for (MoveableObject* m : district->m_moveable_objeсts) {
				// Нужно изменить логику и коллизить не все
				// объекты, а только те, которые находятся в 
				// соседних ячейках
				bool move_possible = true;
				for (SpaceObject* s : district->m_space_objects)
					if (m != s)
						if (Collisions::Collision(m, s, dt, &col_time)) {
							move_possible = false;
							break;
						}
				
				if (move_possible)
					m->move(dt);
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