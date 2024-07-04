// internal
#include "State.h"

// external
#include <windows.h>

namespace Isonia::State
{
	bool GrowHeap(unsigned long long size)
	{
        void* memory = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
		return memory != nullptr;
	}
}
