#include "DeliverySystem.h"
#include <iostream>
#include <conio.h>
#include <cwctype>

namespace DeliverySystem
{
	std::string TrimWhitespace(const std::string& str)
	{
		size_t start = 0;
		while (start < str.size() && std::iswspace(str[start]))
			start++;

		size_t end = str.size();
		while (end > start && std::iswspace(str[end - 1]))
			end--;

		return str.substr(start, end - start);
	}

	std::string GetPasswordWithAsterisks()
	{
		std::string password;
		char ch;

		while ((ch = _getch()) != '\r')
		{
			if (ch == '\b')
			{
				if (!password.empty())
				{
					password.pop_back();
					std::cout << "\b \b";
				}
			}
			else
			{
				password += ch;
				std::cout << '*';
			}
		}
		std::cout << std::endl;
		return password;
	}
}