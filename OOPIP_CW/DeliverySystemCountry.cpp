#include "DeliverySystem.h"
#include <iostream>
#include <fstream>
#include <sstream>

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
		const std::list<City>& cities)
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
	const std::list<City>& Country::GetCities() const
	{
		return cities;
	}
	std::list <City>& Country::GetCitiesL()
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

		auto it = cities.begin();
		std::advance(it, index);
		cities.erase(it);
	}
	void Country::RemoveCity(City* city)
	{
		for(auto i = cities.begin(); i != cities.end();)
		{
			if (*i == *city)
			{
				cities.erase(i);
				return;
			}

			std::advance(i, 1);
		}
	}

	std::vector<std::string> Country::ToTableRow() const
	{
		std::stringstream sscities;
		for (const auto& city : cities)
			sscities << city.GetName() << ' ' << city.GetCountryAbbreviation() << '\n';

		return { name, abbreviation, phoneCode, sscities.str() };
	}
	std::vector<std::string> Country::GetHeaders()
	{
		return { "Назва", "Абрэвіятура", "Тэлефонны код", "Гарады" };
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

		int i = 0;
		for (const auto& city : obj.cities)
		{
			os << '\t' << ++i << ".\n";

			std::stringstream ss;
			ss << city;

			std::string line;
			while (std::getline(ss, line))
				os << '\t' << line << '\n';
			os << '\n';
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

		for (auto& city : obj.cities)
		{
			os << city;
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

		for (auto& city : obj.cities)
		{
			is >> city;
		}

		return is;
	}
}