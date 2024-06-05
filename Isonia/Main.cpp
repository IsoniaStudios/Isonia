#ifndef DLL_BUILD

// internal
#include "Isonia.h"

// build configuration
#ifdef PRODUCTION_BUILD
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
#endif
