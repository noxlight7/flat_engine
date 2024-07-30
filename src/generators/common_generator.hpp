#include<random>

class CommonGenerator {
public:
	CommonGenerator();
	bool isEventOccurs(float probability);

private:
	std::default_random_engine m_generator;
	std::uniform_real_distribution<double> m_distribution;

};