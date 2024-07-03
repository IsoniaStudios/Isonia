#pragma once

// internal
#include "../Noise/Noise.h"
#include "../Pipeline/Pipeline.h"

// external
#include <vulkan/vulkan.h>

namespace Isonia::Renderable
{
	// Textures
	union Color
	{
	public:
		constexpr Color(const unsigned char r, const unsigned char g, const unsigned char b)
			: r(r), g(g), b(b), a(255) { }
		constexpr Color(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
			: r(r), g(g), b(b), a(a) { }
		constexpr Color(const unsigned char c)
			: r(c), g(c), b(c), a(c) { }
		constexpr Color() { }

		struct
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};
		unsigned int rgba;
	};

	extern Color blendColors(const Color* colors, const unsigned int color_count);

	struct Texture
	{
	public:
		Texture(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const unsigned int tex_width, const unsigned int tex_height);
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

		static Texture* createTextureFromNoise(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const unsigned int tex_width, const unsigned int tex_height);
		static Texture* createTextureFromNoise(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const unsigned int tex_width, const unsigned int tex_height);
		static Texture* createTextureFromPalette(Pipeline::Device* device, const Color* colors, const unsigned int tex_width);
		static Texture* createTexture(Pipeline::Device* device, const void* texture, const unsigned int tex_width, const unsigned int tex_height);

		void updateDescriptor();

	private:
		void createTextureImage(const Noise::VirtualWarpNoise* warp_noise, const unsigned int texWidth, const unsigned int texHeight);
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

	typedef Color (*SubTextureFiller)(const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width);
	extern Color* createTextureAtlas(SubTextureFiller sub_texture_filler, const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width);
	extern Texture* createTextureAtlas(Pipeline::Device* device, SubTextureFiller sub_texture_filler, const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width);
	extern Texture* createTextureAtlasMSAA(Pipeline::Device* device, SubTextureFiller sub_texture_filler, const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width, const unsigned int msaa);

	extern Texture* createGrassTexture(Pipeline::Device* device);

	// Vertices
	struct VertexComplete
	{
		static VkVertexInputBindingDescription* getBindingDescriptions();
		static const unsigned int getBindingDescriptionsCount();
		static VkVertexInputAttributeDescription* getAttributeDescriptions();
		static const unsigned int getAttributeDescriptionsCount();

		Math::Vector3 position{};
		Math::Vector3 color{};
		Math::Vector3 normal{};
		Math::Vector2 uv{};
	};

	struct VertexPosition
	{
		static VkVertexInputBindingDescription* getBindingDescriptions();
		static const unsigned int getBindingDescriptionsCount();
		static VkVertexInputAttributeDescription* getAttributeDescriptions();
		static const unsigned int getAttributeDescriptionsCount();

		Math::Vector3 position;
	};

	struct VertexXZUniform
	{
		static VkVertexInputBindingDescription* getBindingDescriptions();
		static const unsigned int getBindingDescriptionsCount();
		static VkVertexInputAttributeDescription* getAttributeDescriptions();
		static const unsigned int getAttributeDescriptionsCount();

		float altitude;
	};

	struct VertexXZUniformN
	{
		static VkVertexInputBindingDescription* getBindingDescriptions();
		static const unsigned int getBindingDescriptionsCount();
		static VkVertexInputAttributeDescription* getAttributeDescriptions();
		static const unsigned int getAttributeDescriptionsCount();

		float altitude;
		float pitch;
		float yaw;
	};

	struct VertexXZUniformNP
	{
		static VkVertexInputBindingDescription* getBindingDescriptions();
		static const unsigned int getBindingDescriptionsCount();
		static VkVertexInputAttributeDescription* getAttributeDescriptions();
		static const unsigned int getAttributeDescriptionsCount();

		Math::Vector3 position;
		float pitch;
		float yaw;
		float gain;
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

	extern constexpr void generatePrimitiveVertices(VertexComplete* vertices, const PrimitiveType type);
	extern constexpr const VertexComplete* generatePrimitiveVertices(const PrimitiveType type);
	extern constexpr const unsigned int generatePrimitiveVerticesCount(const PrimitiveType type);

	extern constexpr void generatePrimitiveIndices(unsigned int* indices, const PrimitiveType type);
	extern constexpr const unsigned int* generatePrimitiveIndices(const PrimitiveType type);
	extern constexpr const unsigned int generatePrimitiveIndicesCount(const PrimitiveType type);

	extern constexpr void generatePrimitiveFaceVertices(VertexComplete* vertices, const unsigned int num_sides, const float height = 0.0f);
	extern constexpr const VertexComplete* generatePrimitiveFaceVertices(const unsigned int num_sides, const float height = 0.0f);
	extern constexpr const unsigned int generatePrimitiveFaceVerticesCount(const unsigned int num_sides);

	extern constexpr void generatePrimitiveFaceIndices(unsigned int* indices, const unsigned int num_sides, const unsigned int offset = 0, const bool face_up = true);
	extern constexpr const unsigned int* generatePrimitiveFaceIndices(const unsigned int num_sides, const unsigned int offset = 0, const bool face_up = true);
	extern constexpr const unsigned int generatePrimitiveFaceIndicesCount(const unsigned int num_sides);

	extern constexpr void generatePrimitivePrismVertices(VertexComplete* vertices, const unsigned int num_sides);
	extern constexpr const VertexComplete* generatePrimitivePrismVertices(const unsigned int num_sides);
	extern constexpr const unsigned int generatePrimitivePrismVerticesCount(const unsigned int num_sides);

	extern constexpr void generatePrimitivePrismIndices(unsigned int* indices, const unsigned int num_sides);
	extern constexpr const unsigned int* generatePrimitivePrismIndices(const unsigned int num_sides);
	extern constexpr const unsigned int generatePrimitivePrismIndicesCount(const unsigned int num_sides);

	extern constexpr void generatePrimitiveSphereVertices(VertexComplete* vertices, const unsigned int sub_divisions);
	extern constexpr const VertexComplete* generatePrimitiveSphereVertices(const unsigned int sub_divisions);
	extern constexpr const unsigned int generatePrimitiveSphereVerticesCount(const unsigned int sub_divisions);

	extern constexpr void generatePrimitiveSphereIndices(unsigned int* indices, const unsigned int sub_divisions);
	extern constexpr const unsigned int* generatePrimitiveSphereIndices(const unsigned int sub_divisions);
	extern constexpr const unsigned int generatePrimitiveSphereIndicesCount(const unsigned int sub_divisions);

	// Model
	struct Model
	{
	public:
		Model(Pipeline::Device* device, const VertexComplete* vertices, const unsigned int vertices_count, const unsigned int* indices, const unsigned int indices_count);
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
		void createVertexBuffers(const VertexComplete* vertices, const unsigned int vertex_count);
		void createIndexBuffers(const unsigned int* indices, const unsigned int indexCount);

		Pipeline::Device* m_device;

		Pipeline::Buffer* m_vertex_buffer;
		unsigned int m_vertex_count;

		Pipeline::Buffer* m_index_buffer;
		unsigned int m_index_count;
	};

	// Builder
	static constexpr const float quad_size = 1.0f;
	static constexpr const unsigned int quads = 64;
	static constexpr const unsigned int vertices = quads + 1;
	static constexpr const unsigned int unique_vertices_count = vertices * vertices;
	static constexpr const unsigned int sample = vertices + 2;
	static constexpr const unsigned int sample_count = sample * sample;
	static constexpr const unsigned int vertices_count = vertices * vertices + (vertices - 2) * (vertices - 1);
	static constexpr const unsigned int triangle_count = vertices_count - 2;

	static constexpr const float grass_density = 1.0f;
	static constexpr const float grass_size = Math::pixels_per_unit / (16.0f * 2.0f);
	static constexpr const unsigned int grass_count_side = static_cast<unsigned int>(grass_density * static_cast<float>(quads));
	static constexpr const unsigned int grass_count = grass_count_side * grass_count_side;

	struct BuilderPosition
	{
		BuilderPosition(Pipeline::Device* device);
		~BuilderPosition();

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

	private:
		void createVertexBuffers(void* vertices);

		const unsigned int m_point_count;
		Pipeline::Device* m_device;
		Pipeline::Buffer* m_vertex_buffer;
	};

	struct BuilderXZUniform
	{
		BuilderXZUniform(Pipeline::Device* device);
		BuilderXZUniform(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const float amplitude, const Math::Vector3 position);
		~BuilderXZUniform();

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

		Math::Vector3 m_position;

	private:
		int calculateCol(const int index, const int strip) const;
		int calculateRow(const int index, const int strip) const;
		int calculateStrip(const int index) const;

		void createVertexBuffers(void* vertices);

		Pipeline::Device* m_device;
		Pipeline::Buffer* m_vertex_buffer;
	};

	struct BuilderXZUniformN
	{
		BuilderXZUniformN(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const float amplitude, const float x, const float z);
		~BuilderXZUniformN();

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

		float mapWorldToHeight(const float world_x, const float world_z) const;
		Math::Vector3 mapWorldToNormal(const float world_x, const float world_z) const;

		const Math::Vector2 m_positional_data;

	private:
		int calculateCol(const int index, const int strip) const;
		int calculateRow(const int index, const int strip) const;
		int calculateStrip(const int index) const;

		void createVertexBuffers(void* vertices);

		float m_sample_altitudes[sample][sample];
		Math::Vector3 m_normals[vertices][vertices];

		Pipeline::Device* m_device;
		Pipeline::Buffer* m_vertex_buffer;
	};

	struct BuilderXZUniformNP
	{
		BuilderXZUniformNP(Pipeline::Device* device, BuilderXZUniformN* ground);
		~BuilderXZUniformNP();

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

	private:
		void createVertexBuffers(void* vertices);

		Pipeline::Device* m_device;
		Pipeline::Buffer* m_vertex_buffer;
	};
}
