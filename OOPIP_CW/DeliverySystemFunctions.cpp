#include "DeliverySystem.h"
#include <iostream>
#include <conio.h>
#include <cwctype>
#include <iomanip>
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

	std::string GetString(const std::string& message)
	{
		std::string result;

		std::cout << message;
		std::getline(std::cin, result);

		result = TrimWhitespace(result);

		return result;
	}

	std::string GetString(const std::string& message, const std::string& forbiddenSymbols)
	{
		std::string result;

		while (true)
		{
			std::cout << message;
			std::getline(std::cin, result);

			result = TrimWhitespace(result);

			if (result.find_first_of(forbiddenSymbols) != std::string::npos)
			{
				std::cout << "\x1b[31;1m" << "Вы ўвялі забароненыя сімвалы для гэтага поля.\n"
					<< "Спіс забароненых сімвалаў для гэтага дадзенага поля: "
					<< forbiddenSymbols << std::endl;
				std::cout << "Паспрабуйце яшчэ раз\n" << "\x1b[0m" << std::endl;

				continue;
			}

			break;
		}

		return result;
	}

	std::string GetString(const std::string& message, unsigned int minSize, unsigned int maxSize)
	{
		std::string result;

		while(true)
		{
			std::cout << message;
			std::getline(std::cin, result);

			result = TrimWhitespace(result);

			if (result.size() < minSize)
			{
				std::cout << "\x1b[31;1m" << "Мінімальны памер: " << minSize
					<< ". Паспрабуйце яшчэ раз\n" << "\x1b[0m" << std::endl;
				continue;
			}
			else if (result.size() > maxSize)
			{
				std::cout << "\x1b[31;1m" << "Максімальны памер: " << maxSize
					<< ". Паспрабуйце яшчэ раз\n" << "\x1b[0m" << std::endl;
				continue;
			}

			break;
		}

		return result;
	}

	std::string GetString(const std::string& message, const std::string& forbiddenSymbols, unsigned int minSize,
		unsigned int maxSize, const std::vector<std::string>& exceptions)
	{
		std::string result;

		while (true)
		{
			std::cout << message;
			std::getline(std::cin, result);
		
			result = TrimWhitespace(result);

			for (const auto& str : exceptions)
			{
				if (str == result)
					return result;
			}

			if (result.size() < minSize)
			{
				std::cout << "\x1b[31;1m" << "Мінімальны памер: " << minSize
					<< ". Паспрабуйце яшчэ раз\n" << "\x1b[0m" << std::endl;
				continue;
			}
			else if (result.size() > maxSize)
			{
				std::cout << "\x1b[31;1m" << "Максімальны памер: " << maxSize
					<< ". Паспрабуйце яшчэ раз\n" << "\x1b[0m" << std::endl;
				continue;
			}
			else if (result.find_first_of(forbiddenSymbols) != std::string::npos)
			{
				std::cout << "\x1b[31;1m" << "Вы ўвялі забароненыя сімвалы для гэтага поля.\n"
					<< "Спіс забароненых сімвалаў для гэтага дадзенага поля: "
					<< forbiddenSymbols << std::endl;
				std::cout << "Паспрабуйце яшчэ раз\n" << "\x1b[0m" << std::endl;

				continue;
			}

			break;
		}

		return result;
	}

	void EnableAnsiColors()
	{
	#ifdef _WIN32
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE) return;

		DWORD dwMode = 0;
		if (!GetConsoleMode(hOut, &dwMode)) return;

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	#endif
	}

	void GoToXY(int x, int y)
	{
		COORD coord;
		coord.X = x;
		coord.Y = y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	void HideCursor()
	{
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	}

	void ShowCursor()
	{
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
		cursorInfo.bVisible = true;
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	}

	int ShowMenuWithNavigation(const std::vector<std::string>& menuItems, const std::string& title)
	{
		HideCursor();

		int selected = 0;
		int key;
		bool chosen = false;

		while (!chosen)
		{
			std::cout << std::endl << std::setw(20) << "\x1b[33;1m" << title << "\x1b[0m" << std::endl;

			for (int i = 0; i < menuItems.size(); i++)
				if (i == selected)
					std::cout << "\x1b[1m> " << menuItems[i] << "\x1b[0m" << std::endl;
				else
					std::cout << "  " << menuItems[i] << std::endl;

			std::cout << "\nУжывайце стрэлкі /\\, \\/ для навігацыі, Enter для выбара" << std::endl;

			key = _getch();

			system("cls");

			if (key == 224)
			{
				key = _getch();

				switch (key)
				{
				case 72:
					selected--;
					if (selected < 0) selected = menuItems.size() - 1;
					break;
				case 80:
					selected++;
					if (selected >= menuItems.size()) selected = 0;
					break;
				case 77:
				case 75:
					break;
				}
			}
			else if (key == 13)
				chosen = true;
			else if (key == 27)
			{
				selected = menuItems.size() - 1;
				chosen = true;
			}
			else if (key >= '1' && key <= '9')
			{
				int num = key - '0';
				if (num >= 1 && num <= menuItems.size())
				{
					selected = num - 1;
					chosen = true;
				}
			}
			else if (key >= '0' && key <= '9' && menuItems.size() >= 10)
			{
				int firstDigit = key - '0';
				key = _getch();
				if (key >= '0' && key <= '9')
				{
					int secondDigit = key - '0';
					int num = firstDigit * 10 + secondDigit;
					if (num >= 1 && num <= menuItems.size())
					{
						selected = num - 1;
						chosen = true;
					}
				}
			}
		}

		ShowCursor();
		return selected + 1;
	}
}