#include "Isonia.h"

namespace Isonia
{
	void Isonia::Run()
	{
		while (!window.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}
