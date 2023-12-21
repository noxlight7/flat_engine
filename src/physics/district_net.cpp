#include "Physics.hpp"

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
}

DistrictCell& District::getCell(float x, float y) {
	float m_size = m_net->m_cells_size;
	return m_cells[(int) (x / m_size)][(int)(y / m_size)];
}

void District::addSpaceObject(SpaceObject *obj, float x, float y) {
	obj->moveTo(x, y);
	m_space_objects.push_back(obj);
	getCell(x, y).m_objects.push_back(obj);

	if (typeid(*obj) == typeid(MoveableObject))
		m_moveable_objeñts.push_back((MoveableObject*)obj);
}

DistrictNet::DistrictNet(
	uint32_t width,
	uint32_t height,
	float district_size,
	uint32_t cells_partion)
	: m_width(width), m_height(height),
	m_district_size(district_size), 
	m_cells_partition(cells_partion) {

	m_cells_size = district_size / cells_partion;

	m_districts = new District * *[width];
	
	for (uint32_t i = 0; i < width; i++) {
		m_districts[i] = new District * [height];

		for (uint32_t j = 0; j < height; j++)
			m_districts[i][j] = nullptr;
	}
}

District* DistrictNet::addDistrict(uint32_t x, uint32_t y) {
	if (m_districts[x][y] == nullptr)
		m_districts[x][y] = new District(this);

	return getDistrict(x, y);
}

District* DistrictNet::getDistrict(uint32_t x, uint32_t y) {
	return m_districts[x][y];
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