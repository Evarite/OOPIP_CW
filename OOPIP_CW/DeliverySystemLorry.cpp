#include "DeliverySystem.h"
#include <iostream>
#include <fstream>

namespace DeliverySystem
{
	void Lorry::SetDelivery(Delivery* delivery)
	{
		if (delivery != nullptr)
			currentDelivery = delivery;
	}

	void Lorry::StopDelivery()
	{
		currentDelivery = nullptr;
	}

	Lorry::Lorry()
	{
		memset(make, '\0', sizeof(make));
		memset(model, '\0', sizeof(model));
		memset(registrationNumber, '\0', sizeof(registrationNumber));

		mileage = 0;
		gasolineCost = 0;

		owner = nullptr;

		currentDelivery = nullptr;

		id = 0;
	}
	Lorry::Lorry(const std::string& make, const std::string& model, unsigned int mileage, const Country& country,
		const City& city, const std::string& registrationSigns, float gasolineCost, const std::vector<Lorry>& lorries)
		: mileage(mileage), gasolineCost(gasolineCost)
	{
		strcpy_s(this->make, make.c_str());
		strcpy_s(this->model, model.c_str());

		std::string number = country.GetAbbreviation() + '-' + city.GetAbbreviation() + registrationSigns;
		strcpy_s(registrationNumber, number.c_str());
		
		while (true)
		{
			bool isUnique = true;

			id = IDGenerator::Generate();
			for (const auto& lorry : lorries)
				if (lorry.id == id)
				{
					isUnique = false;
					break;
				}

			if (isUnique)
				break;
		}

		owner = nullptr;

		currentDelivery = nullptr;
	}
	Lorry::Lorry(const std::string& make, const std::string& model, unsigned int mileage, const Country& country,
		const City& city, const std::string& registrationSigns, float gasolineCost, Driver* owner,
		const std::vector<Lorry>& lorries) : mileage(mileage), gasolineCost(gasolineCost)
	{
		strcpy_s(this->make, make.c_str());
		strcpy_s(this->model, model.c_str());

		std::string number = country.GetAbbreviation() + '-' + city.GetAbbreviation() + registrationSigns;
		strcpy_s(registrationNumber, number.c_str());

		while (true)
		{
			bool isUnique = true;

			id = IDGenerator::Generate();
			for (const auto& lorry : lorries)
				if (lorry.id == id)
				{
					isUnique = false;
					break;
				}

			if (isUnique)
				break;
		}

		this->owner = owner;

		currentDelivery = nullptr;
	}

	std::string Lorry::GetMake() const
	{
		return std::string(make);
	}
	std::string Lorry::GetModel() const
	{
		return std::string(model);
	}
	std::string Lorry::GetRegistrationNumber() const
	{
		return std::string(registrationNumber);
	}
	unsigned int Lorry::GetMileage() const
	{
		return mileage;
	}
	Delivery* Lorry::GetCurrentDelivery() const
	{
		return currentDelivery;
	}
	Driver* Lorry::GetOwner() const
	{
		return owner;
	}
	unsigned int Lorry::GetID() const
	{
		return id;
	}

	void Lorry::SetOwner(Driver* driver)
	{
		owner = driver;
	}

	void Lorry::AddMileage(unsigned int newMileage)
	{
		mileage += newMileage;
	}

	float Lorry::CalculateGasolineCost(unsigned int distance)
	{
		return distance * gasolineCost / 100;
	}

	std::ostream& operator<<(std::ostream& os, const Lorry& obj)
	{
		os << "Марка: " << obj.make << std::endl;
		os << "Мадэль: " << obj.model << std::endl << std::endl;
		os << "Каштоўнасць бензіну на 100км: " << obj.gasolineCost << '$' << std::endl << std::endl;
		os << "Рэгістрацыйны нумар: " << obj.registrationNumber << std::endl << std::endl;

		if (obj.owner == nullptr)
		{
			os << "Належыць да кампаніі";
		}
		else
		{
			os << "Уладалец: " << obj.owner->GetAccount()->GetFirstName() << ' '
				<< obj.owner->GetAccount()->GetLastName();
		}

		os << std::endl << std::endl;

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

	std::ostream& operator<<(std::ofstream& os, const Lorry& obj)
	{
		os.write(obj.make, NAME_SIZE);
		os.write(obj.model, NAME_SIZE);
		os.write(obj.registrationNumber, REGISTRATION_NUMBER_SIZE);
		os.write(reinterpret_cast<const char*>(&obj.id), sizeof(unsigned int));
		os.write(reinterpret_cast<const char*>(&obj.mileage), sizeof(unsigned int));
		os.write(reinterpret_cast<const char*>(&obj.gasolineCost), sizeof(float));

		os.flush();

		return os;
	}

	std::istream& operator>>(std::ifstream& is, Lorry& obj)
	{
		is.read(obj.make, NAME_SIZE);
		if (!is.good())
		{
			return is;
		}
		is.read(obj.model, NAME_SIZE);
		is.read(obj.registrationNumber, REGISTRATION_NUMBER_SIZE);
		is.read(reinterpret_cast<char*>(&obj.id), sizeof(unsigned int));
		is.read(reinterpret_cast<char*>(&obj.mileage), sizeof(unsigned int));
		is.read(reinterpret_cast<char*>(&obj.gasolineCost), sizeof(float));

		return is;
	}
}