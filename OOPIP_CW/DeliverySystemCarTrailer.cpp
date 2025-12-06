#include "DeliverySystem.h"
#include <iostream>

namespace DeliverySystem
{
	unsigned short CarTrailer::speedLimit = 80;

	CarTrailer::CarTrailer() : Trailer() {}
	CarTrailer::CarTrailer(float length, float maxPayload, const std::list<std::unique_ptr<Trailer>>& trailers)
		: Trailer(length, maxPayload, trailers)
	{
		type = Trailer::Type::Car;
	}

	unsigned short CarTrailer::GetSpeedLimit() const
	{
		return speedLimit;
	}

	std::vector<Cargo::Type> CarTrailer::GetSupportedCargoTypes() const
	{
		return {Cargo::Type::Cars};
	}

	std::string CarTrailer::GetTypeString() const
	{
		return "Аўтавоз";
	}

	std::ostream& operator<<(std::ostream& os, const CarTrailer& obj)
	{
		os << "Тып трэйлера: " << obj.GetTypeString() << std::endl;
		os << "Абмежаванне хуткасці: " << obj.GetSpeedLimit() << std::endl;
		const Trailer& baseObj = obj;
		os << baseObj;
		return os;
	}
}