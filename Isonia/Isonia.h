#pragma once

#include "Pipeline/Descriptors/Descriptors.h"
#include "Pipeline/Device.h"
#include "ECS/Coordinator.h"
#include "Pipeline/Renderer.h"
#include "Window/Window.h"

#include "Components/Gravity.h"
#include "Components/Mesh.h"
#include "Components/MeshRenderer.h"
#include "Components/RigidBody.h"

// std
#include <memory>
#include <vector>

namespace Isonia
{
	class Isonia
	{
	public:
		static constexpr int WIDTH = 1024;
		static constexpr int HEIGHT = 576;
		static constexpr const char* NAME = "Window";

		Isonia();
		~Isonia();

		Isonia(const Isonia&) = delete;
		Isonia& operator=(const Isonia&) = delete;

		void Run();

	private:
		Window::Window window{ WIDTH, HEIGHT, NAME };
		Pipeline::Device device{ window };
		Pipeline::Renderer renderer{ window, device };

		// note: order of declarations matters
		std::unique_ptr<Pipeline::Descriptors::DescriptorPool> globalPool{};
	};
}
