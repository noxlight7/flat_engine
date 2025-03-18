#pragma once
#include "physics/physics.hpp"

class SpaceObjectGenerator {
public:
	static void generateSpaceObjects(District* district, float fill_coeff, uint32_t type_id, PoolID& pool);
};

