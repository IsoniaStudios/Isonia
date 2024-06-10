// internal
#include "Debug/Debug.h"

#include "Controllers/Controllers.h"

#include "Pipeline/Pipeline.h"
#include "Renderable/Renderable.h"

#include "Noise/Noise.h"

// std
#include <chrono>

#include <cassert>
#include <stdexcept>

namespace Isonia
{
	class Isonia
	{
	public:
		static constexpr const uint32_t width = 1024;
		static constexpr const uint32_t height = 576;
		static constexpr const char* name = "Isonia";

		Isonia();
		~Isonia();

		Isonia(const Isonia&) = delete;
		Isonia& operator=(const Isonia&) = delete;

		void run();

		State::GlobalUbo ubo{};
		State::Clock clock{};

	private:
		void initializeDescriptorPool();
		void initializeCoordinator();
		void initializeRenderSystems();
		void initializeEntities();
		void initializePlayer();

		Pipeline::Descriptors::DescriptorSetLayout* globalSetLayout;
		VkDescriptorSet globalDescriptorSets[Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT];
		Pipeline::Buffer* uboBuffers[Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT];
		Pipeline::Buffer* clockBuffers[Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT];

		Pipeline::Systems::SimpleRenderSystem* simpleRenderSystem;
		Pipeline::Systems::GroundRenderSystem* groundRenderSystem;
		Pipeline::Systems::WaterRenderSystem* waterRenderSystem;
		Pipeline::Systems::DebuggerRenderSystem* debuggerRenderSystem;

		Renderable::Texture* grassDayPalette;
		Renderable::Texture* waterDayPalette;
		Renderable::Texture* grass;
		Renderable::Texture* debugger;
		Renderable::Texture* cloud;

		Renderable::Complete::Model* sphereModel;
		Renderable::Complete::Model* prismModels[20];

		Controllers::Player player{};
		Window::Window window{ width, height, name };
		Pipeline::Device device{ window };
		Pipeline::PixelRenderer renderer{ window, device };
		Pipeline::Descriptors::DescriptorPool* globalPool{};
	};
}
