//
// Created by curo on 01.04.2025.
//

#include "terrain_gens.hpp"

#include "common_generator.hpp"

void TerrainGenerator::generateTerrains(District* district, float fill_coeff) {
    int x_end = district->getCellsXAmount();
    int y_end = district->getCellsXAmount();
    CommonGenerator gen;
    auto& terrain_matrix = district->getTerrainMatrix();
    for (int x = 0; x < x_end; x++)
        for (int y = 0; y < y_end; y++) {
                if (gen.isEventOccurs(fill_coeff)) {
                    if (gen.isEventOccurs(0.8))
                        terrain_matrix(y, x) = 2;
                    else
                        terrain_matrix(y, x) = 1;
                }
        }
}
