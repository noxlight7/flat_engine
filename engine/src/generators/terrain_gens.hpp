//
// Created by curo on 01.04.2025.
//

#ifndef TERRAIN_GENS_H
#define TERRAIN_GENS_H

#include "physics/physics.hpp"

class TerrainGenerator {
public:
    static void generateTerrains(District* district, float fill_coeff);
};

#endif //TERRAIN_GENS_H
