// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	BuilderPosition::BuilderPosition(Pipeline::Device* device) : m_device(device), m_point_count(7 * 7)
	{
		// alloc memory
		VertexPosition* vertices = static_cast<VertexPosition*>(operator new[](sizeof(VertexPosition)* m_point_count));

		for (int z = -3; z <= 3; z++)
		{
			for (int x = -3; x <= 3; x++)
			{
				const int i = (x + 3) + (z + 3) * 7;
				vertices[i].position.x = static_cast<float>(x * 64) * Math::units_per_pixel;
				vertices[i].position.y = -1.0f * Math::isometric_y_scale;
				vertices[i].position.z = static_cast<float>(z * 64) * Math::units_per_pixel;
			}
		}

		// create the buffers
		createVertexBuffers(vertices);

		// free
		delete vertices;
	}

	BuilderPosition::~BuilderPosition()
	{
		delete m_vertex_buffer;
	}

	void BuilderPosition::bind(VkCommandBuffer command_buffer)
	{
		VkBuffer buffers[] = { m_vertex_buffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
	}

	void BuilderPosition::draw(VkCommandBuffer command_buffer)
	{
		vkCmdDraw(command_buffer, static_cast<unsigned int>(m_point_count), 1, 0, 0);
	}

	void BuilderPosition::createVertexBuffers(void* vertices)
	{
		const unsigned int vertex_count = static_cast<unsigned int>(m_point_count);

		const VkDeviceSize buffer_size = sizeof(VertexPosition) * m_point_count;
		const unsigned int vertex_size = sizeof(VertexPosition);

		Pipeline::Buffer staging_buffer{
			m_device,
			vertex_size,
			vertex_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		staging_buffer.map();
		staging_buffer.writeToBuffer(vertices);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			vertex_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(staging_buffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
	}

	BuilderXZUniform::BuilderXZUniform(Pipeline::Device* device, const unsigned int vertices_side_count, const float quad_size)
		: m_device(device), m_vertices_side_count(vertices_side_count), m_vertices_count(vertices_side_count * vertices_side_count + (vertices_side_count - 2) * (vertices_side_count - 1)), m_quad_size(quad_size)
	{
		// alloc memory
		VertexXZUniform* vertices = static_cast<VertexXZUniform*>(operator new[](sizeof(VertexXZUniform) * m_vertices_count));
		// create the buffers
		createVertexBuffers(vertices);
		// cleanup
		delete vertices;
	}

	BuilderXZUniform::BuilderXZUniform(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const float amplitude, const Math::Vector3 position, const unsigned int vertices_side_count, const float quad_size)
		: m_device(device), m_position(position), m_vertices_side_count(vertices_side_count), m_vertices_count(vertices_side_count * vertices_side_count + (vertices_side_count - 2) * (vertices_side_count - 1)), m_quad_size(quad_size)
	{
		// alloc memory
		VertexXZUniform* vertices = static_cast<VertexXZUniform*>(operator new[](sizeof(VertexXZUniform) * m_vertices_count));

		// calculate and assign perlin altitude
		for (unsigned int i = 0; i < m_vertices_count; i++)
		{
			// calculate strip row and col
			const unsigned int strip = calculateStrip(i);
			const int col = calculateCol(i, strip); // is x
			const int row = calculateRow(i, strip); // is z

			// calculate noise and assign
			float z = row * m_quad_size + position.z - m_quad_size;
			float x = col * m_quad_size + position.x - m_quad_size;
			warp_noise->transformCoordinate(&x, &z);
			vertices[i].altitude = noise->generateNoise(x, z) * amplitude;
		}

		// create the buffers
		createVertexBuffers(vertices);

		// cleanup
		delete vertices;
	}

	BuilderXZUniform::~BuilderXZUniform()
	{
		delete m_vertex_buffer;
	}

	void BuilderXZUniform::bind(VkCommandBuffer command_buffer)
	{
		VkBuffer buffers[] = { m_vertex_buffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
	}

	void BuilderXZUniform::draw(VkCommandBuffer command_buffer)
	{
		vkCmdDraw(command_buffer, m_vertices_count, 1, 0, 0);
	}

	int BuilderXZUniform::calculateCol(const int index, const int strip) const
	{
		return Math::absi(((strip + 1) / 2) * (int(m_vertices_side_count) * 2 - 1) - ((index + (strip % 2)) / 2));
	}
	int BuilderXZUniform::calculateRow(const int index, const int strip) const
	{
		return ((index + strip) % 2) + strip;
	}
	int BuilderXZUniform::calculateStrip(const int index) const
	{
		return (index - 1) / (int(m_vertices_side_count) * 2 - 1);
	}

	void BuilderXZUniform::createVertexBuffers(void* vertices)
	{
		const VkDeviceSize buffer_size = sizeof(VertexXZUniform) * m_vertices_count;
		const unsigned int vertex_size = sizeof(VertexXZUniform);

		Pipeline::Buffer staging_buffer{
			m_device,
			vertex_size,
			m_vertices_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		staging_buffer.map();
		staging_buffer.writeToBuffer(vertices);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			m_vertices_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(staging_buffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
	}

	BuilderXZUniformN::BuilderXZUniformN(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const float amplitude, const float x, const float z, const unsigned int vertices_side_count, const float quad_size)
		: m_device(device), m_positional_data(x, z), m_vertices_side_count(vertices_side_count), m_vertices_count(vertices_side_count * vertices_side_count + (vertices_side_count - 2) * (vertices_side_count - 1)), m_quad_size(quad_size)
	{
		// alloc memory
		VertexXZUniformN* vertices = static_cast<VertexXZUniformN*>(operator new[](sizeof(VertexXZUniformN) * m_vertices_count));

		const unsigned int sample = m_vertices_side_count + 2;
		m_sample_altitudes = (float*)malloc(sample * sample * sizeof(float));
		// calculate perlin
		for (unsigned int i_z = 0; i_z < sample; i_z++)
		{
			for (unsigned int i_x = 0; i_x < sample; i_x++)
			{
				float z = i_z * m_quad_size + m_positional_data.y - m_quad_size;
				float x = i_x * m_quad_size + m_positional_data.x - m_quad_size;
				warp_noise->transformCoordinate(&x, &z);
				*sampleAltitude(i_z, i_x) = noise->generateNoise(x, z) * amplitude;
			}
		}

		m_normals = (Math::Vector3*)malloc(m_vertices_side_count * m_vertices_side_count * sizeof(Math::Vector3));
		for (unsigned int z = 1; z < sample - 1; z++)
		{
			for (unsigned int x = 1; x < sample - 1; x++)
			{
				*sampleNormal(z - 1, x - 1) = Math::smoothNormalFromFour(m_sample_altitudes, m_quad_size, m_quad_size, sample, sample, z, x);
			}
		}

		// assign normal and altitude
		for (unsigned int i = 0; i < m_vertices_count; i++)
		{
			// calculate strip row and col
			const int strip = calculateStrip(i);
			const int col = calculateCol(i, strip); // is x
			const int row = calculateRow(i, strip); // is z

			// get precalculated perlin noise and set it to altitude
			vertices[i].altitude = *sampleAltitude(row + 1, col + 1);

			// get precalculated perlin normal
			const Math::Vector3 normal = *sampleNormal(row, col);

			// calculate pitch and yaw
			vertices[i].pitch = Math::atan2f(normal.y, normal.z);
			vertices[i].yaw = Math::atan2f(normal.y, normal.x);
		}

		// create the buffers
		createVertexBuffers(vertices);

		// cleanup
		delete vertices;
	}

	BuilderXZUniformN::~BuilderXZUniformN()
	{
		delete m_vertex_buffer;
	}

	void BuilderXZUniformN::bind(VkCommandBuffer command_buffer)
	{
		VkBuffer buffers[] = { m_vertex_buffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
	}

	void BuilderXZUniformN::draw(VkCommandBuffer command_buffer)
	{
		vkCmdDraw(command_buffer, m_vertices_count, 1, 0, 0);
	}

	float* BuilderXZUniformN::sampleAltitude(const unsigned int i_z, const unsigned int i_x) const
	{
		return m_sample_altitudes + i_z * (m_vertices_side_count + 2u) + i_x;
	}

	Math::Vector3* BuilderXZUniformN::sampleNormal(const unsigned int i_z, const unsigned int i_x) const
	{
		return m_normals + i_z * m_vertices_side_count + i_x;
	}

	float BuilderXZUniformN::mapWorldToHeight(const float world_x, const float world_z) const
	{
		// get local
		const float local_x = world_x + m_quad_size - m_positional_data.x;
		const float local_z = world_z + m_quad_size - m_positional_data.y;

		// map local to indices
		const unsigned int min_x = Math::floorf_i(local_x);
		const unsigned int max_x = Math::ceilf_i(local_x);

		const unsigned int min_z = Math::floorf_i(local_z);
		const unsigned int max_z = Math::ceilf_i(local_z);

		// get heights from indices
		const float h_00 = *sampleAltitude(min_z, min_x);
		const float h_10 = *sampleAltitude(min_z, max_x);
		const float h_01 = *sampleAltitude(max_z, min_x);
		const float h_11 = *sampleAltitude(max_z, max_x);

		// get t for lerp
		const float t_x = local_x - min_x;
		const float t_z = local_z - min_z;

		// lerp between and return
		const float h_l_0 = Math::lerpf(h_00, h_10, t_x);
		const float h_l_1 = Math::lerpf(h_01, h_11, t_x);

		return Math::lerpf(h_l_0, h_l_1, t_z);
	}

	Math::Vector3 BuilderXZUniformN::mapWorldToNormal(const float world_x, const float world_z) const
	{
		// get local
		const float local_x = world_x - m_positional_data.x;
		const float local_z = world_z - m_positional_data.y;

		// map local to indices
		//const int min_x = Math::floorf_i(local_x);
		//const int max_x = Math::ceilf_i(local_x);

		//const int min_z = Math::floorf_i(local_z);
		//const int max_z = Math::ceilf_i(local_z);
		const int min_x = Math::maxi(Math::floorf_i(local_x), 0);
		const int max_x = Math::mini(Math::ceilf_i(local_x), static_cast<int>(m_vertices_side_count));

		const int min_z = Math::maxi(Math::floorf_i(local_z), 0);
		const int max_z = Math::mini(Math::ceilf_i(local_z), static_cast<int>(m_vertices_side_count));

		// get normals from indices
		const Math::Vector3 n_00 = *sampleNormal(min_z, min_x);
		const Math::Vector3 n_10 = *sampleNormal(min_z, max_x);
		const Math::Vector3 n_01 = *sampleNormal(max_z, min_x);
		const Math::Vector3 n_11 = *sampleNormal(max_z, max_x);

		// get t for lerp
		const float t_x = local_x - min_x;
		const float t_z = local_z - min_z;

		// lerp between and return
		const Math::Vector3 n_l_0 = Math::lerpv3(&n_00, &n_10, t_x);
		const Math::Vector3 n_l_1 = Math::lerpv3(&n_01, &n_11, t_x);

		return Math::lerpv3(&n_l_0, &n_l_1, t_z);
	}

	int BuilderXZUniformN::calculateCol(const int index, const int strip) const
	{
		return Math::absi(((strip + 1) / 2) * (int(m_vertices_side_count) * 2 - 1) - ((index + (strip % 2)) / 2));
	}
	int BuilderXZUniformN::calculateRow(const int index, const int strip) const
	{
		return ((index + strip) % 2) + strip;
	}
	int BuilderXZUniformN::calculateStrip(const int index) const
	{
		return (index - 1) / (int(m_vertices_side_count) * 2 - 1);
	}

	void BuilderXZUniformN::createVertexBuffers(void* vertices)
	{
		const VkDeviceSize buffer_size = sizeof(VertexXZUniformN) * m_vertices_count;
		const unsigned int vertex_size = sizeof(VertexXZUniformN);

		Pipeline::Buffer staging_buffer{
			m_device,
			vertex_size,
			m_vertices_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		staging_buffer.map();
		staging_buffer.writeToBuffer(vertices);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			m_vertices_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(staging_buffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
	}

	BuilderXZUniformNP::BuilderXZUniformNP(Pipeline::Device* device, BuilderXZUniformN* ground, const float density)
		: m_device(device), m_count_side(static_cast<unsigned int>(density* static_cast<float>(ground->m_vertices_side_count - 1u))), m_count(m_count_side * m_count_side)
	{
		// alloc memory
		VertexXZUniformNP* vertices = static_cast<VertexXZUniformNP*>(operator new[](sizeof(VertexXZUniformNP) * m_count));

		const Noise::WhiteNoise offset_noise = {};
		const float size = ground->m_quad_size / density;
		const float half_size = size * 0.33f;

		for (unsigned int z = 0; z < m_count_side; z++)
		{
			for (unsigned int x = 0; x < m_count_side; x++)
			{
				float world_z = z * size + ground->m_positional_data.y;
				float world_x = x * size + ground->m_positional_data.x;
				world_x += offset_noise.generateNoise(world_x, world_z, 0.0f) * half_size;
				world_z += offset_noise.generateNoise(world_x, world_z, 1.0f) * half_size;

				float world_y = ground->mapWorldToHeight(world_x, world_z);
				world_y -= (Math::pixels_per_unit * Math::isometric_y_scale) / (16.0f * 2.0f);

				const Math::Vector3 world_normal = ground->mapWorldToNormal(world_x, world_z);
				const float pitch = Math::atan2f(world_normal.y, world_normal.z);
				const float yaw = Math::atan2f(world_normal.y, world_normal.x);

				const unsigned int i = x + z * m_count_side;

				vertices[i].pitch = pitch;
				vertices[i].yaw = yaw;
				vertices[i].position = Math::Vector3(
					world_x,
					world_y,
					world_z
				);
				vertices[i].gain = 0.0f;
			}
		}

		// create the buffers
		createVertexBuffers(vertices);

		// free
		delete vertices;
	}

	BuilderXZUniformNP::~BuilderXZUniformNP()
	{
		delete m_vertex_buffer;
	}

	void BuilderXZUniformNP::bind(VkCommandBuffer command_buffer)
	{
		VkBuffer buffers[] = { m_vertex_buffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
	}

	void BuilderXZUniformNP::draw(VkCommandBuffer command_buffer)
	{
		vkCmdDraw(command_buffer, m_count, 1, 0, 0);
	}

	void BuilderXZUniformNP::createVertexBuffers(void* vertices)
	{
		const unsigned int vertex_count = static_cast<unsigned int>(m_count);

		const VkDeviceSize buffer_size = sizeof(VertexXZUniformNP) * m_count;
		const unsigned int vertex_size = sizeof(VertexXZUniformNP);

		Pipeline::Buffer staging_buffer{
			m_device,
			vertex_size,
			vertex_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		staging_buffer.map();
		staging_buffer.writeToBuffer(vertices);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			vertex_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(staging_buffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
	}
}
