#include "DeliverySystem.h"
#include <iostream>

namespace DeliverySystem
{
	unsigned short TarpTrailer::speedLimit = 80;

	TarpTrailer::TarpTrailer() : Trailer() {}
	TarpTrailer::TarpTrailer(float length, float maxPayload, const std::vector<std::unique_ptr<Trailer>>& trailers)
		: Trailer(length, maxPayload, trailers)
	{
		type = Trailer::Type::Tarp;
	}

	unsigned short TarpTrailer::GetSpeedLimit() const
	{
		return speedLimit;
	}

	std::vector<Cargo::Type> TarpTrailer::GetSupportedCargoTypes() const
	{
		return { Cargo::Type::Gravel, Cargo::Type::Grain, Cargo::Type::Sand,Cargo::Type::Bricks,
			Cargo::Type::Containers };
	}

	std::string TarpTrailer::GetTypeString() const
	{
		return "Тэнтавы";
	}

	std::ostream& operator<<(std::ostream& os, const TarpTrailer& obj)
	{
		os << "Тып трейлера: " << obj.GetTypeString() << std::endl;
		os << "Абмежаванне хуткасці: " << obj.GetSpeedLimit() << std::endl;
		const Trailer& baseObj = obj;
		os << baseObj;
		return os;
	}
}