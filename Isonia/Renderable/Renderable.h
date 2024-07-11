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
			: r(c), g(c), b(c), a(255) { }
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
		Texture(Pipeline::Device* device, const void* texture, const unsigned int tex_height, const unsigned int tex_width, const VkFormat format, const VkFilter filter = VK_FILTER_NEAREST, const VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
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

		void updateDescriptor();

	private:
		void createTextureImage(const void* source, const unsigned int tex_height, const unsigned int tex_width, const VkFormat format);
		void createTextureImageView();
		void createTextureSampler(VkFilter filter, VkSamplerAddressMode address_mode);
		static constexpr const unsigned int formatToBytesPerPixel(const VkFormat image_format);

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

	struct TextureFactory
	{
	public:
		TextureFactory(const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride);
		Texture* instantiateTexture(Pipeline::Device* device, const VkFormat format, const VkFilter filter = VK_FILTER_NEAREST, const VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT) const;
		virtual void* createTexture() const;

	protected:
		virtual inline void textureFiller(void* memory, const unsigned int index, const unsigned int t_h, const unsigned int t_w) const = 0;

		virtual inline unsigned int getTextureHeight() const;
		virtual inline unsigned int getTextureWidth() const;
		inline unsigned int getTextureSize() const;

		const unsigned int texture_height;
		const unsigned int texture_width;
		const unsigned char stride;
	};

	struct TextureAtlasFactory : public TextureFactory
	{
	public:
		TextureAtlasFactory(const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride);
		void* createTexture() const override;

	protected:
		virtual inline void textureFiller(void* memory, const unsigned int index, const unsigned int t_h, const unsigned int t_w) const { static_assert("Do not use!"); };
		virtual inline void textureFiller(void* memory, const unsigned int index, const unsigned int a_h, const unsigned int a_w, const unsigned int t_h, const unsigned int t_w) const = 0;

		inline unsigned int getTextureHeight() const override;
		inline unsigned int getTextureWidth() const override;

		const unsigned int atlas_height;
		const unsigned int atlas_width;
	};

	struct GrassTextureAtlasFactory : public TextureAtlasFactory
	{
		GrassTextureAtlasFactory(const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride);
		inline void textureFiller(void* memory, const unsigned int index, const unsigned int a_h, const unsigned int a_w, const unsigned int t_h, const unsigned int t_w) const override;
	};

	struct Noise4DTextureFactory : public TextureFactory
	{
		Noise4DTextureFactory(const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride);
		inline void textureFiller(void* memory, const unsigned int index, const unsigned int t_h, const unsigned int t_w) const override;

	private:
		const Noise::VirtualWarpNoise* warp_noise;
		const Noise::VirtualNoise* noise;
	};

	struct WarpNoiseTextureFactory : public TextureFactory
	{
		WarpNoiseTextureFactory(const Noise::VirtualWarpNoise* warp_noise, const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride);
		inline void textureFiller(void* memory, const unsigned int index, const unsigned int t_h, const unsigned int t_w) const override;

	private:
		const Noise::VirtualWarpNoise* warp_noise;
	};

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

	struct VertexUI
	{
		VertexUI();
		VertexUI(const float x, const float y, const float uv_x, const float uv_y);

		static VkVertexInputBindingDescription* getBindingDescriptions();
		static const unsigned int getBindingDescriptionsCount();
		static VkVertexInputAttributeDescription* getAttributeDescriptions();
		static const unsigned int getAttributeDescriptionsCount();

		Math::Vector2 position;
		Math::Vector2 uv;
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
		void createIndexBuffers(const unsigned int* indices, const unsigned int index_count);

		Pipeline::Device* m_device;

		Pipeline::Buffer* m_vertex_buffer;
		unsigned int m_vertex_count;

		Pipeline::Buffer* m_index_buffer;
		unsigned int m_index_count;
	};

	// Builder
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
		BuilderXZUniform(Pipeline::Device* device, const unsigned int vertices_side_count, const float quad_size);
		BuilderXZUniform(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const float amplitude, const Math::Vector3 position, const unsigned int vertices_side_count, const float quad_size);
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

		const unsigned int m_vertices_side_count;
		const unsigned int m_vertices_count;
		const float m_quad_size;
	};

	struct BuilderXZUniformN
	{
		BuilderXZUniformN(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const float amplitude, const float x, const float z, const unsigned int vertices_side_count, const float quad_size);
		~BuilderXZUniformN();

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

		float mapWorldToHeight(const float world_x, const float world_z) const;
		Math::Vector3 mapWorldToNormal(const float world_x, const float world_z) const;

		const Math::Vector2 m_positional_data;
		bool m_culled = false;

	private:
		int calculateCol(const int index, const int strip) const;
		int calculateRow(const int index, const int strip) const;
		int calculateStrip(const int index) const;

		float* sampleAltitude(const unsigned int i_z, const unsigned int i_x) const;
		Math::Vector3* sampleNormal(const unsigned int i_z, const unsigned int i_x) const;

		void createVertexBuffers(void* vertices);

		Pipeline::Device* m_device;
		Pipeline::Buffer* m_vertex_buffer;

		const unsigned int m_vertices_side_count;
		const unsigned int m_vertices_count;
		const float m_quad_size;

		float* m_sample_altitudes;
		Math::Vector3* m_normals;

		friend struct BuilderXZUniformNP;
	};

	struct BuilderXZUniformNP
	{
		BuilderXZUniformNP(Pipeline::Device* device, BuilderXZUniformN* ground, const float density);
		~BuilderXZUniformNP();

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

		bool m_culled = false;

	private:
		void createVertexBuffers(void* vertices);

		Pipeline::Device* m_device;
		Pipeline::Buffer* m_vertex_buffer;

		const unsigned int m_count_side;
		const unsigned int m_count;
	};

	// Text
	extern Texture* create3x6PixelFontSingleRowTexture(Pipeline::Device* device);
	extern Math::Vector2 sample3x6PixelFontSingleRowTexture(const char c);

	struct BuilderUI
	{
		BuilderUI(Pipeline::Device* device, const unsigned int max_text_length);
		~BuilderUI();

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

		void update(const VkExtent2D extent, const char* text);

	private:
		unsigned int getCharLength(const char* text);

		Pipeline::Device* m_device;

		unsigned int m_previous_char_length = 0u;

		const unsigned int m_max_text_length;

		static const constexpr unsigned int vertices_per_quad = 4u;
		const unsigned int m_vertex_count;
		VertexUI* m_vertices;
		Pipeline::Buffer* m_vertex_buffer;
		Pipeline::Buffer* m_vertex_staging_buffer;

		static const constexpr unsigned int indices_per_quad = 6u;
		const unsigned int m_index_count;
		Pipeline::Buffer* m_index_buffer;
	};
}
