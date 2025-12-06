#include "DeliverySystem.h"
#include <iostream>
#include <fstream>

namespace DeliverySystem
{
	Driver* driver;
	Lorry* lorry;
	Cargo* cargo;
	Trailer* trailer;
	unsigned int remainingDistance;

	Delivery::Delivery()
	{
		driver = nullptr;
		lorry = nullptr;
		cargo = nullptr;
		trailer = nullptr;
		remainingDistance = 0;
	}
	Delivery::Delivery(Driver* driver, Lorry* lorry, Cargo* cargo, Trailer* trailer) : driver(driver), lorry(lorry),
		cargo(cargo), trailer(trailer)
	{
		this->driver->SetDelivery(this);
		this->lorry->SetDelivery(this);
		this->cargo->SetDelivery(this);
		this->trailer->SetDelivery(this);
		remainingDistance = City::CalculateDistance(*cargo->from, *cargo->to);
	}

	Driver* Delivery::GetDriver() const
	{
		return driver;
	}
	Cargo* Delivery::GetCargo() const
	{
		return cargo;
	}
	Lorry* Delivery::GetLorry() const
	{
		return lorry;
	}
	City* Delivery::GetCityFrom() const
	{
		return cargo->GetCityFrom();
	}
	City* Delivery::GetCityTo() const
	{
		return cargo->GetCityTo();
	}
	Trailer* Delivery::GetTrailer() const
	{
		return trailer;
	}
	int Delivery::GetRemainingDistance() const
	{
		return remainingDistance;
	}

	void Delivery::StopDelivery(std::list<Delivery>& deliveries)
	{
		std::string nickname = driver->GetAccount()->GetNickname();
		unsigned int lorryID = lorry->GetID();
		unsigned int cargoID = cargo->GetID();
		unsigned int trailerID = trailer->GetID();

		for (auto i = deliveries.begin(); i != deliveries.end();)
		{
			if (*i == *this)
			{
				deliveries.erase(i);
				break;
			}

			std::advance(i, 1);
		}

		Manager::FindDriver(nickname)->StopDelivery();
		static_cast<Lorry*>(Manager::FindWithID<Lorry>(lorryID))->StopDelivery();
		static_cast<Cargo*>(Manager::FindWithID<Cargo>(cargoID))->StopDelivery();
		static_cast<Trailer*>(Manager::FindWithID<Trailer>(trailerID))->StopDelivery();
	}
	void Delivery::UpdateDistance(std::list<Delivery>& deliveries)
	{
		//Skipping an hour
		remainingDistance -= trailer->GetSpeedLimit();
		if (remainingDistance <= 0)
		{
			std::cout << "\nТолькі што была завершана наступная перавозка:\n";
			std::cout << *this << '\n';
			this->StopDelivery(deliveries);
		}
	}

	bool Delivery::operator==(const Delivery& obj) const
	{
		return driver->GetAccount()->GetNickname() == obj.GetDriver()->GetAccount()->GetNickname();
	}

	std::ostream& operator<<(std::ostream& os, const Delivery& obj)
	{
		os << "Груз: " << obj.cargo->GetName() << std::endl;
		os << "З: " << obj.cargo->GetCityFrom()->GetName() << ' '
			<< obj.cargo->GetCityFrom()->GetCountryAbbreviation() << std::endl;
		os << "Да: " << obj.cargo->GetCityTo()->GetName() << ' '
			<< obj.cargo->GetCityTo()->GetCountryAbbreviation() << std::endl;
		os << "Заказчык: " << obj.cargo->GetClient()->GetNickname() << std::endl;
		os << "Кіраўнік: " << obj.driver->GetAccount()->GetFirstName() << ' '
			<< obj.driver->GetAccount()->GetLastName() << std::endl;
		os << "Грузавік: " << obj.lorry->GetMake() << ' ' << obj.lorry->GetModel() << std::endl;
		os << "Прычэп: " << obj.trailer->GetTypeString()  << std::endl;

		if (obj.remainingDistance > 0)
			os << "Астатняя адлегласць: " << obj.remainingDistance;	
		
		return os;
	}

	std::ostream& operator<<(std::ofstream& os, const Delivery& obj)
	{
		unsigned int lorryID = obj.lorry->GetID();
		unsigned int cargoID = obj.cargo->GetID();
		unsigned int trailerID = obj.trailer->GetID();

		os.write(obj.driver->GetAccount()->GetNickname().c_str(), NAME_SIZE);
		os.write(reinterpret_cast<const char*>(&lorryID), sizeof(int));
		os.write(reinterpret_cast<const char*>(&cargoID), sizeof(unsigned int));
		os.write(reinterpret_cast<const char*>(&trailerID), sizeof(unsigned int));
		os.write(reinterpret_cast<const char*>(&obj.remainingDistance), sizeof(unsigned int));

		return os;
	}

	std::istream& operator>>(std::ifstream& is, Delivery& obj)
	{
		unsigned int lorryID, cargoID, trailerID;
		char accountName[NAME_SIZE];
		
		is.read(accountName, NAME_SIZE);
		if (!is.good())
		{
			return is;
		}
		is.read(reinterpret_cast<char*>(&lorryID), sizeof(unsigned int));
		is.read(reinterpret_cast<char*>(&cargoID), sizeof(unsigned int));
		is.read(reinterpret_cast<char*>(&trailerID), sizeof(unsigned int));
		is.read(reinterpret_cast<char*>(&obj.remainingDistance), sizeof(unsigned int));
		
		obj.driver = Manager::FindDriver(accountName);
		obj.lorry = static_cast<Lorry*>(Manager::FindWithID<Lorry>(lorryID));
		obj.cargo = static_cast<Cargo*>(Manager::FindWithID<Cargo>(cargoID));
		obj.trailer = static_cast<Trailer*>(Manager::FindWithID<Trailer>(trailerID));

		return is;
	}
}