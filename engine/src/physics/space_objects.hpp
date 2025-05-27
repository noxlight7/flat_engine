#pragma once

#include "utils/common.hpp"
#include "entities/base_entity.hpp"
#include <list>
#include "base_physics.hpp"
#include "../texts/texts.hpp"
#include "utils/constants.hpp"
#include "factory/definitions.hpp"

class District;
class DistrictCell;
class DistrictRenderer;
class SpaceObject;
class LocatableObject;
class Actor;

// ������������ ����� ���� �� ��������� �� ������ �
// ���������, ������������ �� ���������� ������� ������
template <typename Type>
class ListPElementInfo
{
public:
	ListPElementInfo();
	// ��������� ��������� �� ������� pval � ������ m_plist. 
	// ���� ������� ����� �������� ������ ������� � ������
	// ������, ��� �������������� ������� ���� ��������� �� 
	// ������� �� ����������� ������ 
	void insert(list<Type*>* plist, Type* pval);
	// ������� ��������� �� ������� �� ������, � ������� ��� 
	// ��� �������� �������� insert
	void remove();
	// ���������� ����� ������������ �������� insert ��������
	Type* getElement();
	virtual ~ListPElementInfo();

private:
	list<Type*>* m_plist;
	typename list<Type*>::iterator m_iterator;
};

template <typename Type>
class ListElementMatrix {
public:
	ListElementMatrix(int row_amount, int column_amount);
	~ListElementMatrix();

	void shiftLeft();
	void shiftRight();
	void shiftTop();
	void shiftBottom();

	ListPElementInfo<Type>* getInfo(int row, int column);

private:
	ListPElementInfo<Type>*** m_buffer;
	int m_column_amount;
	int m_row_amount;
};

constexpr float k_rotation_up = 0.f;
constexpr float k_rotation_left = math::g_pi / 2.f;
constexpr float k_rotation_down = math::g_pi;
constexpr float k_rotation_right = math::g_pi * 3.f / 2.f;

// ����������� ������. ����� x, y ����������
class LocatableObject : public BaseEntity {
public:
	LocatableObject() = delete;
	LocatableObject(LocatableObject&) = delete;
	LocatableObject(LocatableObject&&) noexcept;
	explicit LocatableObject(uint32_t id, uint32_t type_id, ObjectForm& form);
	explicit LocatableObject(PoolID* pool_id, uint32_t type_id, ObjectForm& form);
	LocatableObject(uint32_t id, LocatableObject&);
	LocatableObject(PoolID* pool_id, LocatableObject&);
	~LocatableObject() override;

	// ��������� ������� ������ � ������������ � ������������
	// void normalizeCoords();

	ObjectForm& getForm() { return m_form; }

	[[nodiscard]] uint32_t getTypeID() const { return m_type_id; }
	// [[nodiscard]] Position& getPosition() { return m_position; }

	void rotate(float da);				// ������� ������ �� da ������
	void setRotation(float angle);
	[[nodiscard]] float getRotation() const { return m_rotation; }

	Position& getPosition();					// ���������� ������ �������
	void setPosition(Position pos) { this->m_position = pos; }
	[[nodiscard]] glm::dvec3 getRenderOrigin() const;

	float getRotation() { return m_rotation; }

protected:
	float m_rotation{};				// ���� ��������
	Position m_position{};			// ������� �������
	ObjectForm m_form{};				// ������ � ����� �������
	District* m_current_district{};	// �������, � ������� ��������� ������
	uint32_t m_type_id{};				// ������������� ���� �������

private:
	static uint32_t c_next_id;	// ��������� ������������� �������
	static uint32_t nextID() { return c_next_id++; }
};

using LocatableObject2Predict = std::function<bool(
	LocatableObject& subject, LocatableObject& object)>;
using LocatableObject1Predict = std::function<bool(LocatableObject& object)>;

inline bool alwaysTrue2(LocatableObject& subject, LocatableObject& object) { return true; }
inline bool alwaysTrue1(LocatableObject& obj) { return true; }

// ������ �������, ����� x, y ���������� � ������
class SpaceObject : public LocatableObject
{
public:
	SpaceObject() = delete;
	SpaceObject(SpaceObject&) = delete;
	// ������ ��������� ������ � ���������� �����������
	SpaceObject(
		PoolID* pool_id,
		bool is_moveable,
		uint32_t type_id,
		ObjectForm& form,
		float rotation = 0.f,
		float rotation_speed = 0.f,
		float max_rotation_speed = 0.f,
		Vector&& speed_direction = Vector(1.f, 0.f),
		float max_speed = 1.0f,
		float acceleration = 0);
	SpaceObject(
		uint32_t id,
		bool is_moveable,
		uint32_t type_id,
		ObjectForm& form,
		float rotation = 0.f,
		float rotation_speed = 0.f,
		float max_rotation_speed = 0.f,
		Vector&& speed_direction = Vector(1.f, 0.f),
		float max_speed = 1.0f,
		float acceleration = 0);
	// ������ ��������� ������, �������� ��� �� �����
	// SpaceObject(FILE* f);
	// ���������� �����������
	SpaceObject(PoolID* pool_id, SpaceObject& lo);
	SpaceObject(uint32_t id, SpaceObject& lo);
	// Move-�����������
	SpaceObject(SpaceObject&& lo) noexcept;

	~SpaceObject() override;

	void moveTo(double x, double y);
	void moveTo(District* district, double x, double y);

	void initInCell(DistrictCell* cell);

	// ����������, ����� ������ ������������ � col_obj
	virtual void onCollision(SpaceObject* col_obj) {};

	virtual void load(FILE* f);				// ��������� ������ �� �����
	virtual void save(FILE* f);				// ��������� ������ � ����

	// ��������� ������ � ������ �������� �������
	// ���� ������ ����� ��������� � ������ �������,
	// �������������� ������� ��� �� � ������� 
	void insertToDistrictList();
	// ������� ������ �� ������� �������� �������
	void removeFromDistrictList();

	void move(float dt);					// ���������� ������ � ��������� ����� dt ��
	void setSpeedDirection(Vector& speed_direction); // ������������� ����������� ��������
	void setCurrentSpeed(float current_speed); // ������������� ������� �������� ��������
	void setMaxSpeed(float max_speed);
	[[nodiscard]] float getMaxSpeed() const { return m_max_speed; }
	Vector getFutureCoords(float dt); // ���������� ��������� ������� ����� ����� dt
	Position getFuturePosition(float dt);

	bool isMoveable() const;

	// ��������� ������ � ������� ������
	void updateCell();

	DistrictCell* getCurrentCell() { return m_cell; }

	float getCurrentSpeed() const { return m_current_speed; }
	Vector getSpeedDirection() const { return m_speed_direction; }

protected:
	void RemoveFromOldDistrict();

	bool m_is_moveable{};					// �������� �� ���������� ��������

	float m_rotation_speed{};				// �������� ��������, ������������ ������ ��� �������� �������
	float m_max_rotation_speed{};			// ������������ �������� ��������
	float m_current_speed{};				// ������� �������� �������
	Vector m_speed_direction{};				// ������� ����������� �������� �������
	float m_max_speed{};					// ������������ ��������
	float m_acceleration{};					// ��������� ������� (�������� ������ ��������)

	DistrictCell* m_cell{};
	ListPElementInfo<SpaceObject> m_cell_info;
	ListPElementInfo<SpaceObject> m_district_info;
	ListPElementInfo<SpaceObject> m_district_moveable_info;
	// ������ ���������� � ���, ��� � � ����� ������� ���������
	// ������ ��� �������� �� ��� � ������ �������������
	// ���������� �������� ��������� ��������� �� ������,
	// ����� ��� ���������� � ������ ����� ������ 
	// ������� �� � ���� ����������� ����, �, ��� �������� -
	// ������.		
};

class Actor {
	float m_current_hit_points;
	float m_max_hit_points;
	std::unique_ptr<SpaceObject> m_physical_body;
public:
	virtual ~Actor() = default;
	Actor(std::unique_ptr<SpaceObject> physical_body, float max_hit_points)
		: m_current_hit_points(max_hit_points), m_max_hit_points(max_hit_points),
		m_physical_body(std::move(physical_body)) {
	}

	[[nodiscard]] SpaceObject* getPhysicalBody() const { return m_physical_body.get(); }
	[[nodiscard]] float getCurrentHitPoints() const { return m_current_hit_points; }
	[[nodiscard]] float getMaxHitPoints() const { return m_max_hit_points; }

	void setCurrentHitPoints(float current_hit_points) { m_current_hit_points = current_hit_points; }
	void setMaxHitPoints(float max_hit_points) { m_max_hit_points = max_hit_points; }
	void setPhysicalBody(std::unique_ptr<SpaceObject> physical_body) { m_physical_body = std::move(physical_body); }

	std::list<LocatableObject*> getAreaInterestObjects();

	void load(FILE* f);	// ��������� ������ �� �����
	void save(FILE* f);	// ��������� ������ � ����

	void moveTo(double x, double y);
	void moveTo(District* district, double x, double y);

	virtual void onLoadArea();			// ����������� ��� �������� �������, �� ������� ��������� ����
	virtual void onUnloadArea();		// ����������� ��� �������� �������, �� ������� ��������� ����
	virtual void onAreaChange();        // ����������� ��� ����� �������
	virtual void onTick();				// ����������� �� ������ ����

	virtual float getAreaInterestRadius() { return constants::actors::k_default_visible_radius; }

	[[nodiscard]] virtual bool isNetRelevantFor(const Actor& actor) const { return true; }
	virtual void onDamaged(Actor& damager, float damage) { m_current_hit_points -= damage; }
	virtual void onDie() {}
	virtual void onSpawn() {}

	//void* questEvents;
protected:
	virtual void loadv(FILE* f) {}		// ��������� ������ �� �����
	virtual void savev(FILE* f) {}		// ��������� ������ � ����
};