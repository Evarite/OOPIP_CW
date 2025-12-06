#include "DeliverySystem.h"
#include <iostream>
#include <fstream>

namespace DeliverySystem
{
	City::City()
	{
		memset(name, '\0', sizeof(name));
		memset(abbreviation, '\0', sizeof(abbreviation));
		memset(countryAbbreviation, '\0', sizeof(countryAbbreviation));
		x = 0;
		y = 0;
		id = 0;
	}
	City::City(const std::string& name, const std::string& abbreviation, Country& country, int x, int y,
		const std::list<Country>& countries) : x(x), y(y)
	{
		strcpy_s(this->name, name.c_str());
		strcpy_s(this->abbreviation, abbreviation.c_str());
		strcpy_s(this->countryAbbreviation, country.GetAbbreviation().c_str());

		while (true)
		{
			bool isUnique = true;

			id = IDGenerator::Generate();
			for (const auto& country : countries)
				for(const auto& city : country.GetCities())
					if (city.id == id)
					{
						isUnique = false;
						break;
					}

			if (isUnique)
				break;
		}

		country.AddCity(*this);
	}

	std::string City::GetName() const
	{
		return std::string(name);
	}
	std::string City::GetAbbreviation() const
	{
		return std::string(abbreviation);
	}
	std::string City::GetCountryAbbreviation() const
	{
		return std::string(countryAbbreviation);
	}
	unsigned int City::GetID() const
	{
		return id;
	}

	unsigned int City::CalculateDistance(const City& city1, const City& city2)
	{
		int x = city1.x - city2.x;
		int y = city1.y - city2.y;

		return unsigned int(sqrt(x * x + y * y));
	}

	bool City::operator==(const City& obj) const
	{
		return GetName() == obj.GetName() && GetCountryAbbreviation() == obj.GetCountryAbbreviation();
	}

	std::ostream& operator<<(std::ostream& os, const City& obj)
	{		
		os << "Назва горада: " << obj.name << std::endl;
		os << "Краіна: " << obj.countryAbbreviation << std::endl;
		os << "Абрэвіятура: " << obj.abbreviation;	

		return os;
	}

	std::ostream& operator<<(std::ofstream& os, const City& obj)
	{
		os.write(reinterpret_cast<const char*>(&obj), sizeof(City));
		if (!os.good())
		{
			throw std::runtime_error("Не атрымалася выканаць запіс у файл");
		}

		os.flush();

		return os;
	}

	std::istream& operator>>(std::ifstream& is, City& obj)
	{
		is.read(reinterpret_cast<char*>(&obj), sizeof(City));
		if (!is.good())
		{
			throw std::runtime_error("Не атрымалася выканаць чытанне з файла");
		}

		return is;
	}
}