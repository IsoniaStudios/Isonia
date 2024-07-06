#pragma once

// internal
#include "../Math/Math.h"
#include "../State/State.h"

// external
#include <vulkan/vulkan.h>

namespace Isonia::Pipeline
{
    struct KeyCodes
    {
        static const constexpr unsigned int a = 0x01E;
        static const constexpr unsigned int d = 0x020;
        static const constexpr unsigned int w = 0x011;
        static const constexpr unsigned int s = 0x01F;

        static const constexpr unsigned int e = 0x012;
        static const constexpr unsigned int q = 0x010;

        static const constexpr unsigned int left_shift = 0x02A;
        static const constexpr unsigned int left_control = 0x01D;
        static const constexpr unsigned int right_shift = 0x036;
        static const constexpr unsigned int right_control = 0x11D;
        static const constexpr unsigned int tab = 0x00F;

        static const constexpr unsigned int left = 0x14B;
        static const constexpr unsigned int right = 0x14D;
        static const constexpr unsigned int up = 0x148;
        static const constexpr unsigned int down = 0x150;
    };
    struct KeyActions
    {
        static const constexpr unsigned char release = 0;
        static const constexpr unsigned char press = 1;
    };
    struct KeyMods
    {
        static const constexpr unsigned int shift = 0x0001;
        static const constexpr unsigned int control = 0x0002;
        static const constexpr unsigned int alt = 0x0004;
        static const constexpr unsigned int super = 0x0008;
        static const constexpr unsigned int caps_lock = 0x0010;
        static const constexpr unsigned int num_lock = 0x0020;
    };

    struct Window
    {
    public:
        Window(unsigned int width, unsigned int height, const char* name);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void inputKey(unsigned int key, unsigned char action);
        unsigned char getKey(unsigned int key) const;
            
        void waitEvents() const;
        void pollEvents();

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

        typedef void (*EventHandler)(Window*);
        void registerCallback(EventHandler handler);
        void propagateEvent();

        VkExtent2D m_extent;
        bool m_resized = false;
        bool m_should_close = false;

    private:
        void createWindow();

        void* m_window_instance;
        void* m_window;

        State::Keyboard m_input{};

        unsigned int m_event_count = 0;
        EventHandler m_handlers[4];

        const char* m_name;
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        VkSurfaceFormatKHR* formats;
        unsigned int formats_count;
        VkPresentModeKHR* present_modes;
        unsigned int present_modes_count;
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

		unsigned int findMemoryType(unsigned int type_filter, VkMemoryPropertyFlags properties) const;
		VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer command_buffer);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* buffer_memory);
        void copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
        void copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layer_count);
        void createImageWithInfo(const VkImageCreateInfo* image_info, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* image_memory);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, unsigned int mip_levels, unsigned int layer_count);
        VkFormat findSupportedFormat(const VkFormat* candidates, const unsigned int candidates_count, VkImageTiling tiling, VkFormatFeatureFlags features) const;

        VkPhysicalDeviceProperties m_properties;

	private:
		const char** getRequiredExtensions(unsigned int* count);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
		SwapChainSupportDetails findSwapChainSupport(VkPhysicalDevice device) const;

        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
		void createCommandPool();

		bool isDeviceSuitable(VkPhysicalDevice device);

        void hasRequiredInstanceExtensions();

        Window* m_window;
        
        VkInstance m_instance;
		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		VkCommandPool m_command_pool;
		VkDevice m_device;
		VkSurfaceKHR m_surface;
		VkQueue m_graphics_queue;
		VkQueue m_present_queue;

        static const constexpr unsigned int m_device_extensions_count = 1u;
        static const constexpr char* m_device_extensions[m_device_extensions_count] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef DEBUG
        static const constexpr unsigned int m_validation_layers_count = 1u;
        static const constexpr char* m_validation_layers[m_validation_layers_count] = { "VK_LAYER_KHRONOS_validation" };
        VkDebugUtilsMessengerEXT m_debug_messenger;

        static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger);
        static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

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

    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkVertexInputBindingDescription* binding_descriptions;
        unsigned int binding_descriptions_count;
        VkVertexInputAttributeDescription* attribute_descriptions;
        unsigned int attribute_descriptions_count;
        VkPipelineViewportStateCreateInfo viewport_info;
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
        VkPipelineRasterizationStateCreateInfo rasterization_info;
        VkPipelineMultisampleStateCreateInfo multisample_info;
        VkPipelineColorBlendAttachmentState color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo color_blend_info;
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
        VkDynamicState* dynamic_state_enables;
        unsigned int dynamic_state_enables_count;
        VkPipelineDynamicStateCreateInfo dynamic_state_info;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        unsigned int subpass = 0;
    };

    struct Pipeline
    {
    public:
        Pipeline(Device* device, const unsigned int shader_stages_count);
        ~Pipeline();

        VkShaderStageFlags getStageFlags() const;

        void bind(VkCommandBuffer command_buffer);

        Pipeline* addShaderModule(VkShaderStageFlagBits stage, const unsigned char* const code, const unsigned int size);

        static void pixelPipelineTriangleStripConfigInfo(PipelineConfigInfo* config_info);
        static void pixelPipelineTriangleStripNormalConfigInfo(PipelineConfigInfo* config_info);
        static void pixelPipelineConfigInfo(PipelineConfigInfo* config_info);
        static void defaultPipelineConfigInfo(PipelineConfigInfo* config_info);
        static void makePixelPerfectConfigInfo(PipelineConfigInfo* config_info);
        static void makeTransparentConfigInfo(PipelineConfigInfo* config_info);
        static void makeTriangleStripConfigInfo(PipelineConfigInfo* config_info);

        Pipeline* createGraphicsPipeline(const PipelineConfigInfo* config_info);

    private:
        VkShaderModule createShaderModule(const unsigned char* const code, const unsigned int size);

        Device* m_device;
        VkPipeline m_graphics_pipeline;
        VkPipelineShaderStageCreateInfo* m_shader_stages;
        const unsigned int m_shader_stages_count;
        unsigned int m_shader_stages_index = 0u;
        VkShaderStageFlags m_stage_flags{};
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

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR* available_formats, const unsigned int available_formats_count);
        VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR* available_present_modes, const unsigned int available_present_modes_count);
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
		PixelSwapChain* m_old_swap_chain;

		VkSemaphore m_image_available_semaphores[max_frames_in_flight];
		VkSemaphore m_render_finished_semaphores[max_frames_in_flight];
		VkFence m_in_flight_fences[max_frames_in_flight];
		unsigned int m_current_frame = 0;
	};

    struct PixelRenderer
    {
    public:
        PixelRenderer(Window* window, Device* device);
        ~PixelRenderer();

        PixelRenderer(const PixelRenderer&) = delete;
        PixelRenderer& operator=(const PixelRenderer&) = delete;

        typedef void (*EventHandler)(PixelRenderer*, void*);
        void registerRenderResizeCallback(EventHandler, void*);
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
        static void calculateResolution(VkExtent2D window_extent, float* out_width, float* out_height, unsigned int* out_render_factor);
        static VkExtent2D recalculateCameraSettings(VkExtent2D window_extent, unsigned int* out_render_factor);

        void recreateSwapChain();

        Window* m_window;
        Device* m_device;
        PixelSwapChain* m_pixel_swap_chain = nullptr;
        VkCommandBuffer m_command_buffers[PixelSwapChain::max_frames_in_flight];

        unsigned int m_event_count = 0;
        EventHandler m_handlers[4];
        void* m_user_data[4];

        unsigned int m_current_image_index;
        unsigned int m_current_frame_index = 0u;
        bool m_is_frame_started = false;
        unsigned int m_render_factor = 1u;
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

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR* available_formats, const unsigned int available_formats_count);
        VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR* available_present_modes, const unsigned int available_present_modes_count);
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
        VkImage* m_swap_chain_images;
        VkFramebuffer* m_swap_chain_framebuffers;
        VkImageView* m_swap_chain_image_views;

        Device* m_device;
        VkExtent2D m_window_extent;

        VkSwapchainKHR m_swap_chain;
        SwapChain* m_old_swap_chain;

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

        typedef void (*EventHandler)(Renderer*, void*);
        void registerRenderResizeCallback(EventHandler, void*);
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

        unsigned int m_event_count = 0;
        EventHandler m_handlers[4];
        void* m_user_data[4];

		unsigned int m_current_image_index;
		unsigned int m_current_frame_index = 0u;
		bool m_is_frame_started = false;
	};

    struct Camera
    {
    public:
        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);
        void setViewDirection(const Math::Vector3* position, const Math::Vector3* direction, const Math::Vector3* up);
        void setViewTarget(const Math::Vector3* position, const Math::Vector3* target, const Math::Vector3* up);
        virtual void setView(Math::Transform* transform);
        void setProjection(Renderer* renderer);

        virtual bool inFustrum(Math::BoundingPlane plane) const;

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
        void setView(Math::Transform* transform) override;
        void setProjection(PixelRenderer* renderer);

        bool inFustrum(Math::BoundingPlane plane) const override;

        Math::Vector2 m_sub_pixel_offset{};

    private:
        //bool intersectsFrustumEdge(const Math::Vector3* start, const Math::Vector3* end) const;
        //bool intersectsPlane(const Math::Vector3* start, const Math::Vector3* end, const Math::Vector3* normal, float d) const;

        float m_left, m_right, m_top, m_bottom, m_near, m_far;

        const float camera_distance = 500.0f;
        const float camera_distance_x = 0;
        const float camera_distance_y = -camera_distance * Math::sinf(Math::radiansf(30.0f));
        const float camera_distance_z = -camera_distance * Math::cosf(Math::radiansf(30.0f));

        Math::Vector4 m_pixel_global_top_left{};
    };
}
