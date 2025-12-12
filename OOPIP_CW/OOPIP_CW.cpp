#include "DeliverySystem.h"
#include <Windows.h>
#include <iostream>

using namespace DeliverySystem;

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	try
	{
		Manager::Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Упс! Падчас выконвання праграмы ўзнікла памылка!\n" << e.what();
		return -1;
	}
	catch (...)
	{
		std::cerr << "Упс! Узнікла невядомая памылка!\n";
		return -1;
	}
}