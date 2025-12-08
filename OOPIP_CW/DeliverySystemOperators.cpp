#include "DeliverySystem.h"
#include <iostream>

namespace DeliverySystem
{
	std::ostream& operator<<(std::ostream& os, const Trailer::Type& type)
	{
		switch (type)
		{
		case Trailer::Type::Car:
			std::cout << "Аўтавоз";
			break;
		case Trailer::Type::Lowboy:
			std::cout << "Трал";
			break;
		case Trailer::Type::Refrigerated:
			std::cout << "Рэфрыжэратар";
			break;
		case Trailer::Type::Tank:
			std::cout << "Цыстэрна";
			break;
		case Trailer::Type::Tarp:
			std::cout << "Тэнтавы";
			break;
		case Trailer::Type::Timber:
			std::cout << "Лесавоз";
			break;
		}

		return os;
	}

	std::ostream& operator<<(std::ostream& os, const Cargo::Type& type)
	{
		switch (type)
		{
		case Cargo::Type::Wood:
			os << "Драўніна";
			break;
		case Cargo::Type::Food:
			os << "Харчовыя прадукты";
			break;
		case Cargo::Type::Cars:
			os << "Аўтамабілі";
			break;
		case Cargo::Type::Fuel:
			os << "Паліва";
			break;
		case Cargo::Type::Chemicals:
			os << "Хімічныя рэчыва";
			break;
		case Cargo::Type::Milk:
			os << "Малако";
			break;
		case Cargo::Type::Gravel:
			os << "Жвір, друз";
			break;
		case Cargo::Type::Grain:
			os << "Крупы";
			break;
		case Cargo::Type::Sand:
			os << "Пясок";
			break;
		case Cargo::Type::Concrete:
			os << "Бетон";
			break;
		case Cargo::Type::Steel:
			os << "Сталёвыя канструкцыі";
			break;
		case Cargo::Type::Bricks:
			os << "Цэгла";
			break;
		case Cargo::Type::Machinery:
			os << "Прамысловае абсталяванне";
			break;
		case Cargo::Type::Construction:
			os << "Будаўнічая тэхніка";
			break;
		case Cargo::Type::Containers:
			os << "Кантэйнеры";
			break;
		case Cargo::Type::Explosives:
			os << "Выбуховыя рэчыва";
			break;
		case Cargo::Type::Toxic:
			os << "Таксічныя матэрыялы";
			break;
		case Cargo::Type::Refrigerated:
			os << "Замарожаныя прадукты";
			break;
		case Cargo::Type::Pharmaceuticals:
			os << "Медыкаменты";
			break;
		}

		return os;
	}

	std::ostream& operator<<(std::ostream& os, const Account::Type& type)
	{
		switch (type)
		{
		case Account::Type::User:
			os << "Карыстальнік";
			break;
		case Account::Type::Driver:
			os << "Кіраўнік";
			break;
		case Account::Type::Moderator:
			os << "Мадэратар";
			break;
		case Account::Type::Admin:
			os << "Адміністратар";
			break;
		}

		return os;
	}
}