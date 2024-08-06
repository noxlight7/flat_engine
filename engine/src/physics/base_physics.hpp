#pragma once

#include <glm/glm.hpp>

using namespace glm;

using Vector = vec2;
using DVector = dvec2;
//#define Vector glm::vec3

std::tuple<double, double> to_tuple(const glm::vec2& vec);

typedef struct Position
{
public:
	Vector m_coords;			// Координаты в ячейке
	ivec2 m_index;				// Индекс ячейки

	Position() = default;
	Position(Vector coords, ivec2 index);
	Position(double x, double y);

	void normalizeCoords();

	DVector getGlobalCoords() const;
	void setFromGlobalCoords(double x, double y);
	void shiftToCoordsSystem(ivec2 cell_index);
	void shiftToCoordsSystem(Position pos);

	// Вычисляет вектор, который нужно прибавить к позиции, 
	// чтобы получить позицию объекта
	vec2 getShift(Position obj);
} Position;

// Форма объектов, участвующих в столкновении
enum CollisionType
{
	COL_LINE,					// Линия
	COL_CIRCLE,					// Круг
	COL_RECTANGLE,				// Прямоугольник
};

// Форма объекта
enum ObjectFormType
{
	FORM_CIRCLE = COL_CIRCLE,		// Круг
	FORM_RECTANGLE = COL_RECTANGLE// Прямоугольник
};

struct CircleFormData
{
	float m_radius;
};

struct RectangleFormData
{
	float m_height;
	float m_width;
};

union ObjectFormData
{
	CircleFormData* m_circle;
	RectangleFormData* m_rectangle;
};

struct ObjectForm
{
public:
	ObjectForm();
	ObjectForm(ObjectForm& form);
	ObjectForm(float radius);
	ObjectForm(float width, float height);
	~ObjectForm();

	void setData(float radius);
	void setData(float width, float height);
	void releaseData();

	vec2&& getSize(float angle);
	float getMaxSize();

	ObjectFormData m_data;
	ObjectFormType m_type;
};

// Круглая область
struct CircleArea
{
	glm::vec2 m_pos;
	float m_radius;
};

// Прямоугольная область
struct RectangleArea
{
	float m_left;
	float m_right;
	float m_bottom;
	float m_top;
};

struct RectangleGlobalArea
{
	double m_left;
	double m_right;
	double m_bottom;
	double m_top;
};