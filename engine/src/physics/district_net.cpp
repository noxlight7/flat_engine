#include "collisions.hpp"
#include "client/client_engine.hpp"
#include "display/display_objects.hpp"
#include "display/draw_info_factory.hpp"
#include "factory/definitions.hpp"
#include "district_net.hpp"

// TerrainMap::TerrainMap(): m_matrix(g_metres_in_cell, g_metres_in_cell){
// }

DistrictCell::DistrictCell()
	:m_is_border(false),
	m_index_in_district(),
	m_owner_district(nullptr),
	m_objects()
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
		index_in_district.x == m_owner_district->getCellsXAmount() - 1 ||
		index_in_district.y == m_owner_district->getCellsYAmount() - 1;
}

std::list<SpaceObject*>& DistrictCell::getInnerObjects() {
	return m_objects;
}

void DistrictCell::addFillObject(std::unique_ptr<SpaceObject> fill_object) {
	m_fill_object = std::move(fill_object);
	const auto form = m_fill_object->getForm().m_data.m_rectangle;
	form->m_height = 1.0f;
	form->m_width = 1.0f;
	m_fill_object->getPosition().m_index = m_index_in_district;
	m_fill_object->getPosition().m_coords = glm::ivec2(form->m_width / 2, form->m_height / 2);
	auto coords = m_fill_object->getPosition().getGlobalCoords();
	m_fill_object->moveTo(m_owner_district, coords.x + 0.5f, coords.y + 0.5f);
}

District::District(int cells_x_amount, int cells_y_amount, const TerrainMap& terrain_map)
	: m_cells(cells_y_amount, cells_x_amount),
	  m_terrain_matrix(cells_y_amount, cells_x_amount, 0),
	  m_terrain_map(terrain_map),
	  m_renderer(nullptr)
{
	for (uint32_t y = 0; y < cells_y_amount; y++) {
		for (uint32_t x = 0; x < cells_x_amount; x++) {
			m_cells(y, x).setOwnerDistrict(this);
			m_cells(y, x).init(glm::ivec2(y, x));
		}
	}
}

District::~District() {

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
		if (m->getCurrentSpeed() == 0)
			continue;

		bool move_possible = true;
		glm::ivec2 mo_cell_index = m->getPosition().m_index;
		float step_cost = m_terrain_map.at(m_terrain_matrix(
			mo_cell_index.y, mo_cell_index.x)).m_step_cost;
		float max_speed = m->getMaxSpeed() / step_cost;
		if (m->getCurrentSpeed() > max_speed) {
			m->setCurrentSpeed(max_speed);
		}
		for (int x_cell = std::max(mo_cell_index.x - 2, 0),
			x_cell_end = std::min(mo_cell_index.x + 3, width);
			x_cell < x_cell_end; x_cell++)
			for (int y_cell = std::max(mo_cell_index.y - 2, 0),
				y_cell_end = std::min(mo_cell_index.y + 3, height);
				move_possible && y_cell < y_cell_end; y_cell++) {
				DistrictCell* current_cell = &m_cells(y_cell, x_cell);
				m->getPosition().shiftToCoordsSystem(glm::ivec2(x_cell, y_cell));

				for (SpaceObject* s : current_cell->getObjects())
					if (m != s) {
						if (s->isMoveable() && s->getCurrentSpeed() > 0) {
							auto s_cell_indices = s->getPosition().m_index;
							float s_max_speed = s->getMaxSpeed() / m_terrain_matrix(
								s_cell_indices.y, s_cell_indices.x);
							if (s->getCurrentSpeed() > max_speed) {
								s->setCurrentSpeed(max_speed);
							}
						}
						if (Collisions::Collision(m, s, dt, &col_time)) {
							move_possible = false;
							m->setCurrentSpeed(0);
							if (s->isMoveable())
								s->setCurrentSpeed(0);
							break;
						}
					}
			}
		m->getPosition().normalizeCoords();
		if (move_possible) {
			if (m->getCurrentCell()->isBorder() == true) {
				Position future_pos = m->getFuturePosition(dt);
				future_pos.normalizeCoords();

				if (isCellExist(future_pos.m_index)) {
					m->setPosition(future_pos);
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
		m_renderer->setWidth(width);
		m_renderer->setHeight(height);
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
		m_list_it = m_district.getCell(m_current_row, m_current_col)->getObjects().begin();
	}
}

void District::RectangleAreaRange::Iterator::skipEmpty() {
	while (m_current_row <= m_row_end && m_current_col <= m_col_end &&
		m_list_it == m_district.getCell(m_current_row, m_current_col)->getObjects().end()) {
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
		m_list_it = m_district.getCell(m_current_row, m_current_col)->getObjects().begin();
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
		if (obj.getPosition().getDistance(center) <= area.m_radius && func(obj)) {
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
		if (obj.getPosition().getDistance(center) <= area.m_radius && func(subject, obj)) {
			objects.push_back(&obj);
		}
	}

	return objects;
}

bool District::isPosInFreeCell(double x, double y) {
	Position new_position;
	new_position.setFromGlobalCoords(x, y);
	return !getCell(new_position.m_index)->isFill();
}

DistrictRenderer::DistrictRenderer(District* district, int out_width, int out_height)
	: IRendererWorld(), m_district(district), m_width(out_width), m_height(out_height) {
}

void DistrictRenderer::drawWorld(DisplaySystem& display_system,
	const DisplayObjects& object_types_textures, const TerrainMap& terrain_map,
	IRenderer* renderer, uint64_t current_time) {
	auto& building_layout = display_system.getBuildingLayout();


	auto &old_camera = *dynamic_cast<Camera*>(renderer->getCurrentCamera().get());
	Camera camera(old_camera);

	auto& land_layer = building_layout[static_cast<int>(Layers::Land)];
	auto& terrain_layer = building_layout[static_cast<int>(Layers::Terrain)];
	auto render_area = camera.getVisibleRect(m_width, m_height);
	Position left_bottom = Position(render_area.m_left, render_area.m_bottom);
	Position right_top = Position(render_area.m_right, render_area.m_top);
	int x_start = std::max(0, left_bottom.m_index.x);
	int y_start = std::max(0, left_bottom.m_index.y);
	int x_end = std::min(right_top.m_index.x + 1, m_district->getCellsXAmount());
	int y_end = std::min(right_top.m_index.y + 1, m_district->getCellsYAmount());
	auto& terrain_matrix = m_district->getTerrainMatrix();
	for (int x_index = x_start; x_index < x_end; x_index++)
		for (int y_index = y_start; y_index < y_end; y_index++) {
			terrain_layer[x_index * m_district->getCellsYAmount() + y_index] = DrawInfoFactory::getTerrainDrawInfo(
				terrain_map.at(terrain_matrix(y_index, x_index)), x_index, y_index, camera);
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
	display_system.nextState(current_time);
}
