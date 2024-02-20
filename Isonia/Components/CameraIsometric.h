#pragma once

// internal
#include "Camera.h"
#include "Transform.h"

#include "../Pipeline/Renderer.h"
#include "../Pipeline/PixelRenderer.h"

#include "../Utilities/MathUtility.h"
#include "../Utilities/PixelPerfectUtility.h"

// external
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Isonia::Components
{
	struct CameraIsometric : public Camera
	{
	public:
		void SetView(Components::Transform* transform) override
		{
			// set final rotation
			transform->rotation.x = Utilities::Math::Radians(-30.0);

			// get matrices for calculations
			glm::mat4x4 cameraRotation = transform->NormalMatrix();
			glm::mat4x4 inverseCameraRotation = glm::inverse(cameraRotation);

			// get local position
			glm::vec3 localPosition = transform->position;

			glm::vec3 cameraPosition = cameraRotation * glm::vec4{ 0.0f, 0.0f, -cameraDistance, pixelGlobalTopLeft.w };

			// rotate offset from global to local
			glm::vec4 pixelLocalTopLeft = cameraRotation * pixelGlobalTopLeft;

			// offset by pixelLocalTopLeft to get world
			glm::vec4 pixelWorldTopLeft
			{
				localPosition.x + pixelLocalTopLeft.x,
				localPosition.y + pixelLocalTopLeft.y,
				localPosition.z + pixelLocalTopLeft.z,
				pixelLocalTopLeft.w
			};

			// rotate back to grid
			glm::vec4 pixelGlobalTopLeft = inverseCameraRotation * pixelWorldTopLeft;

			// now we can snap it to the global pixel grid
			glm::vec4 roundedPixelGlobalTopLeft = Utilities::PixelPerfectUtility::RoundToPixel(pixelGlobalTopLeft);

			// rotate it back again
			glm::vec4 roundedPixelWorldTopLeft = cameraRotation * roundedPixelGlobalTopLeft;

			// get the rounded and unrounded pixel position difference
			glm::vec3 difference = roundedPixelWorldTopLeft - pixelWorldTopLeft;

			// final position
			glm::vec3 position = localPosition + difference + cameraPosition;

			// set view and position
			SetViewYXZ(position, transform->rotation);
		}

		void SetProjection(Pipeline::PixelRenderer* renderer) override
		{
			// get extent
			const auto extent = renderer->GetExtent();
			// get ortho size
			const float orthoSize = Utilities::PixelPerfectUtility::PIXELS_PER_UNIT * 2.0;
			// set projection
			SetOrthographicProjection(
				-(extent.width / orthoSize),
				extent.width / orthoSize,
				-(extent.height / orthoSize),
				extent.height / orthoSize,
				0.f,
				1000.f
			);

			// NDC coordinates of the top left upmost center pixel position
			glm::vec4 ndcTopLeft{ -1.0f, -1.0f, 0.0f, 1.0f };
			// apply inverse projection matrix
			glm::vec4 globalTopLeft = inverseProjectionMatrix * ndcTopLeft;
			// normalize by dividing by w
			globalTopLeft /= globalTopLeft.w;
			// pushbach camera distance
			globalTopLeft.z -= cameraDistance;

			// push to center pixel position
			pixelGlobalTopLeft = {
				globalTopLeft.x + Utilities::PixelPerfectUtility::UNITS_PER_PIXEL * 0.5f,
				globalTopLeft.y + Utilities::PixelPerfectUtility::UNITS_PER_PIXEL * 0.5f,
				globalTopLeft.z + Utilities::PixelPerfectUtility::UNITS_PER_PIXEL * 0.5f,
				globalTopLeft.w
			};
		}

	private:
		const float cameraDistance = 500.0f;
		const float cameraDistanceX = 0;
		const float cameraDistanceY = -cameraDistance * std::sin(Utilities::Math::Radians(30.0f));
		const float cameraDistanceZ = -cameraDistance * std::cos(Utilities::Math::Radians(30.0f));

		glm::vec4 pixelGlobalTopLeft{};
	};
}
