#include "DeliverySystem.h"

namespace DeliverySystem
{
	std::random_device IDGenerator::rd;
	std::mt19937 IDGenerator::gen(IDGenerator::rd());
	std::uniform_int_distribution<unsigned int> IDGenerator::dis(MIN_ID, MAX_ID);

	unsigned int IDGenerator::Generate()
	{
		return dis(gen);
	}
}