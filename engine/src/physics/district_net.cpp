#include "physics.hpp"
#include "collisions.hpp"
#include "display/display_objects.hpp"
#include "display/draw_info_factory.hpp"
#include "factory/definitions.hpp"

TerrainMap::TerrainMap(): m_matrix(g_metres_in_cell, g_metres_in_cell){
}

DistrictCell::DistrictCell()
	:m_is_border(false),
	m_index_in_district(),
	m_owner_district(nullptr),
	m_objects(),
	m_map()
{}

DistrictCell* District::getCell(int x, int y) {
	return &m_cells(y, x);
}

DistrictCell* District::getCell(glm::ivec2 index) {
	return &m_cells(index.y, index.x);
}

void DistrictCell::init(glm::ivec2 index_in_district) {
	m_index_in_district = index_in_district;
	m_is_border = index_in_district.x == 0 || index_in_district.y == 0 ||
		index_in_district.x == m_owner_district->m_cells.colCount() - 1 ||
		index_in_district.y == m_owner_district->m_cells.rowCount() - 1;
}

std::list<SpaceObject*>& DistrictCell::getInnerObjects() {
	return m_objects;
}

District::District(int cells_x_amount, int cells_y_amount) 
	: m_cells(cells_y_amount, cells_x_amount), m_renderer(nullptr)
{
	for (uint32_t y = 0; y < cells_y_amount; y++) {
		for (uint32_t x = 0; x < cells_x_amount; x++) {
			m_cells(y, x).m_owner_district = this;
			m_cells(y, x).init(glm::ivec2(y, x));
		}
	}
}

District::~District() {
	// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ
	// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ 

	for (int i = m_space_objects.size(); i; i--)
		(*m_space_objects.begin())->removeFromDistrictList();

	for (int i = m_moveable_objects.size(); i; i--)
		(*m_moveable_objects.begin())->removeFromDistrictList();

	if (m_renderer)
		delete m_renderer;
}

bool District::isCellExist(glm::ivec2 index) const{
	return !(index.x < 0 || index.y < 0 || 
		index.x >= getCellsXAmount() || index.y >= getCellsYAmount());
}

void District::moveObjects(float dt) {
	float col_time;

	vector<bool> m_collision_members(m_moveable_objects.size());
	int moveable_object_index = 0;
	int width = getCellsXAmount();
	int height = getCellsYAmount();

	for (SpaceObject* m : m_moveable_objects) {
		if (m->m_current_speed == 0)
			continue;
		// пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅ
		// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ, пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ 
		// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ

		bool move_possible = true;
		glm::ivec2 mo_cell_index = m->m_position.m_index;
		for (int x_cell = std::max(mo_cell_index.x - 1, 0),
			x_cell_end = std::min(x_cell + 3, width);
			x_cell < x_cell_end; x_cell++)
			for (int y_cell = std::max(mo_cell_index.y - 1, 0),
				y_cell_end = std::min(y_cell + 3, height);
				move_possible && y_cell < y_cell_end; y_cell++) {
				DistrictCell* current_cell = &m_cells(y_cell, x_cell);
				m->m_position.shiftToCoordsSystem(glm::ivec2(x_cell, y_cell));

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

void District::setRenderer(int width, int height) {
	if (m_renderer == nullptr)
		m_renderer = new DistrictRenderer(this, width, height);
	else {
		m_renderer->m_height = height;
		m_renderer->m_width = width;
	}
}

void District::RectangleAreaRange::Iterator::advanceCell() {
	if (m_current_col < m_col_end) {
		++m_current_col;
	} else {
		++m_current_row;
		if (m_current_row <= m_row_end) {
			m_current_col = m_col_start;
		}
	}
	if (m_current_row <= m_row_end && m_current_col <= m_col_end) {
		m_list_it = m_district.getCell(m_current_row, m_current_col)->m_objects.begin();
	}
}

void District::RectangleAreaRange::Iterator::skipEmpty() {
	while (m_current_row <= m_row_end && m_current_col <= m_col_end &&
		m_list_it == m_district.getCell(m_current_row, m_current_col)->m_objects.end()) {
		advanceCell();
	}
}

District::RectangleAreaRange::Iterator::Iterator(District& district,
                                              int row_start, int col_start,
                                              int row_end, int col_end, bool end)
	: m_district(district), m_row_start(row_start), m_row_end(row_end),
	  m_col_start(col_start), m_col_end(col_end),
	  m_current_row(row_start), m_current_col(col_start){
	if (end) {
		m_current_row = row_end + 1;
		m_current_col = col_end;
	}
	if (m_current_row <= m_row_end && m_current_col <= m_col_end) {
		m_list_it = m_district.getCell(m_current_row, m_current_col)->m_objects.begin();
		skipEmpty();
	}
}

bool District::RectangleAreaRange::Iterator::operator!=(const Iterator& other) const {
	return m_current_row != other.m_current_row ||
		m_current_col != other.m_current_col ||
			m_list_it != other.m_list_it;
}

SpaceObject& District::RectangleAreaRange::Iterator::operator*() const {
	return **m_list_it;
}

District::RectangleAreaRange::Iterator& District::RectangleAreaRange::Iterator::operator++() {
	++m_list_it;
	skipEmpty();
	return *this;
}

District::RectangleAreaRange::RectangleAreaRange(District& district, int row_start, int col_start, int row_end,
	int col_end)
	: m_district(district), m_row_start(row_start), m_row_end(row_end),
	  m_col_start(col_start), m_col_end(col_end) {
}

District::RectangleAreaRange::Iterator District::RectangleAreaRange::begin() {
	return {m_district, m_row_start, m_col_start, m_row_end, m_col_end};
}

District::RectangleAreaRange::Iterator District::RectangleAreaRange::end() {
	return {m_district, m_row_start, m_col_start, m_row_end, m_col_end, true};
}

District::RectangleAreaRange District::getRectangleAreaObjects(
	const RectangleArea& area) {
	auto start = Position(area.m_left, area.m_bottom).m_index;
	auto end = Position(area.m_right, area.m_top).m_index;
	return {*this, start.y, start.x, end.y, end.x};
}

std::list<LocatableObject*> District::getCircleAreaObjects(
	const CircleArea& area, const LocatableObject1Predict& func) {
	list<LocatableObject*> objects;
	RectangleArea rect(area);
	Position center = {area.m_pos.x, area.m_pos.y};
	for (auto& obj: getRectangleAreaObjects(rect)) {
		if (obj.m_position.getDistance(center) <= area.m_radius && func(obj)) {
			objects.push_back(&obj);
		}
	}

	return objects;
}

std::list<LocatableObject*> District::getCircleAreaObjects(
	LocatableObject& subject, const CircleArea& area, const LocatableObject2Predict& func) {
	list<LocatableObject*> objects;
	RectangleArea rect(area);
	Position center = {area.m_pos.x, area.m_pos.y};
	for (auto& obj: getRectangleAreaObjects(rect)) {
		if (obj.m_position.getDistance(center) <= area.m_radius && func(subject, obj)) {
			objects.push_back(&obj);
		}
	}

	return objects;
}

DistrictRenderer::DistrictRenderer(District* district, int out_width, int out_height)
	: IRendererWorld(), m_district(district), m_width(out_width), m_height(out_height) {
}

void DistrictRenderer::drawWorld(DisplaySystem& display_system,
	const DisplayObjects& object_types_textures,
	IRenderer* renderer) {
	auto& building_layout = display_system.getBuildingLayout();

	auto &camera = *dynamic_cast<Camera*>(renderer->getCurrentCamera().get());

	auto& land_layer = building_layout[static_cast<int>(Layers::Land)];
	auto render_area = camera.getVisibleRect(m_width, m_height);
	Position left_bottom = Position(render_area.m_left, render_area.m_bottom);
	Position right_top = Position(render_area.m_right, render_area.m_top);
	int x_start = std::max(0, left_bottom.m_index.x);
	int y_start = std::max(0, left_bottom.m_index.y);
	int x_end = std::min(right_top.m_index.x + 1, m_district->getCellsXAmount());
	int y_end = std::min(right_top.m_index.y + 1, m_district->getCellsYAmount());
	for (int x_index = x_start; x_index < x_end; x_index++)
		for (int y_index = y_start; y_index < y_end; y_index++) {
			for (auto &draw_obj : m_district->getCell(x_index, y_index)->getInnerObjects()) {
				auto draw_info = DrawInfoFactory::getSpaceObjectDrawInfo(
					*draw_obj, camera, object_types_textures);
				auto id = draw_obj->getID();
				land_layer[id] = draw_info;
				// auto info = draw_obj->getDrawInfo();
				// info->setOrigin(vec3(g_camera->getOrigin().getShift(
				// 	draw_obj->getPosition()), 0.0f));
				// info->draw();
			}
		}
	display_system.nextState();
}
