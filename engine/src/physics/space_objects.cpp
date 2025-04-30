#include "physics.hpp"
#include "factory/definitions.hpp"
#include "input/input.hpp"
#include "base_physics.hpp"

uint32_t LocatableObject::c_next_id = 1;

LocatableObject::LocatableObject(LocatableObject&& lo) noexcept
	: BaseEntity(std::move(lo)), m_form(std::move(lo.m_form)) {
	m_position = lo.m_position;
	m_rotation = lo.m_rotation;
	m_current_district = lo.m_current_district;
	m_type_id = lo.m_type_id;
}

LocatableObject::LocatableObject(uint32_t id, uint32_t type_id, ObjectForm& form)
	: BaseEntity(id), m_current_district(nullptr), m_form(form), m_position(),
	  m_type_id(type_id) {
	/*if (texture != nullptr) {
		m_entity = new VertexBaseEntity();
		m_entity->createObject();
		m_entity->setTexture(texture);
	}*/
}

LocatableObject::LocatableObject(PoolID* pool_id, uint32_t type_id, ObjectForm& form)
	: BaseEntity(pool_id), m_current_district(nullptr), m_form(form), m_position(),
	  m_type_id(type_id) {
}

LocatableObject::LocatableObject(uint32_t id, LocatableObject &obj)
	: BaseEntity(id), m_position(obj.m_position), m_form(obj.m_form),
	  m_current_district(obj.m_current_district), m_type_id(obj.m_type_id) {
}

LocatableObject::LocatableObject(PoolID*pool_id, LocatableObject&obj)
	: BaseEntity(pool_id), m_position(obj.m_position), m_form(obj.m_form),
	  m_current_district(obj.m_current_district), m_type_id(obj.m_type_id) {
}

LocatableObject::~LocatableObject() = default;

// РЎРѕР·РґР°С‘С‚ Р»РѕРєР°Р»СЊРЅС‹Р№ РѕР±СЉРµРєС‚ СЃ СѓРєР°Р·Р°РЅРЅС‹РјРё РїР°СЂР°РјРµС‚СЂР°РјРё
SpaceObject::SpaceObject(
	PoolID* pool_id,
	bool is_moveable,
	uint32_t type_id,
	ObjectForm& form,
	float rotation,
	float rotation_speed,
	float max_rotation_speed,
	Vector&& speed_direction,
	float max_speed, float acceleration)
	: LocatableObject(pool_id, type_id, form),
	m_is_moveable(is_moveable),
	m_rotation_speed(rotation_speed), 
	m_max_rotation_speed(max_rotation_speed),
	m_speed_direction(speed_direction),
	m_current_speed(0),
	m_max_speed(max_speed),
	m_acceleration(acceleration),
	m_cell(nullptr)
{
}

SpaceObject::SpaceObject(
	uint32_t id,
	bool is_moveable,
	uint32_t type_id,
	ObjectForm& form,
	float rotation,
	float rotation_speed,
	float max_rotation_speed,
	Vector&& speed_direction,
	float max_speed, float acceleration)
	: LocatableObject(id, type_id, form),
	m_is_moveable(is_moveable),
	m_rotation_speed(rotation_speed),
	m_max_rotation_speed(max_rotation_speed),
	m_speed_direction(speed_direction),
	m_current_speed(0),
	m_max_speed(max_speed),
	m_acceleration(acceleration),
	m_cell(nullptr) {
}

// РЎРѕР·РґР°С‘С‚ Р»РѕРєР°Р»СЊРЅС‹Р№ РѕР±СЉРµРєС‚, Р·Р°РіСЂСѓР¶Р°СЏ РµРіРѕ РёР· С„Р°Р№Р»Р°
// SpaceObject::SpaceObject(FILE* f)
// {
// 	load(f);
// }
// РљРѕРїРёСЂСѓСЋС‰РёР№ РєРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ
SpaceObject::SpaceObject(PoolID* pool_id, SpaceObject& lo)
	: LocatableObject(pool_id, lo),
	m_is_moveable(lo.m_is_moveable),
	m_rotation_speed(lo.m_rotation_speed),
	m_max_rotation_speed(lo.m_max_rotation_speed),
	m_current_speed(0),
	m_speed_direction(Vector(1, 0)),
	m_max_speed(lo.m_max_speed),
	m_acceleration(lo.m_acceleration),
	m_cell(nullptr)
{
		
}

SpaceObject::SpaceObject(const uint32_t id, SpaceObject& lo)
	: LocatableObject(id, lo),
	  m_is_moveable(lo.m_is_moveable),
	  m_rotation_speed(lo.m_rotation_speed),
	  m_max_rotation_speed(lo.m_max_rotation_speed),
	  m_current_speed(0),
	  m_speed_direction(Vector(1, 0)),
	  m_max_speed(lo.m_max_speed),
	  m_acceleration(lo.m_acceleration),
	  m_cell(nullptr){
}

SpaceObject::SpaceObject(SpaceObject&& lo) noexcept : LocatableObject(std::move(lo)) {
	m_cell = lo.m_cell;
	m_cell_info = lo.m_cell_info;
	m_acceleration = lo.m_acceleration;
	m_current_speed = lo.m_current_speed;
	m_speed_direction = lo.m_speed_direction;
	m_max_speed = lo.m_max_speed;
	m_is_moveable = lo.m_is_moveable;
	m_rotation_speed = lo.m_rotation_speed;
	m_max_rotation_speed = lo.m_max_rotation_speed;
}

SpaceObject::~SpaceObject() {
	if (getID())
		removeFromDistrictList();
}

void LocatableObject::rotate(float da)
{
	setRotation(m_rotation + da);
}

void LocatableObject::setRotation(float angle) {
	float normalizedAngle = std::fmod(angle, math::g_pi2);
	if (normalizedAngle < 0)
		normalizedAngle += math::g_pi2;

	if (m_form.m_type == FORM_RECTANGLE) {
		const float step = math::g_pi2 / 4;
		normalizedAngle = std::round(normalizedAngle / step) * step;

		if (normalizedAngle >= math::g_pi2)
			normalizedAngle -= math::g_pi2;
	}

	m_rotation = normalizedAngle;
}

void SpaceObject::moveTo(double x, double y)
{
	m_position.setFromGlobalCoords(x, y);

	updateCell();
}

void SpaceObject::RemoveFromOldDistrict()
{
	m_cell_info.remove();
	m_district_info.remove();
	if (m_is_moveable)
		m_district_moveable_info.remove();
}

bool SpaceObject::isMoveable() const {
	return m_is_moveable;
}

void SpaceObject::moveTo(District* district, double x, double y)
{
	if (district->isPosInFreeCell(x, y)) {
		if (m_current_district != nullptr &&
			m_current_district != district)
			RemoveFromOldDistrict();

		m_current_district = district;

		m_position.setFromGlobalCoords(x, y);

		insertToDistrictList();
		updateCell();
	}
}

void SpaceObject::load(FILE* f)
{
	
}

void SpaceObject::save(FILE* f)
{
	
}

void SpaceObject::initInCell(DistrictCell* cell) {
	m_cell_info.remove();
	m_cell_info.insert(&cell->m_objects, this);
	m_cell = cell;
}

void SpaceObject::insertToDistrictList()
{
	District* v_district = m_current_district;

	m_district_info.insert(
		&v_district->m_space_objects, this);

	initInCell(v_district->getCell(m_position.m_index));

	if (m_is_moveable)
		m_district_moveable_info.insert(
			&v_district->m_moveable_objects, this);
}

void SpaceObject::removeFromDistrictList() {
	m_district_info.remove();

	m_cell_info.remove();

	if (m_is_moveable)
		m_district_moveable_info.remove();
}

template <typename Type>
ListElementMatrix<Type>::ListElementMatrix(int row_amount, int column_amount)
	: m_row_amount(row_amount), m_column_amount(column_amount)
{
	m_buffer = new ListPElementInfo<Type>**[row_amount];

	for (int i = 0; i < row_amount; i++)
	{
		m_buffer[i] = new ListPElementInfo<Type>*[column_amount];

		for (int j = 0; j < column_amount; j++)
			m_buffer[i][j] = new ListPElementInfo<Type>();
	}
}

template <typename Type>
ListElementMatrix<Type>::~ListElementMatrix()
{
	for (int i = 0; i < m_row_amount; i++)
	{
		for (int j = 0; j < m_column_amount; j++)
		{
			m_buffer[i][j]->remove();
			delete[] m_buffer[i][j];
		}

		delete[] m_buffer[i];
	}

	delete[] m_buffer;
}

template <typename Type>
void ListElementMatrix<Type>::shiftLeft()
{
	int n = m_column_amount - 1;
	for (int i = 0; i < m_row_amount; i++)
		m_buffer[i][0]->remove();

	for (int i = 0; i < m_row_amount; i++)
		for (int j = 0; j < n; j++)
			m_buffer[i][j] = m_buffer[i][j + 1];

	for (int i = 0; i < m_row_amount; i++)
		m_buffer[i][n] = nullptr;

}

template <typename Type>
void ListElementMatrix<Type>::shiftRight()
{

}

template <typename Type>
void ListElementMatrix<Type>::shiftTop()
{

}

template <typename Type>
void ListElementMatrix<Type>::shiftBottom()
{

}

template <typename Type>
ListPElementInfo<Type>*
ListElementMatrix<Type>::getInfo(int row, int column)
{
	return m_buffer[row][column];
}

template <typename Type>
ListPElementInfo<Type>::ListPElementInfo() 
	: m_plist(nullptr)
{
}

template <typename Type>
ListPElementInfo<Type>::~ListPElementInfo()
{
	remove();
}

template <typename Type>
void ListPElementInfo<Type>::insert(list<Type*>* plist, Type* pval)
{
	if (m_plist != nullptr)
		remove();

	m_plist = plist;
	m_iterator = plist->insert(plist->end(), pval);
}

template <typename Type>
void ListPElementInfo<Type>::remove()
{
	if (m_plist != nullptr)
	{
		m_plist->erase(m_iterator);
		m_plist = nullptr;
	}
}

template <typename Type>
Type* ListPElementInfo<Type>::getElement()
{
	if (m_plist == nullptr)
		return nullptr;

	return *m_iterator;
}

void SpaceObject::move(float dt)
{
	m_position.m_coords += dt * m_current_speed * m_speed_direction;
}

void SpaceObject::setSpeedDirection(Vector& speed_direction)
{
	if (speed_direction.x == 0 && speed_direction.y == 0)
		return;

	m_speed_direction = speed_direction;

	m_speed_direction = glm::normalize(m_speed_direction);
}

void SpaceObject::setCurrentSpeed(float current_speed)
{
	if (m_current_speed > m_max_speed)
		m_current_speed = m_max_speed;

	m_current_speed = current_speed;
}

void SpaceObject::setMaxSpeed(float max_speed) {
	m_max_speed = max_speed;
}

glm::dvec3 LocatableObject::getRenderOrigin( ) const {
	return {m_position.getGlobalCoords(), 0};
}

Position& LocatableObject::getPosition()
{
	return m_position;
}

Vector SpaceObject::getFutureCoords(float dt)
{
	return m_position.m_coords + dt * m_current_speed * m_speed_direction;
}

Position SpaceObject::getFuturePosition(float dt) {
	Position fpos(getFutureCoords(dt), m_position.m_index);
	return fpos;
}

void SpaceObject::updateCell() {
	DistrictCell* cell = m_cell;
	m_position.normalizeCoords();

	if (m_cell->m_index_in_district != m_position.m_index)
		initInCell(&m_current_district->m_cells(m_position.m_index.y,
			m_position.m_index.x));
}

std::list<LocatableObject*> Actor::getAreaInterestObjects() {
	auto cell_indices = m_physical_body->getPosition().getGlobalCoords();
	return m_physical_body->getCurrentCell()->getOwnerDistrict()->getCircleAreaObjects(
		*m_physical_body,
		CircleArea(
			glm::dvec2(cell_indices.x, cell_indices.y),
			getAreaInterestRadius()));
}