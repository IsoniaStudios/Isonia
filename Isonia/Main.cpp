#include "Isonia.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

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
