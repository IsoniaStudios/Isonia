#ifndef DLL_BUILD

// internal
#include "Isonia.h"

// build configuration
//#ifdef PRODUCTION_BUILD
//#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
//#endif

int main()
{
    Isonia::Isonia isonia{};
    isonia.run();
}

#endif
