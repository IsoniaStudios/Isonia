#include "Isonia.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#ifdef DEBUG
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

int main()
{
	try
	{
		Isonia::Isonia isonia{};
		isonia.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
