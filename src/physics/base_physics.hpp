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
	Vector m_coords;			// ���������� � ������
	ivec2 m_index;				// ������ ������

	Position() = default;
	Position(Vector coords, ivec2 index);
	Position(double x, double y);

	void normalizeCoords();

	DVector getGlobalCoords() const;
	void setFromGlobalCoords(double x, double y);
	void shiftToCoordsSystem(ivec2 cell_index);
	void shiftToCoordsSystem(Position pos);

	// ��������� ������, ������� ����� ��������� � �������, 
	// ����� �������� ������� �������
	vec2 getShift(Position obj);
} Position;

// ����� ��������, ����������� � ������������
enum CollisionType
{
	COL_LINE,					// �����
	COL_CIRCLE,					// ����
	COL_RECTANGLE,				// �������������
};

// ����� �������
enum ObjectFormType
{
	FORM_CIRCLE = COL_CIRCLE,		// ����
	FORM_RECTANGLE = COL_RECTANGLE// �������������
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

// ������� �������
struct CircleArea
{
	glm::vec2 m_pos;
	float m_radius;
};

// ������������� �������
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