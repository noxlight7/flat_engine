#include "physics.hpp"
#include "factory/definitions.hpp"
#include "input/input.hpp"

using namespace std;
using namespace math;

void Position::normalizeCoords() {
	int x_shift = (int)floorf(m_coords.x / g_cell_size);
	int y_shift = (int)floorf(m_coords.y / g_cell_size);
	m_coords.x -= x_shift * g_cell_size;
	m_coords.y -= y_shift * g_cell_size;
	m_index.x += x_shift;
	m_index.y += y_shift;
}

DVector Position::getGlobalCoords() {
	return DVector(
		m_coords.x + m_index.x * g_cell_size,
		m_coords.y + m_index.y * g_cell_size
	);
}

void Position::setFromGlobalCoords(double x, double y) {
	m_index.x = (int)floor(x / g_cell_size);
	m_index.y = (int)floor(y / g_cell_size);

	m_coords.x = x - m_index.x * g_cell_size;
	m_coords.y = y - m_index.y * g_cell_size;

}

void Position::shiftToCoordsSystem(ivec2 cell_index) {
	m_coords.x += (m_index.x - cell_index.x) * g_cell_size;
	m_coords.y += (m_index.y - cell_index.y) * g_cell_size;
	m_index = cell_index;
}

void Position::shiftToCoordsSystem(Position pos) {
	shiftToCoordsSystem(pos.m_index);
}

ObjectForm::ObjectForm(ObjectForm& form)
{
	m_type = form.m_type;
	switch (m_type)
	{
	case FORM_CIRCLE:
		m_data.m_circle = new CircleFormData();
		m_data.m_circle->m_radius = form.m_data.m_circle->m_radius;
		break;
	case FORM_RECTANGLE:
		m_data.m_rectangle = new RectangleFormData();
		m_data.m_rectangle->m_width = form.m_data.m_rectangle->m_width;
		m_data.m_rectangle->m_height = form.m_data.m_rectangle->m_height;
		break;
	default:
		break;
	}
}

ObjectForm::ObjectForm(float radius)
{
	m_type = ObjectFormType::FORM_CIRCLE;
	m_data.m_circle = new CircleFormData();
	m_data.m_circle->m_radius = radius;
}
ObjectForm::ObjectForm(float width, float height)
{
	m_type = ObjectFormType::FORM_RECTANGLE;
	m_data.m_rectangle = new RectangleFormData();
	m_data.m_rectangle->m_width = width;
	m_data.m_rectangle->m_height = height;
}
ObjectForm::ObjectForm()
{
	m_data.m_circle = nullptr;
	m_type = ObjectFormType::FORM_CIRCLE;
}

void ObjectForm::setData(float radius)
{
	if (m_type == FORM_RECTANGLE)
	{
		releaseData();
		m_type = ObjectFormType::FORM_CIRCLE;
		m_data.m_circle = new CircleFormData();
	}
	m_data.m_circle->m_radius = radius;
}

void ObjectForm::setData(float width, float height)
{
	if (m_type == FORM_CIRCLE)
	{
		releaseData();
		m_data.m_rectangle = new RectangleFormData();
	}
	m_data.m_rectangle->m_width = width;
	m_data.m_rectangle->m_height = height;
}

void ObjectForm::releaseData()
{
	switch (m_type)
	{
	case FORM_CIRCLE:
		if (m_data.m_circle != nullptr)
			delete m_data.m_circle;
		break;
	case FORM_RECTANGLE:
		if (m_data.m_rectangle != nullptr)
			delete m_data.m_rectangle;
		break;
	default:
		break;
	}
}

vec2&& ObjectForm::getSize(float angle)
{
	vec2 size{};
	switch (m_type)
	{
		case ObjectFormType::FORM_CIRCLE:
			size.x = size.y = m_data.m_circle->m_radius * 2;
			break;
		case ObjectFormType::FORM_RECTANGLE:
			float x = m_data.m_rectangle->m_width;
			float y = m_data.m_rectangle->m_height;
			size.x = x * cos(angle) + y * sin(angle);
			size.y = y * cos(angle) + x * sin(angle);
			break;
	}
	return move(size);
}

float ObjectForm::getMaxSize()
{
	float res = 0;
	switch (m_type)
	{
	case ObjectFormType::FORM_CIRCLE:
		res = m_data.m_circle->m_radius * 2;
		break;
	case ObjectFormType::FORM_RECTANGLE:
		float x = m_data.m_rectangle->m_width;
		float y = m_data.m_rectangle->m_height;
		res = sqrt(x * x + y * y);
		break;
	}
	return res;
}

ObjectForm::~ObjectForm()
{
	releaseData();
}

LocatableObject::LocatableObject(ObjectForm &&form)
	: m_current_district(nullptr), m_form(form), m_rotation(0), m_position(){
}

LocatableObject::LocatableObject(LocatableObject &obj)
	: m_current_district(obj.m_current_district), m_form(obj.m_form), 
	m_rotation(obj.m_rotation), m_position(obj.m_position) {
}

// Создаёт локальный объект с указанными параметрами
SpaceObject::SpaceObject(
	bool is_moveable,
	ObjectForm&& form,
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
// Создаёт локальный объект, загружая его из файла
SpaceObject::SpaceObject(FILE* f)
{
	load(f);
}
// Копирующий конструктор
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

void SpaceObject::initInNewDistrict()
{
	// Сюда добавить код создания матрицы информации о списках
	// m_matrix_info = new ListElementMatrix<SpaceObject>(10, 10);
	// m_matrix_info->shiftLeft();
}

bool SpaceObject::isMoveable() {
	return m_is_moveable;
}

void SpaceObject::moveTo(District* district, double x, double y)
{
	if (m_current_district == nullptr ||
		m_current_district != district) 
		initInNewDistrict();
	
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
			&v_district->m_moveable_objeсts, this);
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
	Position fpos = { getFutureCoords(dt), m_position.m_index };
	return fpos;
}

void SpaceObject::updateCell() {
	DistrictCell* cell = m_cell;
	m_position.normalizeCoords();

	if (m_cell->m_index_in_district != m_position.m_index)
		initInCell(&m_current_district->m_cells(m_position.m_index.y,
			m_position.m_index.x));
}