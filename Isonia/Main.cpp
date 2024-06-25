#ifndef DLL_BUILD

// internal
#include "Isonia.h"

// build configuration
//#ifdef PRODUCTION_BUILD
//#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
//#endif

int main()
{
	auto start_time = std::chrono::high_resolution_clock::now();
    Isonia::Isonia isonia{};
	auto end_time = std::chrono::high_resolution_clock::now();
	float initilize_time = std::chrono::duration<float, std::chrono::milliseconds::period>(end_time - start_time).count();
	std::cout << "Initilize Time: " << initilize_time << " ms" << std::endl;
	isonia.run();
}

#endif
