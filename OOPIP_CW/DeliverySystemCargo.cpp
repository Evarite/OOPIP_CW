#include "DeliverySystem.h"
#include <iostream>
#include <fstream>

namespace DeliverySystem
{
	void Cargo::SetDelivery(Delivery* delivery)
	{
		currentDelivery = delivery;
	}

	void Cargo::CompleteDelivery(std::list<Cargo>& cargos)
	{
		for (auto cargo = cargos.begin(); cargo != cargos.end();)
		{
			if (this == *cargo)
			{
				cargos.erase(cargo);
				break;
			}

			std::advance(cargo, 1);
		}

		currentDelivery = nullptr;
		to = nullptr;
	}
	void Cargo::StopDelivery()
	{
		currentDelivery = nullptr;
	}

	Cargo::Cargo()
	{
		memset(name, '\0', sizeof(name));
		mass = 0;
		type = Cargo::Type::Wood;
		currentDelivery = nullptr;
		from = nullptr;
		to = nullptr;
		id = 0;
		client = nullptr;
	}
	Cargo::Cargo(const std::string& name, float mass, const Type& type, City* from, const std::list<Cargo>& cargos)
		: mass(mass), type(type), from(from), to(nullptr), client(nullptr), currentDelivery(nullptr)
	{
		strcpy_s(this->name, name.c_str());

		while (true)
		{
			bool isUnique = true;

			id = IDGenerator::Generate();
			for (const auto& cargo : cargos)
				if (cargo.id == id)
				{
					isUnique = false;
					break;
				}

			if (isUnique)
				break;
		}
	}

	std::string Cargo::GetName() const
	{
		return std::string(name);
	}
	float Cargo::GetMass() const
	{
		return mass;
	}
	Cargo::Type Cargo::GetType() const
	{
		return type;
	}

	City* Cargo::GetCityFrom() const
	{
		return from;
	}
	City* Cargo::GetCityTo() const
	{
		return to;
	}
	unsigned int Cargo::GetID() const
	{
		return id;
	}
	Delivery* Cargo::GetCurrentDelivery() const
	{
		return currentDelivery;
	}
	Account* Cargo::GetClient() const
	{
		return client;
	}

	void Cargo::RequestDelivery(Account * client, City * cityTo)
	{
		this->client = client;
		this->client->AddCargo(this);
		this->to = cityTo;
	}

	bool Cargo::operator==(Cargo* other)
	{
		return id == other->id;
	}
	bool Cargo::operator==(const Cargo& other)
	{
		return id == other.id;
	}

	std::ostream& operator<<(std::ostream& os, const Cargo& obj)
	{
		os << "ID: " << obj.id << std::endl;
		os << "Назва: " << obj.name << std::endl;
		os << "Маса: " << obj.mass << "кг" << std::endl;
		os << "Тып: " << obj.type << std::endl;
		os << "З: " << obj.from->GetName() << ' ' << obj.from->GetCountryAbbreviation() << std::endl;
		if (obj.to != nullptr)
		{
			os << "Да: " << obj.to->GetName() << ' ' << obj.to->GetCountryAbbreviation() << std::endl;
			os << "Заказчык: " << obj.client->GetNickname() << std::endl;
		}

		os << "Стан: ";

		if (obj.currentDelivery == nullptr)
		{
			os << "Не дастаўляецца";
		}
		else
		{
			os << "У пуці";
		}
		
		return os;
	}

	std::ostream& operator<<(std::ofstream& os, const Cargo& obj)
	{
		unsigned int fromID = obj.from->GetID();
		unsigned int toID = obj.to != nullptr ? obj.to->GetID() : 0;

		std::string nickname = obj.client != nullptr ? obj.client->GetNickname() : "\0";

		os.write(nickname.c_str(), NAME_SIZE);
		os.write(obj.name, NAME_SIZE);
		os.write(reinterpret_cast<const char*>(&obj.id), sizeof(unsigned int));
		os.write(reinterpret_cast<const char*>(&obj.mass), sizeof(float));
		os.write(reinterpret_cast<const char*>(&obj.type), sizeof(obj.type));
		os.write(reinterpret_cast<const char*>(&fromID), sizeof(unsigned int));
		os.write(reinterpret_cast<const char*>(&toID), sizeof(unsigned int));

		os.flush();

		return os;
	}

	std::istream& operator>>(std::ifstream& is, Cargo& obj)
	{
		unsigned int fromID, toID;
		char clientName[NAME_SIZE];

		is.read(clientName, NAME_SIZE);
		if (!is.good())
		{
			return is;
		}
		is.read(obj.name, NAME_SIZE);
		is.read(reinterpret_cast<char*>(&obj.id), sizeof(unsigned int));
		is.read(reinterpret_cast<char*>(&obj.mass), sizeof(float));
		is.read(reinterpret_cast<char*>(&obj.type), sizeof(obj.type));
		is.read(reinterpret_cast<char*>(&fromID), sizeof(unsigned int));
		is.read(reinterpret_cast<char*>(&toID), sizeof(unsigned int));

		obj.from = static_cast<City*>(Manager::FindWithID<City>(fromID));
		obj.to = static_cast<City*>(Manager::FindWithID<City>(toID));

		if (clientName[0] != '\0')
		{
			obj.client = Manager::FindAccount(clientName);
		}
		else
			obj.client = nullptr;

		return is;
	}
}