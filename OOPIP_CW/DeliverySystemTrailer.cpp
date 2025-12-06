#include "DeliverySystem.h"
#include <iostream>
#include <fstream>

namespace DeliverySystem
{
	void Trailer::SetDelivery(Delivery* delivery)
	{
		currentDelivery = delivery;
	}

	void Trailer::StopDelivery()
	{
		currentDelivery = nullptr;
	}

	Trailer::Trailer()
	{
		length = 0;
		maxPayload = 0;
		currentDelivery = nullptr;
		id = 0;
	}
	Trailer::Trailer(float length, float maxPayload, const std::list<std::unique_ptr<Trailer>>& trailers)
		: length(length), maxPayload(maxPayload), currentDelivery(nullptr)
	{
		while (true)
		{
			bool isUnique = true;

			id = IDGenerator::Generate();
			for (const auto& trailer : trailers)
				if (trailer->id == id)
				{
					isUnique = false;
					break;
				}

			if (isUnique)
				break;
		}
	}

	float Trailer::GetLength() const
	{
		return length;
	}
	float Trailer::GetMaxPayload() const
	{
		return maxPayload;
	}
	Delivery* Trailer::GetCurrentDelivery() const
	{
		return currentDelivery;
	}
	unsigned int Trailer::GetID() const
	{
		return id;
	}
	Trailer::Type Trailer::GetType() const
	{
		return type;
	}
	bool Trailer::IsCargoSupported(Cargo* cargo)
	{
		for (const auto& type : GetSupportedCargoTypes())
			if (type == cargo->GetType())
				return true;
		return false;
	}

	std::ostream& operator<<(std::ostream& os, const Trailer& obj)
	{
		os << "Даўжыня прычэпа: " << obj.length << 'м' << std::endl;
		os << "Максімальная грузападымальнасць: " << obj.maxPayload << "кг" << std::endl;
		os << "Бягучае заданне: ";
		if (obj.currentDelivery == nullptr)
		{
			os << "Няма";
		}
		else
		{
			os << obj.currentDelivery->GetCityFrom() << " - " << obj.currentDelivery->GetCityTo() << '\t'
				<< obj.currentDelivery->GetCargo()->GetName();
		}
		
		return os;
	}

	std::ostream& operator<<(std::ofstream& os, const Trailer& obj)
	{
		os.write(reinterpret_cast<const char*>(&obj.type), sizeof(obj.type));
		os.write(reinterpret_cast<const char*>(&obj.id), sizeof(obj.id));
		os.write(reinterpret_cast<const char*>(&obj.length), sizeof(obj.length));
		os.write(reinterpret_cast<const char*>(&obj.maxPayload), sizeof(obj.maxPayload));

		os.flush();

		return os;
	}

	std::istream& operator>>(std::ifstream& is, Trailer& obj)
	{
		is.read(reinterpret_cast<char*>(&obj.id), sizeof(obj.id));
		is.read(reinterpret_cast<char*>(&obj.length), sizeof(obj.length));
		is.read(reinterpret_cast<char*>(&obj.maxPayload), sizeof(obj.maxPayload));

		return is;
	}
}