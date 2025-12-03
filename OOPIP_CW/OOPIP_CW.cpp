#include "DeliverySystem.h"
#include <Windows.h>

using namespace DeliverySystem;

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	Manager::Run();
}