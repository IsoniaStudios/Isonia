#pragma once

// internal
#include "Camera.h"
#include "Transform.h"

#include "../Pipeline/Renderer.h"
#include "../Pipeline/PixelRenderer.h"

#include "../Math/Vector.h"

namespace Isonia::Components
{
	struct CameraIsometric : public Camera
	{
	public:
		void SetView(Components::Transform* transform) override
		{
			// set final rotation
			transform->rotation.x = Math::Radians(-30.0);

			// get matrices for calculations
			Math::Matrix4x4 cameraRotation = transform->NormalMatrix();
			Math::Matrix4x4 inverseCameraRotation = Math::Inverse(cameraRotation);

			// get local position
			Math::Vector3 localPosition = transform->position;

			Math::Vector3 cameraPosition = cameraRotation * Math::Vector4{ 0.0f, 0.0f, -cameraDistance, pixelGlobalTopLeft.w };

			// rotate offset from global to local
			Math::Vector4 pixelLocalTopLeft = cameraRotation * pixelGlobalTopLeft;

			// offset by pixelLocalTopLeft to get world
			Math::Vector4 pixelWorldTopLeft
			{
				localPosition.x + pixelLocalTopLeft.x,
				localPosition.y + pixelLocalTopLeft.y,
				localPosition.z + pixelLocalTopLeft.z,
				pixelLocalTopLeft.w
			};

			// rotate back to grid
			Math::Vector4 pixelGlobalTopLeft = inverseCameraRotation * pixelWorldTopLeft;

			// now we can snap it to the global pixel grid
			Math::Vector4 roundedPixelGlobalTopLeft = Math::Retro::RoundToPixel(pixelGlobalTopLeft);

			// rotate it back again
			Math::Vector4 roundedPixelWorldTopLeft = cameraRotation * roundedPixelGlobalTopLeft;

			// get the rounded and unrounded pixel position difference
			Math::Vector3 difference = roundedPixelWorldTopLeft - pixelWorldTopLeft;

			// final position
			Math::Vector3 position = localPosition + difference + cameraPosition;

			// set view and position
			SetViewYXZ(position, transform->rotation);

			// from difference get render offset by rotating back
			Math::Vector3 unrotatedDifference = inverseCameraRotation * Math::Vector4{ difference, 1.0 };

			// from now on dismiss z
			subPixelOffset.x = unrotatedDifference.x;
			subPixelOffset.y = unrotatedDifference.y;
		}

		void SetProjection(Pipeline::PixelRenderer* renderer) override
		{
			// get extent
			const auto extent = renderer->GetExtent();
			// get ortho size
			const float orthoSize = Math::Retro::PIXELS_PER_UNIT * 2.0;
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
			Math::Vector4 ndcTopLeft{ -1.0f, -1.0f, 0.0f, 1.0f };
			// apply inverse projection matrix
			Math::Vector4 globalTopLeft = inverseProjectionMatrix * ndcTopLeft;
			// normalize by dividing by w
			globalTopLeft /= globalTopLeft.w;
			// pushbach camera distance
			globalTopLeft.z -= cameraDistance;

			// push to center pixel position
			pixelGlobalTopLeft = {
				globalTopLeft.x + Math::Retro::UNITS_PER_PIXEL * 0.5f,
				globalTopLeft.y + Math::Retro::UNITS_PER_PIXEL * 0.5f,
				globalTopLeft.z + Math::Retro::UNITS_PER_PIXEL * 0.5f,
				globalTopLeft.w
			};
		}

		Math::Vector2 subPixelOffset{};

	private:
		const float cameraDistance = 500.0f;
		const float cameraDistanceX = 0;
		const float cameraDistanceY = -cameraDistance * Math::Sin(Math::Radians(30.0f));
		const float cameraDistanceZ = -cameraDistance * Math::Cos(Math::Radians(30.0f));

		Math::Vector4 pixelGlobalTopLeft{};
	};
}
