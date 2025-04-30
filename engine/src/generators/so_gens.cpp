#include "so_gens.hpp"
#include "factory/definitions.hpp"
#include "random"
#include "common_generator.hpp"

void SpaceObjectGenerator::generateSpaceObjects(
	District* district, 
	float fill_coeff,
	uint32_t type_id, PoolID& pool) {
	int x_end = district->getCellsXAmount();
	int y_end = district->getCellsXAmount();
	CommonGenerator gen;
	ObjectForm std_form = {1.f, 1.f};
	for (int x = 0; x < x_end; x++)
		for (int y = 0; y < y_end; y++) {
			DistrictCell* cell = district->getCell(x, y);
			if (cell->getInnerObjects().empty())
				if (gen.isEventOccurs(fill_coeff)) {
					auto so = std::make_unique<SpaceObject>(
						SpaceObject(
							&pool,
							false,
							type_id,
							std_form));

					cell->addFillObject(std::move(so));
				}
		}
}
