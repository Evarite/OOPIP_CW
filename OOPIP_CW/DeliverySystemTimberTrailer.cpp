#include "DeliverySystem.h"
#include <iostream>

namespace DeliverySystem
{
	unsigned short TimberTrailer::speedLimit = 80;

	TimberTrailer::TimberTrailer() : Trailer() {}
	TimberTrailer::TimberTrailer(float length, float maxPayload, const std::vector<std::unique_ptr<Trailer>>& trailers) 
		: Trailer(length, maxPayload, trailers)
	{
		type = Trailer::Type::Timber;
	}

	unsigned short TimberTrailer::GetSpeedLimit() const
	{
		return speedLimit;
	}

	std::vector<Cargo::Type> TimberTrailer::GetSupportedCargoTypes() const
	{
		return { Cargo::Type::Wood };
	}

	std::string TimberTrailer::GetTypeString() const
	{
		return "Лесавоз";
	}

	std::ostream& operator<<(std::ostream& os, const TimberTrailer& obj)
	{
		os << "Тып трейлера: " << obj.GetTypeString() << std::endl;
		os << "Абмежаванне хуткасці: " << obj.GetSpeedLimit() << std::endl;
		const Trailer& baseObj = obj;
		os << baseObj;
		return os;
	}
}