#pragma once

// internal
#include "../Noise/Noise.h"
#include "../Pipeline/Pipeline.h"

// external
#include <vulkan/vulkan.h>

// std
#include <vector>

namespace Isonia::Renderable
{
	// Textures
	struct Color
	{
	public:
		constexpr Color(const unsigned char r, const unsigned char g, const unsigned char b);
		constexpr Color(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
		constexpr Color();

		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	struct Texture
	{
	public:
		Texture(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const unsigned int tex_width, const unsigned int tex_height);
		Texture(Pipeline::Device* device, const void* texture, const unsigned int tex_width, const unsigned int tex_height, const VkFormat format);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		VkSampler getSampler() const;
		VkImage getImage() const;
		VkImageView getImageView() const;
		VkDescriptorImageInfo getImageInfo() const;
		VkImageLayout getImageLayout() const;
		VkExtent3D getExtent() const;
		VkFormat getFormat() const;

		static Texture* createTextureFromNoise(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const unsigned int tex_width, const unsigned int tex_height);
		static Texture* createTextureFromPalette(Pipeline::Device* device, const Color* colors, const unsigned int tex_width);
		static Texture* createTexture(Pipeline::Device* device, const void* texture, const unsigned int tex_width, const unsigned int tex_height);

		void updateDescriptor();

	private:
		void createTextureImage(const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const unsigned int texWidth, const unsigned int texHeight);
		void createTextureImage(const void* source, const unsigned int tex_width, const unsigned int tex_height, const VkFormat format);
		void createTextureImageView();
		void createTextureSampler(VkFilter filter, VkSamplerAddressMode addressMode);
		static constexpr const unsigned int formatToBytesPerPixel(const VkFormat imageFormat);

		VkDescriptorImageInfo m_descriptor{};
		Pipeline::Device* m_device;
		VkImage m_texture_image;
		VkDeviceMemory m_texture_image_memory;
		VkImageView m_texture_image_view;
		VkSampler m_texture_sampler;
		VkFormat m_format;
		VkImageLayout m_texture_layout;
		VkImageType m_image_type;
		unsigned int m_bytes_per_pixel;
		unsigned int m_mip_levels{ 1 };
		unsigned int m_layer_count{ 1 };
		VkExtent3D m_extent{};
	};

	static constexpr const unsigned int palette_length = 10;
	extern Texture* createGrassDayPalette(Pipeline::Device* device);
	extern Texture* createGrassNightPalette(Pipeline::Device* device);
	extern Texture* createWaterDayPalette(Pipeline::Device* device);

	extern Texture* createNullTexture(Pipeline::Device* device);
	extern Texture* createDebugTexture(Pipeline::Device* device);

	// Vertices
	struct VertexComplete
	{
		static constexpr std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static constexpr std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		Math::Vector3 position{};
		Math::Vector3 color{};
		Math::Vector3 normal{};
		Math::Vector2 uv{};
	};

	// Primitives
	enum PrimitiveType
	{
		Triangle,
		Quad,
		Pentagon,
		Hexagon,
		TriangularPrism,
		Cube,
		PentagonalPrism,
		HexagonalPrism,
		Icosahedron,
	};

	extern constexpr const std::vector<VertexComplete> generatePrimitiveVertices(const PrimitiveType type);
	extern constexpr const std::vector<unsigned int> generatePrimitiveIndices(const PrimitiveType type);

	extern constexpr const std::vector<VertexComplete> generatePrimitiveFaceVertices(const unsigned int num_sides, const float height = 0.0f);
	extern constexpr const std::vector<unsigned int> generatePrimitiveFaceIndices(const unsigned int num_sides, const unsigned int offset = 0, const bool face_up = true);

	extern constexpr const std::vector<VertexComplete> generatePrimitivePrismVertices(const unsigned int num_sides);
	extern constexpr const std::vector<unsigned int> generatePrimitivePrismIndices(const unsigned int num_sides);

	extern constexpr const std::vector<VertexComplete> generatePrimitiveSphereVertices(const unsigned int sub_divisions);
	extern constexpr const std::vector<unsigned int> generatePrimitiveSphereIndices(const unsigned int sub_divisions);

	struct Model
	{
	public:
		Model(Pipeline::Device* device, const std::vector<VertexComplete>* vertices, const std::vector<unsigned int>* indices);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		static Model* createPrimitive(Pipeline::Device* device, const PrimitiveType primitive);
		static Model* createPrimitiveFace(Pipeline::Device* device, const unsigned int num_sides);
		static Model* createPrimitivePrism(Pipeline::Device* device, const unsigned int num_sides);
		static Model* createPrimitiveSphere(Pipeline::Device* device, const unsigned int sub_divisions);

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

	private:
		void createVertexBuffers(const std::vector<VertexComplete>* vertices);
		void createIndexBuffers(const std::vector<unsigned int>* indices);
		void createVertexBuffers(const VertexComplete* vertices, const unsigned int vertex_count);
		void createIndexBuffers(const unsigned int* indices, const unsigned int indexCount);

		Pipeline::Device* m_device;

		Pipeline::Buffer* m_vertex_buffer;
		unsigned int m_vertex_count;

		Pipeline::Buffer* m_index_buffer;
		unsigned int m_index_count;
	};
}
