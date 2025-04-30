#pragma once

#include <glm/glm.hpp>

using Vector = glm::vec2;
using DVector = glm::dvec2;
//#define Vector glm::vec3

std::tuple<double, double> to_tuple(const glm::vec2& vec);

typedef struct Position
{
	Vector m_coords;			// Координаты в ячейке
	glm::ivec2 m_index;				// Индекс ячейки

	Position() = default;
	Position(Vector coords, glm::ivec2 index);
	Position(double x, double y);

	void normalizeCoords();

	[[nodiscard]] DVector getGlobalCoords() const;
	[[nodiscard]] glm::dvec3 getRenderOrigin() const;
	void setFromGlobalCoords(double x, double y);
	void shiftToCoordsSystem(glm::ivec2 cell_index);
	void shiftToCoordsSystem(Position pos);

	// Вычисляет вектор, который нужно прибавить к позиции, 
	// чтобы получить позицию объекта
	glm::vec2 getShift(Position obj);
	float getDistance(const Position& obj);
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
	FORM_RECTANGLE = COL_RECTANGLE, // Прямоугольник
	FORM_DELETED
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
	ObjectForm();
	ObjectForm(ObjectForm& form);
	explicit ObjectForm(float radius);
	ObjectForm(float width, float height);
	ObjectForm(ObjectForm&& form);
	~ObjectForm();

	void setData(float radius);
	void setData(float width, float height);
	void releaseData();

	[[nodiscard]] glm::vec2 getSize(float angle) const;
	[[nodiscard]] float getMaxSize() const;

	ObjectFormData m_data;
	ObjectFormType m_type;
};

// Круглая область
struct CircleArea
{
	glm::dvec2 m_pos;
	double m_radius;
};

// Прямоугольная область
struct RectangleArea
{
	double m_left;
	double m_right;
	double m_bottom;
	double m_top;

	RectangleArea() = default;
	RectangleArea(double left, double right, double bottom, double top);
	RectangleArea(CircleArea area);
};

// struct RectangleGlobalArea
// {
// 	double m_left;
// 	double m_right;
// 	double m_bottom;
// 	double m_top;
// };


using TerrainID = uint16_t;
using TextureID = uint16_t;
struct Terrain {
	TerrainID m_id;
	TextureID m_texture_id;
	float m_step_cost;
};