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
}