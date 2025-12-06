#include "DeliverySystem.h"
#include <iostream>

namespace DeliverySystem
{
	unsigned short RefrigeratedTrailer::speedLimit = 80;

	RefrigeratedTrailer::RefrigeratedTrailer() : Trailer() {}
	RefrigeratedTrailer::RefrigeratedTrailer(float length, float maxPayload,
		const std::list<std::unique_ptr<Trailer>>& trailers) : Trailer(length, maxPayload, trailers)
	{
		type = Trailer::Type::Refrigerated;
	}

	unsigned short RefrigeratedTrailer::GetSpeedLimit() const
	{
		return speedLimit;
	}

	std::vector<Cargo::Type> RefrigeratedTrailer::GetSupportedCargoTypes() const
	{
		return { Cargo::Type::Food, Cargo::Type::Milk, Cargo::Type::Refrigerated, Cargo::Type::Pharmaceuticals };
	}
	
	std::string RefrigeratedTrailer::GetTypeString() const
	{
		return "Рэфрыжэратар";
	}

	std::ostream& operator<<(std::ostream& os, const RefrigeratedTrailer& obj)
	{
		os << "Тып трейлера: " << obj.GetTypeString() << std::endl;
		os << "Абмежаванне хуткасці: " << obj.GetSpeedLimit() << std::endl;
		const Trailer& baseObj = obj;
		os << baseObj;
		return os;
	}
}