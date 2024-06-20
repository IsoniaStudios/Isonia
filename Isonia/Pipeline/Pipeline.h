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
#include <unordered_set>
#include <set>

namespace Isonia::Pipeline
{
    struct Window
    {
    public:
        Window(const unsigned int width, const unsigned int height, const char* name);
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

        unsigned int m_width;
        unsigned int m_height;
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

		unsigned int findMemoryType(unsigned int type_filter, VkMemoryPropertyFlags properties);
		VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer command_buffer);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* buffer_memory);
        void copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
        void copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layer_count);
        void createImageWithInfo(const VkImageCreateInfo* image_info, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* image_memory);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, unsigned int mip_levels, unsigned int layer_count);
        VkFormat findSupportedFormat(const std::vector<VkFormat>* candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        VkPhysicalDeviceProperties m_properties;

	private:
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
        static VkDeviceSize getAlignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment);

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
        unsigned int getImageCount() const;
        VkFormat getSwapChainImageFormat() const;
        VkExtent2D getSwapChainExtent() const;
        unsigned int getWidth() const;
        unsigned int getHeight() const;
        float getExtentAspectRatio() const;

        VkFormat findDepthFormat();
        VkResult acquireNextImage(unsigned int* image_index);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* image_index);
		bool compareSwapFormats(const SwapChain* swap_chain) const;

	private:
		void init();
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createFramebuffers();
		void createDepthResources();
		void createSyncObjects();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>* available_formats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>* available_present_modes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities) const;

		static constexpr const unsigned int attachments_length = 2;

		VkFormat m_swap_chain_image_format;
		VkFormat m_swap_chain_depth_format;
		VkExtent2D m_swap_chain_extent;

		std::vector<VkFramebuffer> m_swap_chain_framebuffers;
		VkRenderPass m_render_pass;

		std::vector<VkImage> m_depth_images;
		std::vector<VkDeviceMemory> m_depth_image_memorys;
		std::vector<VkImageView> m_depth_image_views;
		std::vector<VkImage> m_swap_chain_images;
		std::vector<VkImageView> m_swap_chain_image_views;

		Device* m_device;
		VkExtent2D m_window_extent;

		VkSwapchainKHR m_swap_chain;
		SwapChain* m_old_swap_chain;

		VkSemaphore m_image_available_semaphores[max_frames_in_flight];
		VkSemaphore m_render_finished_semaphores[max_frames_in_flight];
		VkFence m_in_flight_fences[max_frames_in_flight];
		std::vector<VkFence> m_images_in_flight;
		unsigned int m_current_frame = 0;
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
            Builder(Device* device);

            Builder* addShaderModule(VkShaderStageFlagBits stage, const unsigned char* const code, const size_t size);
            Pipeline* createGraphicsPipeline(const PipelineConfigInfo* config_info) const;

        private:
            VkShaderModule createShaderModule(const unsigned char* const code, const size_t size);

            Device* m_device;
            std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
        };

        Pipeline(Device* device, std::vector<VkPipelineShaderStageCreateInfo> shader_stages, const PipelineConfigInfo* config_info);
        ~Pipeline();

        VkShaderStageFlags getStageFlags() const;

        void bind(VkCommandBuffer command_buffer);

        static constexpr void pixelPipelineTriangleStripConfigInfo(PipelineConfigInfo* config_info);
        static constexpr void pixelPipelineTriangleStripNormalConfigInfo(PipelineConfigInfo* config_info);
        static constexpr void pixelPipelineConfigInfo(PipelineConfigInfo* config_info);
        static constexpr void defaultPipelineConfigInfo(PipelineConfigInfo* config_info);
        static constexpr void makePixelPerfectConfigInfo(PipelineConfigInfo* config_info);
        static constexpr void makeTransparentConfigInfo(PipelineConfigInfo* config_info);
        static constexpr void makeTriangleStripConfigInfo(PipelineConfigInfo* config_info);

    private:
        void createGraphicsPipeline(std::vector<VkPipelineShaderStageCreateInfo> shader_stages, const PipelineConfigInfo* config_info);

        Device* m_device;
        VkPipeline m_graphics_pipeline;
        std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
        VkShaderStageFlags m_stage_flags{};
    };

	struct PixelRenderer
	{
	public:
        PixelRenderer(Window* window, Device* device);
        ~PixelRenderer();

		PixelRenderer(const PixelRenderer&) = delete;
		PixelRenderer& operator=(const PixelRenderer&) = delete;

        typedef void (*EventHandler)(PixelRenderer*);
        void registerRenderResizeCallback(EventHandler);
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
		void createCommandBuffers();
		void freeCommandBuffers();
		static void calculateResolution(VkExtent2D windowExtent, float* outWidth, float* outHeight);
		static VkExtent2D recalculateCameraSettings(VkExtent2D windowExtent);

		void recreateSwapChain();

		Window* m_window;
		Device* m_device;
		PixelSwapChain* m_pixel_swap_chain = nullptr;
		VkCommandBuffer m_command_buffers[PixelSwapChain::max_frames_in_flight];
		std::vector<EventHandler> m_handlers;

		unsigned int m_current_image_index;
		int m_current_frame_index = 0;
		bool m_is_frame_started = false;
	};

	struct PixelSwapChain
	{
	public:
		static constexpr const unsigned int max_frames_in_flight = 2;

        PixelSwapChain(Device* device, VkExtent2D window_extent, VkExtent2D render_extent);
        PixelSwapChain(Device* device, VkExtent2D window_extent, VkExtent2D render_extent, PixelSwapChain* previous);

        ~PixelSwapChain();
        void freeOldPixelSwapChain();

		PixelSwapChain(const PixelSwapChain&) = delete;
		PixelSwapChain& operator=(const PixelSwapChain&) = delete;

        VkImage getSwapChainImage(int index) const;
        VkImage getImage(int index) const;

        VkFramebuffer getFrameBuffer(int index) const;
        VkRenderPass getRenderPass() const;
        VkImageView getImageView(int index) const;
        unsigned int getImageCount() const;

        VkFormat getPixelSwapChainImageFormat() const;
        VkExtent2D getPixelSwapChainExtent() const;
        unsigned int getSwapChainWidth() const;
        unsigned int getSwapChainHeight() const;

        VkExtent2D getRenderExtent() const;
        unsigned int getRenderWidth() const;
        unsigned int getRenderHeight() const;
        float getExtentAspectRatio() const;

        VkFormat findDepthFormat();
        VkResult acquireNextImage(unsigned int* image_index);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* image_index);
		bool compareSwapFormats(const PixelSwapChain* swap_chain) const;

	private:
		void init();
		void createPixelSwapChain();
		void createImageViews();
		void createRenderPass();
		void createFramebuffers();
		void createColorResources();
		void createDepthResources();
		void createSyncObjects();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>* available_formats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>* available_present_modes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities) const;

		static constexpr const unsigned int attachments_length = 2;

		unsigned int m_image_count;

		VkFormat m_swap_chain_image_format;
		VkFormat m_swap_chain_color_format;
		VkFormat m_swap_chain_depth_format;
		VkExtent2D m_swap_chain_extent;

		VkRenderPass m_render_pass;

		VkFence* m_images_in_flight;
		VkImage* m_depth_images;
		VkDeviceMemory* m_depth_image_memorys;
		VkImageView* m_depth_image_views;
		VkImage* m_color_images;
		VkDeviceMemory* m_color_image_memorys;
		VkImageView* m_color_image_views;
		VkImage* m_swap_chain_images;
		VkFramebuffer* m_swap_chain_framebuffers;
		VkImageView* m_swap_chain_image_views;

		Device* m_device;
		VkExtent2D m_window_extent;
		VkExtent2D m_render_extent;

		VkSwapchainKHR m_swap_chain;
		PixelSwapChain* m_old_pixel_swap_chain;

		VkSemaphore m_image_available_semaphores[max_frames_in_flight];
		VkSemaphore m_render_finished_semaphores[max_frames_in_flight];
		VkFence m_in_flight_fences[max_frames_in_flight];
		unsigned int m_current_frame = 0;
	};

	struct Renderer
	{
	public:
        Renderer(Window* window, Device* device);
        ~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

        typedef void (*EventHandler)(Renderer*);
        void registerRenderResizeCallback(EventHandler);
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

	protected:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		Window* m_window;
		Device* m_device;
		SwapChain* m_swap_chain = nullptr;
		VkCommandBuffer m_command_buffers[SwapChain::max_frames_in_flight];
		std::vector<EventHandler> m_handlers;

		unsigned int m_current_image_index;
		int m_current_frame_index = 0;
		bool m_is_frame_started = false;
	};

    struct Camera
    {
    public:
        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);
        void setViewDirection(const Math::Vector3* position, const Math::Vector3* direction, const Math::Vector3* up);
        void setViewTarget(const Math::Vector3* position, const Math::Vector3* target, const Math::Vector3* up);
        virtual void setView(Components::Transform* transform);
        virtual void setProjection(PixelRenderer* renderer);

        const Math::Matrix4x4* getProjection() const;
        const Math::Matrix4x4* getView() const;
        const Math::Matrix4x4* getInverseView() const;
        const Math::Vector3 getForwardVector() const;
        const Math::Vector3 getUpVector() const;
        const Math::Vector3 getRightVector() const;
        const Math::Vector3 getPositionVector() const;

    protected:
        void setViewYXZ(const Math::Vector3* position, const Math::Vector3* rotation);

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

        Math::Vector2 m_sub_pixel_offset{};

    private:
        const float camera_distance = 500.0f;
        const float camera_distance_x = 0;
        const float camera_distance_y = -camera_distance * Math::sinf(Math::radiansf(30.0f));
        const float camera_distance_z = -camera_distance * Math::cosf(Math::radiansf(30.0f));

        Math::Vector4 m_pixel_global_top_left{};
    };
}

