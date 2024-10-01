#include "physics.hpp"
#include "factory/definitions.hpp"
#include "input/input.hpp"
#include "base_physics.hpp"

using namespace std;
using namespace math;

LocatableObject::LocatableObject(ObjectForm &&form)
	: m_current_district(nullptr), m_form(form), m_position(),
	m_draw_info(nullptr){
	/*if (texture != nullptr) {
		m_entity = new VertexBaseEntity();
		m_entity->createObject();
		m_entity->setTexture(texture);
	}*/
}

void LocatableObject::addDrawInfo(Texture *texture) {
	if (m_draw_info == nullptr) {
		m_draw_info = new SpaceObjectDrawInfo();
		m_draw_info->init(this, texture);
	}
	else {
		m_draw_info->setTexture(texture);
	}
}

LocatableObject::LocatableObject(LocatableObject &obj)
	: m_current_district(obj.m_current_district), m_form(obj.m_form), 
	m_position(obj.m_position) {
}

LocatableObject::~LocatableObject() {
	if (m_draw_info)
		delete m_draw_info;
}

// РЎРѕР·РґР°С‘С‚ Р»РѕРєР°Р»СЊРЅС‹Р№ РѕР±СЉРµРєС‚ СЃ СѓРєР°Р·Р°РЅРЅС‹РјРё РїР°СЂР°РјРµС‚СЂР°РјРё
SpaceObject::SpaceObject(
	bool is_moveable,
	ObjectForm&& form,
	Texture* texture,
	float rotation,
	float rotation_speed,
	float max_rotation_speed,
	Vector&& speed_direction,
	float max_speed, float acceleration)
	: LocatableObject(std::move(form)),
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
// РЎРѕР·РґР°С‘С‚ Р»РѕРєР°Р»СЊРЅС‹Р№ РѕР±СЉРµРєС‚, Р·Р°РіСЂСѓР¶Р°СЏ РµРіРѕ РёР· С„Р°Р№Р»Р°
SpaceObject::SpaceObject(FILE* f)
{
	load(f);
}
// РљРѕРїРёСЂСѓСЋС‰РёР№ РєРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ
SpaceObject::SpaceObject(SpaceObject& lo)
	: LocatableObject(lo),
	m_rotation_speed(lo.m_rotation_speed), 
	m_max_rotation_speed(lo.m_max_rotation_speed),
	m_acceleration(lo.m_acceleration),
	m_cell(nullptr),
	m_current_speed(0),
	m_is_moveable(lo.m_is_moveable),
	m_max_speed(lo.m_max_speed),
	m_speed_direction(Vector(1, 0))
{
		
}

SpaceObject::~SpaceObject() {
	removeFromDistrictList();
}

void SpaceObject::rotate(float da)
{
	m_rotation += da;
	if (m_rotation > g_pi2)
		m_rotation -= trunc(m_rotation / g_pi2) * g_pi2;
	else if (m_rotation < 0)
		m_rotation -= (trunc(m_rotation / g_pi2) - 1) * g_pi2;
}

void SpaceObject::moveTo(double x, double y)
{
	m_position.m_index.x = (int) trunc(x / g_cell_size);
	m_position.m_index.y = (int) trunc(y / g_cell_size);

	m_position.m_coords.x = x - m_position.m_index.x * g_cell_size;
	m_position.m_coords.y = y - m_position.m_index.y * g_cell_size;

	updateCell();
}

void SpaceObject::RemoveFromOldDistrict()
{
	m_cell_info.remove();
	m_district_info.remove();
	if (m_is_moveable)
		m_district_moveable_info.remove();
}

bool SpaceObject::isMoveable() {
	return m_is_moveable;
}

void SpaceObject::moveTo(District* district, double x, double y)
{
	if (m_current_district != nullptr &&
		m_current_district != district) 
		RemoveFromOldDistrict();
	
	m_current_district = district;

	m_position.setFromGlobalCoords(x, y);

	insertToDistrictList();
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

glm::vec3 SpaceObject::getRenderOrigin( ) {
	return glm::vec3(m_position.getGlobalCoords(), 0);
}

Position SpaceObject::getPosition()
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