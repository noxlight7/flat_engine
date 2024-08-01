#include "common_generator.hpp"
#include <ctime>

CommonGenerator::CommonGenerator() : 
	m_generator(static_cast<unsigned int>(std::time(0))), 
	m_distribution(0.0, 1.0) {
}

bool CommonGenerator::isEventOccurs(float probability) {
	return m_distribution(m_generator) <= probability;
}
