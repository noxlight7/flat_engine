#include "so_gens.hpp"
#include "factory/definitions.hpp"
#include "random"
#include "common_generator.hpp"

void SpaceObjectGenerator::generateSpaceObjects(
	District* district, 
	float fill_coeff,
	Texture *tex) {
	int x_end = district->getCellsXAmount();
	int y_end = district->getCellsXAmount();
	CommonGenerator gen;
	for (int x = 0; x < x_end; x++)
		for (int y = 0; y < y_end; y++) {
			DistrictCell* cell = district->getCell(x, y);
			if (cell->getInnerObjects().empty())
				for (int xc = 0; xc < g_metres_in_cell; xc++)
					for (int yc = 0; yc < g_metres_in_cell; yc++)
						if (gen.isEventOccurs(fill_coeff)) {
							SpaceObject* so = new SpaceObject(false, ObjectForm(1.f, 1.f));
							so->addDrawInfo(tex);
							so->moveTo(district, g_cell_size * x + xc, g_cell_size * y + yc);
						}
		}
}
