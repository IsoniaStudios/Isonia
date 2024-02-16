#pragma once

// internal
#include "KeyboardController.h"
#include "KeyboardControllerIsometric.h"

#include "../Components/Camera.h"
#include "../Components/Transform.h"
#include "../Components/CameraIsometric.h"

#include "../Window/Window.h"
#include "../Pipeline/Renderer.h"

#include "../Utilities/MathUtility.h"
#include "../Utilities/PixelPerfectUtility.h"

// std
#include <limits>

// external
#include <glm/gtc/matrix_transform.hpp>

namespace Isonia::Controllers
{
	class Player
	{
	public:
		Player()
		{
		}

		~Player()
		{
		}

		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

		void Act(GLFWwindow* window, float frameTime)
		{
			controller.Move(window, frameTime, &transform);
			camera.SetView(&transform);
		}

		Pipeline::Renderer::EventHandler GetOnAspectChangeCallback()
		{
			return [&](Pipeline::Renderer* renderer) { this->OnAspectChange(renderer); };
		}

		void OnAspectChange(Pipeline::Renderer* renderer)
		{
			camera.SetProjection(renderer);
		}

		Components::Transform transform{};
		Components::CameraIsometric camera{};
		KeyboardControllerIsometric controller{};
	};
}
