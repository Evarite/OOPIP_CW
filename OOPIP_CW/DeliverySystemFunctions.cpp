#include "DeliverySystem.h"
#include <iostream>
#include <conio.h>
#include <cwctype>
#define NOMINMAX
#include <Windows.h>

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

	int GetInt(const std::string& message)
	{
		int result;
		
		while (true)
		{
			std::cout << message;
			std::cin >> result;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		
			return result;
		}
	}

	int GetIntWithinRange(int left, int right, const std::string& message)
	///Get a number within the given range, borders included
	{
		int result;
		
		while(true)
		{
			result = GetInt(message);

			if (result < left || result > right)
			{
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				continue;
			}

			return result;
		}
	}

	float GetFloat(const std::string& message)
	{
		float result;

		while (true)
		{
			std::cout << message;
			std::cin >> result;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			return result;
		}
	}

	void EnableAnsiColors()
	{
	#ifdef _WIN32
		// Получаем хэндл консоли
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE) return;

		// Получаем текущий режим
		DWORD dwMode = 0;
		if (!GetConsoleMode(hOut, &dwMode)) return;

		// Включаем поддержку виртуального терминала
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	#endif
	}
}