#include "DeliverySystem.h"
#include <iostream>

namespace DeliverySystem
{
	unsigned short TankTrailer::speedLimit = 70;

	TankTrailer::TankTrailer() : Trailer() {}
	TankTrailer::TankTrailer(float length, float maxPayload, const std::list<std::unique_ptr<Trailer>>& trailers) 
		: Trailer(length, maxPayload, trailers)
	{
		type = Trailer::Type::Tank;
	}

	unsigned short TankTrailer::GetSpeedLimit() const
	{
		return speedLimit;
	}

	std::vector<Cargo::Type> TankTrailer::GetSupportedCargoTypes() const
	{
		return { Cargo::Type::Fuel, Cargo::Type::Chemicals, Cargo::Type::Concrete,Cargo::Type::Explosives,
			Cargo::Type::Toxic };
	}

	std::string TankTrailer::GetTypeString() const
	{
		return "Цыстэрна";
	}

	std::ostream& operator<<(std::ostream& os, const TankTrailer& obj)
	{
		os << "Тып трейлера: " << obj.GetTypeString() << std::endl;
		os << "Абмежаванне хуткасці: " << obj.GetSpeedLimit() << std::endl;
		const Trailer& baseObj = obj;
		os << baseObj;
		return os;
	}
}