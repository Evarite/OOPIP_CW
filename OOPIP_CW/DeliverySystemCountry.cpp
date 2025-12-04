#include "DeliverySystem.h"
#include <iostream>
#include <fstream>

namespace DeliverySystem
{
	Country::Country()
	{
		memset(name, '\0', sizeof(name));
		memset(abbreviation, '\0', sizeof(abbreviation));
		memset(phoneCode, '\0', sizeof(phoneCode));
	}
	Country::Country(const std::string& name, const std::string& abbreviation, const std::string& phoneCode)
	{
		strcpy_s(this->name, name.c_str());
		strcpy_s(this->abbreviation, abbreviation.c_str());
		strcpy_s(this->phoneCode, phoneCode.c_str());
	}
	Country::Country(const std::string& name, const std::string& abbreviation, const std::string& phoneCode,
		const City& city)
	{
		strcpy_s(this->name, name.c_str());
		strcpy_s(this->abbreviation, abbreviation.c_str());
		strcpy_s(this->phoneCode, phoneCode.c_str());

		cities.push_back(city);
	}
	Country::Country(const std::string& name, const std::string& abbreviation, const std::string& phoneCode,
		const std::vector<City>& cities)
	{
		strcpy_s(this->name, name.c_str());
		strcpy_s(this->abbreviation, abbreviation.c_str());
		strcpy_s(this->phoneCode, phoneCode.c_str());

		this->cities = cities;
	}

	std::string Country::GetName() const
	{
		return std::string(name);
	}
	std::string Country::GetPhoneCode() const
	{
		return std::string(phoneCode);
	}
	std::string Country::GetAbbreviation() const
	{
		return std::string(abbreviation);
	}
	const std::vector<City>& Country::GetCities() const
	{
		return cities;
	}
	std::vector<City>& Country::GetCitiesL() 
	{
		return cities;
	}

	void Country::AddCity(const City& city)
	{
		cities.push_back(city);
	}
	void Country::RemoveCity(unsigned int index)
	{
		if (index >= cities.size() || index < 0)
			return;

		cities.erase(cities.begin() + index);
	}
	void Country::RemoveCity(City* city)
	{
		for(size_t i = 0; i < cities.size(); i++)
			if (cities[i] == *city)
			{
				cities.erase(cities.begin() + i);
				return;
			}
	}
	
	bool Country::operator==(const Country& other)
	{
		return GetName() == other.GetName();
	}

	std::ostream& operator<<(std::ostream& os, const Country& obj)
	{
		os << "Назва краіны: " << obj.name << std::endl;
		os << "Абрэвіятура: " << obj.abbreviation << std::endl;
		os << "Тэлефонны код: " << obj.phoneCode << std::endl;
		os << "Спіс гарадоў: " << std::endl << std::endl;
		os << "*************************************************" << std::endl << std::endl;

		for (size_t i = 0; i < obj.cities.size(); i++)
		{
			os << i + 1 << std::endl << std::endl;
			os << obj.cities[i] << std::endl << std::endl;
			os << "*************************************************" << std::endl << std::endl;
		}

		return os;
	}

	std::ostream& operator<<(std::ofstream & os, const Country & obj)
	{
		os.write(reinterpret_cast<const char*>(&obj.name), sizeof(obj.name));
		os.write(reinterpret_cast<const char*>(&obj.abbreviation), sizeof(obj.abbreviation));
		os.write(reinterpret_cast<const char*>(&obj.phoneCode), sizeof(obj.phoneCode));

		size_t size = obj.cities.size();
		os.write(reinterpret_cast<const char*>(&size), sizeof(size));

		for (size_t i = 0; i < obj.cities.size(); i++)
		{
			os << obj.cities[i];
		}

		return os;
	}

	std::istream& operator>>(std::ifstream& is, Country& obj)
	{
		is.read(reinterpret_cast<char*>(&obj.name), sizeof(obj.name));
		if (!is.good())
		{
			return is;
		}

		is.read(reinterpret_cast<char*>(&obj.abbreviation), sizeof(obj.abbreviation));
		is.read(reinterpret_cast<char*>(&obj.phoneCode), sizeof(obj.phoneCode));

		size_t size;
		is.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		obj.cities.resize(size);

		for (size_t i = 0; i < size; i++)
		{
			is >> obj.cities[i];
		}

		return is;
	}
}