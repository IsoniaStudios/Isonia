#pragma once

// internal
#include "../Math/Math.h"

// external
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <iostream>
#include <stdexcept>
#include <cassert>

#include <vector>

namespace Isonia::Pipeline
{
    struct Window
    {
    public:
        Window(int width, int height, char* name);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        const VkExtent2D getExtent() const;
        GLFWwindow* getGLFWWindow() const;

        bool shouldClose() const;
        void resetWindowResizedFlag();
        const bool wasWindowResized() const;

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        
        typedef void (*EventHandler)(Window*);
        void registerCallback(EventHandler handler);
        void propagateEvent();

    private:
        void initWindow();
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        int m_width;
        int m_height;
        bool m_framebuffer_resized = false;

        unsigned int m_handlers_count = 0;
        EventHandler m_handlers[4];

        const char* m_name;
        GLFWwindow* m_window;
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    struct QueueFamilyIndices
    {
        unsigned int graphics_family;
        unsigned int present_family;
        bool graphics_family_has_value = false;
        bool present_family_has_value = false;
        bool isComplete() const
        {
            return graphics_family_has_value && present_family_has_value;
        }
    };

	struct Device
	{
	public:
        Device(Window* window);
        ~Device();

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

        VkDevice getDevice() const;
        VkCommandPool getCommandPool() const;
        VkSurfaceKHR getSurface() const;
        VkQueue getGraphicsQueue() const;
        VkQueue getPresentQueue() const;
        SwapChainSupportDetails getSwapChainSupport();
        QueueFamilyIndices getPhysicalQueueFamilies();

		unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);
		VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer command_buffer);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
        void copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
        void copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layer_count);
        void createImageWithInfo(const VkImageCreateInfo* image_info, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* image_memory);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, unsigned int mip_levels, unsigned int layer_count);

        VkPhysicalDeviceProperties m_properties;

	private:
        VkFormat findSupportedFormat(const std::vector<VkFormat>* candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		std::vector<const char*> getRequiredExtensions();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails findSwapChainSupport(VkPhysicalDevice device);

        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
		void createCommandPool();

		bool isDeviceSuitable(VkPhysicalDevice device);

        Window* m_window;
        
        VkInstance m_instance;
		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		VkCommandPool m_command_pool;
		VkDevice m_device;
		VkSurfaceKHR m_surface;
		VkQueue m_graphics_queue;
		VkQueue m_present_queue;

#ifdef DEBUG
        const std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> m_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        VkDebugUtilsMessengerEXT m_debug_messenger;

        static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger);
        static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);
        void hasGLFWRequiredInstanceExtensions();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* create_info);
        void setupDebugMessenger();
        bool checkValidationLayerSupport();
#endif
	};

    struct Buffer
    {
    public:
        Buffer(Device* device, VkDeviceSize instance_size, unsigned int instance_count, VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_property_flags, VkDeviceSize min_offset_alignment = 1);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void unmap();
        void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void writeToIndex(void* data, int index);
        VkResult flushIndex(int index);
        VkDescriptorBufferInfo descriptorInfoForIndex(int index);
        VkResult invalidateIndex(int index);

        VkBuffer getBuffer() const;
        void* getMappedMemory() const;
        unsigned int getInstanceCount() const;
        VkDeviceSize getInstanceSize() const;
        VkDeviceSize getAlignmentSize() const;
        VkBufferUsageFlags getUsageFlags() const;
        VkMemoryPropertyFlags getMemoryPropertyFlags() const;
        VkDeviceSize getBufferSize() const;

    private:
        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize min_offset_alignment);

        Device* m_device;
        void* m_mapped = nullptr;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_buffer_size;
        unsigned int m_instance_count;
        VkDeviceSize m_instance_size;
        VkDeviceSize m_alignment_size;
        VkBufferUsageFlags m_usage_flags;
        VkMemoryPropertyFlags m_memory_property_flags;
    };

	struct SwapChain
	{
	public:
		static constexpr const unsigned int max_frames_in_flight = 2;

        SwapChain(Device* device_ref, VkExtent2D extent);
        SwapChain(Device* device_ref, VkExtent2D extent, SwapChain* previous);

        ~SwapChain();
        void freeOldSwapChain();

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

        VkFramebuffer getFrameBuffer(int index) const;
        VkRenderPass getRenderPass() const;
        VkImageView getImageView(int index) const;
        unsigned int imageCount() const;
        VkFormat getSwapChainImageFormat() const;
        VkExtent2D getSwapChainExtent() const;
        unsigned int width() const;
        unsigned int height() const;
        float extentAspectRatio() const;

        VkFormat findDepthFormat();
        VkResult acquireNextImage(unsigned int* imageIndex);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* imageIndex);
		bool compareSwapFormats(const SwapChain* swapChain) const;

	private:
		void init();
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createFramebuffers();
		void createDepthResources();
		void createSyncObjects();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>* availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>* availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities) const;

		static constexpr const unsigned int attachmentsLength = 2;

		VkFormat m_swapChainImageFormat;
		VkFormat m_swapChainDepthFormat;
		VkExtent2D m_swapChainExtent;

		std::vector<VkFramebuffer> m_swapChainFramebuffers;
		VkRenderPass m_renderPass;

		std::vector<VkImage> m_depthImages;
		std::vector<VkDeviceMemory> m_depthImageMemorys;
		std::vector<VkImageView> m_depthImageViews;
		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;

		Device* m_device;
		VkExtent2D m_windowExtent;

		VkSwapchainKHR m_swapChain;
		SwapChain* m_oldSwapChain;

		VkSemaphore m_imageAvailableSemaphores[max_frames_in_flight];
		VkSemaphore m_renderFinishedSemaphores[max_frames_in_flight];
		VkFence m_inFlightFences[max_frames_in_flight];
		std::vector<VkFence> m_imagesInFlight;
		unsigned int m_currentFrame = 0;
	};

    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription> binding_descriptions{};
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};
        VkPipelineViewportStateCreateInfo viewport_info;
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
        VkPipelineRasterizationStateCreateInfo rasterization_info;
        VkPipelineMultisampleStateCreateInfo multisample_info;
        VkPipelineColorBlendAttachmentState color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo color_blend_info;
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
        std::vector<VkDynamicState> dynamic_state_enables;
        VkPipelineDynamicStateCreateInfo dynamic_state_info;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        unsigned int subpass = 0;
    };

    struct Pipeline
    {
    public:
        struct Builder
        {
        public:
            Builder(Device& device);

            Builder& addShaderModule(VkShaderStageFlagBits stage, const unsigned char* const code, const size_t size);
            Pipeline* createGraphicsPipeline(const PipelineConfigInfo& config_info) const;

        private:
            VkShaderModule createShaderModule(const unsigned char* const code, const size_t size);

            Device& m_device;
            std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
        };

        Pipeline(Device& device, std::vector<VkPipelineShaderStageCreateInfo> shader_stages, const PipelineConfigInfo& config_info);
        ~Pipeline();

        VkShaderStageFlags getStageFlags() const;

        void bind(VkCommandBuffer command_buffer);

        static constexpr void pixelPipelineTriangleStripConfigInfo(PipelineConfigInfo& config_info);
        static constexpr void pixelPipelineTriangleStripNormalConfigInfo(PipelineConfigInfo& config_info);
        static constexpr void pixelPipelineConfigInfo(PipelineConfigInfo& config_info);
        static constexpr void defaultPipelineConfigInfo(PipelineConfigInfo& config_info);
        static constexpr void makePixelPerfectConfigInfo(PipelineConfigInfo& config_info);
        static constexpr void makeTransparentConfigInfo(PipelineConfigInfo& config_info);
        static constexpr void makeTriangleStripConfigInfo(PipelineConfigInfo& config_info);

    private:
        void create_graphics_pipeline(std::vector<VkPipelineShaderStageCreateInfo> shader_stages, const PipelineConfigInfo& config_info);

        Device& m_device;
        VkPipeline m_graphics_pipeline;
        std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
        VkShaderStageFlags m_stage_flags{};
    };

	struct PixelRenderer
	{
	public:
        PixelRenderer(Window& window, Device& device);
        ~PixelRenderer();

		PixelRenderer(const PixelRenderer&) = delete;
		PixelRenderer& operator=(const PixelRenderer&) = delete;

        void registerRenderResizeCallback(void (*handler)(PixelRenderer*));
        void propigateRenderResizeEvent();
        VkRenderPass getSwapChainRenderPass() const;
        float getAspectRatio() const;
        VkExtent2D getExtent() const;
        bool isFrameInProgress() const;

        VkCommandBuffer getCurrentCommandBuffer() const;
        int getFrameIndex() const;
        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer command_buffer);
        void endSwapChainRenderPass(VkCommandBuffer command_buffer);
        void blit(VkCommandBuffer command_buffer, Math::Vector2 offset);

	protected:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		static void CalculateResolution(VkExtent2D windowExtent, float& outWidth, float& outHeight);
		static VkExtent2D RecalculateCameraSettings(VkExtent2D windowExtent);

		void RecreateSwapChain();

		Window* window;
		Device& device;
		PixelSwapChain* pixelSwapChain = nullptr;
		VkCommandBuffer commandBuffers[PixelSwapChain::max_frames_in_flight];
		std::vector<EventHandler> handlers;

		unsigned int currentImageIndex;
		int currentFrameIndex = 0;
		bool isFrameStarted = false;
	};

	struct PixelSwapChain
	{
	public:
		static constexpr const unsigned int max_frames_in_flight = 2;

        PixelSwapChain(Device& deviceRef, VkExtent2D windowExtent, VkExtent2D renderExtent);
        PixelSwapChain(Device& deviceRef, VkExtent2D windowExtent, VkExtent2D renderExtent, PixelSwapChain* previous);

        ~PixelSwapChain();
        void FreeOldPixelSwapChain();

		PixelSwapChain(const PixelSwapChain&) = delete;
		PixelSwapChain& operator=(const PixelSwapChain&) = delete;

        VkImage GetSwapChainImage(int index) const;
        VkImage GetImage(int index) const;

        VkFramebuffer GetFrameBuffer(int index) const;
        VkRenderPass GetRenderPass() const;
        VkImageView GetImageView(int index) const;
        unsigned int ImageCount() const;

        VkFormat GetPixelSwapChainImageFormat() const;
        VkExtent2D GetPixelSwapChainExtent() const;
        unsigned int SwapChainWidth() const;
        unsigned int SwapChainHeight() const;

        VkExtent2D GetRenderExtent() const;
        unsigned int RenderWidth() const;
        unsigned int RenderHeight() const;
        float ExtentAspectRatio() const;

        VkFormat FindDepthFormat();
        VkResult AcquireNextImage(unsigned int* imageIndex);
		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* imageIndex);
		bool CompareSwapFormats(const PixelSwapChain& swapChain) const;

	private:
		void Init();
		void CreatePixelSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateColorResources();
		void CreateDepthResources();
		void CreateSyncObjects();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

		static constexpr const unsigned int attachmentsLength = 2;

		unsigned int imageCount;

		VkFormat swapChainImageFormat;
		VkFormat swapChainColorFormat;
		VkFormat swapChainDepthFormat;
		VkExtent2D swapChainExtent;

		VkRenderPass renderPass;

		VkFence* imagesInFlight;
		VkImage* depthImages;
		VkDeviceMemory* depthImageMemorys;
		VkImageView* depthImageViews;
		VkImage* colorImages;
		VkDeviceMemory* colorImageMemorys;
		VkImageView* colorImageViews;
		VkImage* swapChainImages;
		VkFramebuffer* swapChainFramebuffers;
		VkImageView* swapChainImageViews;

		Device& device;
		VkExtent2D windowExtent;
		VkExtent2D renderExtent;

		VkSwapchainKHR swapChain;
		PixelSwapChain* oldPixelSwapChain;

		VkSemaphore imageAvailableSemaphores[max_frames_in_flight];
		VkSemaphore renderFinishedSemaphores[max_frames_in_flight];
		VkFence inFlightFences[max_frames_in_flight];
		unsigned int currentFrame = 0;
	};

	struct Renderer
	{
	public:
        Renderer(Window& window, Device& device);
        ~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		void RegisterRenderResizeCallback(void (*handler)(PixelRenderer*))
		{
			handlers.push_back(handler);
		}

        void PropigateRenderResizeEvent();

        VkRenderPass GetSwapChainRenderPass() const;
        float GetAspectRatio() const;
        VkExtent2D GetExtent() const;
        bool IsFrameInProgress() const;
        VkCommandBuffer GetCurrentCommandBuffer() const;
        int GetFrameIndex() const;

        VkCommandBuffer BeginFrame();
        void EndFrame();

        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

	protected:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();

		Window& window;
		Device& device;
		SwapChain* swapChain = nullptr;
		VkCommandBuffer commandBuffers[SwapChain::max_frames_in_flight];
		std::vector<EventHandler> handlers;

		unsigned int currentImageIndex;
		int currentFrameIndex = 0;
		bool isFrameStarted = false;
	};

    struct Camera
    {
    public:
        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);
        void setViewDirection(Math::Vector3 position, Math::Vector3 direction, Math::Vector3 up);
        void setViewTarget(Math::Vector3 position, Math::Vector3 target, Math::Vector3 up);
        virtual void setView(Components::Transform* transform);
        virtual void setProjection(PixelRenderer* renderer);

        const Math::Matrix4x4& getProjection() const;
        const Math::Matrix4x4& getView() const;
        const Math::Matrix4x4& getInverseView() const;
        const Math::Vector3 getForwardVector() const;
        const Math::Vector3 getUpVector() const;
        const Math::Vector3 getRightVector() const;
        const Math::Vector3 getPositionVector() const;

    protected:
        void setViewYXZ(Math::Vector3 position, Math::Vector3 rotation);

        Math::Matrix4x4 m_projection_matrix{ 1.f };
        Math::Matrix4x4 m_inverse_projection_matrix{ 1.f };
        Math::Matrix4x4 m_view_matrix{ 1.f };
        Math::Matrix4x4 m_inverse_view_matrix{ 1.f };
    };

    struct CameraIsometric : public Camera
    {
    public:
        void setView(Components::Transform* transform) override;
        void setProjection(PixelRenderer* renderer) override;

        Math::Vector2 subPixelOffset{};

    private:
        const float camera_distance = 500.0f;
        const float camera_distance_x = 0;
        const float camera_distance_y = -camera_distance * Math::sinf(Math::radiansf(30.0f));
        const float camera_distance_z = -camera_distance * Math::cosf(Math::radiansf(30.0f));

        Math::Vector4 m_pixel_global_top_left{};
    };
}

