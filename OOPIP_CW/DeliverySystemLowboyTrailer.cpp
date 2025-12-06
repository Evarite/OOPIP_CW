#include "DeliverySystem.h"
#include <iostream>

namespace DeliverySystem
{
	unsigned short LowboyTrailer::speedLimit = 80;

	LowboyTrailer::LowboyTrailer() : Trailer() {}
	LowboyTrailer::LowboyTrailer(float length, float maxPayload, const std::list<std::unique_ptr<Trailer>>& trailers)
		: Trailer(length, maxPayload, trailers)
	{
		type = Trailer::Type::Lowboy;
	}

	unsigned short LowboyTrailer::GetSpeedLimit() const
	{
		return speedLimit;
	}

	std::vector<Cargo::Type> LowboyTrailer::GetSupportedCargoTypes() const
	{
		return { Cargo::Type::Steel, Cargo::Type::Machinery, Cargo::Type::Construction };
	}

	std::string LowboyTrailer::GetTypeString() const
	{
		return "Трал";
	}

	std::ostream& operator<<(std::ostream& os, const LowboyTrailer& obj)
	{
		os << "Тып трэйлера: " << obj.GetTypeString() << std::endl;
		os << "Абмежаванне хуткасці: " << obj.GetSpeedLimit() << std::endl;
		const Trailer& baseObj = obj;
		os << baseObj;
		return os;
	}
}