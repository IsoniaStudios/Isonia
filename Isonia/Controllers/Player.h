#pragma once

// internal
#include "KeyboardMovementController.h"
#include "../Components/Transform.h"
#include "../Window/Window.h"

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
			camera.SetViewYXZ(transform.position, transform.rotation);
		}

		Pipeline::Renderer::EventHandler GetOnAspectChangeCallback()
		{
			return [&](Pipeline::Renderer* renderer) { this->OnAspectChange(renderer); };
		}

		void OnAspectChange(Pipeline::Renderer* renderer)
		{
			float aspect = renderer->GetAspectRatio();
			/*
			const float orthoSize = 100.f;
			camera.SetOrthographicProjection(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, 0.f, 1000.f);
			*/
			camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);
		}

		KeyboardMovementController controller{};
		Components::Transform transform{};
		Components::Camera camera{};
	};
}
