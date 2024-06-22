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
				vertices[i].position.y = -10.0f * Math::isometric_y_scale;
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

		Pipeline::Buffer stagingBuffer{
			m_device,
			vertex_size,
			vertex_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer(vertices);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			vertex_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(stagingBuffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
	}

	BuilderXZUniform::BuilderXZUniform(Pipeline::Device* device)
		: m_device(device)
	{
		// alloc memory
		VertexXZUniform* vertices = static_cast<VertexXZUniform*>(operator new[](sizeof(VertexXZUniform)* vertices_count));
		// create the buffers
		createVertexBuffers(vertices);
		// cleanup
		delete vertices;
	}

	BuilderXZUniform::BuilderXZUniform(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const float amplitude, const Math::Vector3 position)
		: m_device(device), m_position(position)
	{
		// alloc memory
		VertexXZUniform* vertices = static_cast<VertexXZUniform*>(operator new[](sizeof(VertexXZUniform)* vertices_count));

		// calculate and assign perlin altitude
		for (unsigned int i = 0; i < vertices_count; i++)
		{
			// calculate strip row and col
			const unsigned int strip = calculateStrip(i);
			const int col = calculateCol(i, strip); // is x
			const int row = calculateRow(i, strip); // is z

			// calculate noise and assign
			float z = row * quad_size + position.z - quad_size;
			float x = col * quad_size + position.x - quad_size;
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
		vkCmdDraw(command_buffer, vertices_count, 1, 0, 0);
	}

	int BuilderXZUniform::calculateCol(const int index, const int strip) const
	{
		return abs(((strip + 1) / 2) * (int(vertices) * 2 - 1) - ((index + (strip % 2)) / 2));
	}
	int BuilderXZUniform::calculateRow(const int index, const int strip) const
	{
		return ((index + strip) % 2) + strip;
	}
	int BuilderXZUniform::calculateStrip(const int index) const
	{
		return (index - 1) / (int(vertices) * 2 - 1);
	}

	void BuilderXZUniform::createVertexBuffers(void* vertices)
	{
		const VkDeviceSize buffer_size = sizeof(VertexXZUniform) * vertices_count;
		const unsigned int vertex_size = sizeof(VertexXZUniform);

		Pipeline::Buffer stagingBuffer{
			m_device,
			vertex_size,
			vertices_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer(vertices);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			vertices_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(stagingBuffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
	}

	BuilderXZUniformN::BuilderXZUniformN(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const float amplitude, const float x, const float z)
		: m_device(device), m_positional_data(x, z)
	{
		// alloc memory
		VertexXZUniformN* vertices = static_cast<VertexXZUniformN*>(operator new[](sizeof(VertexXZUniformN)* vertices_count));

		// calculate perlin
		for (unsigned int i_z = 0; i_z < sample; i_z++)
		{
			for (unsigned int i_x = 0; i_x < sample; i_x++)
			{
				float z = i_z * quad_size + m_positional_data.y - quad_size;
				float x = i_x * quad_size + m_positional_data.x - quad_size;
				warp_noise->transformCoordinate(&x, &z);
				m_sample_altitudes[i_z][i_x] = noise->generateNoise(x, z) * amplitude;
			}
		}

		for (unsigned int z = 1; z < sample - 1; z++)
		{
			for (unsigned int x = 1; x < sample - 1; x++)
			{
				m_normals[z - 1][x - 1] = Math::smoothNormalFromFour(*m_sample_altitudes, quad_size, quad_size, sample, sample, z, x);
			}
		}

		// assign normal and altitude
		for (unsigned int i = 0; i < vertices_count; i++)
		{
			// calculate strip row and col
			const int strip = calculateStrip(i);
			const int col = calculateCol(i, strip); // is x
			const int row = calculateRow(i, strip); // is z

			// get precalculated perlin noise and set it to altitude
			vertices[i].altitude = m_sample_altitudes[row + 1][col + 1];

			// get precalculated perlin normal
			const Math::Vector3 normal = m_normals[row][col];

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
		vkCmdDraw(command_buffer, vertices_count, 1, 0, 0);
	}

	float BuilderXZUniformN::mapWorldToHeight(const float world_x, const float world_z) const
	{
		// get local
		const float local_x = world_x + quad_size - m_positional_data.x;
		const float local_z = world_z + quad_size - m_positional_data.y;

		// map local to indices
		const unsigned int min_x = Math::floorf_i(local_x);
		const unsigned int max_x = Math::ceilf_i(local_x);

		const unsigned int min_z = Math::floorf_i(local_z);
		const unsigned int max_z = Math::ceilf_i(local_z);

		// get heights from indices
		const float h_00 = m_sample_altitudes[min_z][min_x];
		const float h_10 = m_sample_altitudes[min_z][max_x];
		const float h_01 = m_sample_altitudes[max_z][min_x];
		const float h_11 = m_sample_altitudes[max_z][max_x];

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
		const size_t min_x = Math::maxi(Math::floorf_i(local_x), 0);
		const size_t max_x = Math::mini(Math::ceilf_i(local_x), static_cast<int>(vertices));

		const size_t min_z = Math::maxi(Math::floorf_i(local_z), 0);
		const size_t max_z = Math::mini(Math::ceilf_i(local_z), static_cast<int>(vertices));

		// get normals from indices
		const Math::Vector3 n_00 = m_normals[min_z][min_x];
		const Math::Vector3 n_10 = m_normals[min_z][max_x];
		const Math::Vector3 n_01 = m_normals[max_z][min_x];
		const Math::Vector3 n_11 = m_normals[max_z][max_x];

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
		return Math::absi(((strip + 1) / 2) * (int(vertices) * 2 - 1) - ((index + (strip % 2)) / 2));
	}
	int BuilderXZUniformN::calculateRow(const int index, const int strip) const
	{
		return ((index + strip) % 2) + strip;
	}
	int BuilderXZUniformN::calculateStrip(const int index) const
	{
		return (index - 1) / (int(vertices) * 2 - 1);
	}

	void BuilderXZUniformN::createVertexBuffers(void* vertices)
	{
		const VkDeviceSize buffer_size = sizeof(VertexXZUniformN) * vertices_count;
		const unsigned int vertex_size = sizeof(VertexXZUniformN);

		Pipeline::Buffer staging_buffer{
			m_device,
			vertex_size,
			vertices_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		staging_buffer.map();
		staging_buffer.writeToBuffer(vertices);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			vertices_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(staging_buffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
	}

	BuilderXZUniformNP::BuilderXZUniformNP(Pipeline::Device* device, BuilderXZUniformN* ground)
		: m_device(device)
	{
		// alloc memory
		VertexXZUniformNP* vertices = static_cast<VertexXZUniformNP*>(operator new[](sizeof(VertexXZUniformNP)* grass_count));

		const Noise::WhiteNoise offset_noise = {};
		const float size = quad_size / grass_density;
		const float half_size = size * 0.5f;

		for (unsigned int z = 0; z < grass_count_side; z++)
		{
			for (unsigned int x = 0; x < grass_count_side; x++)
			{
				float world_z = z * size + ground->m_positional_data.y;
				float world_x = x * size + ground->m_positional_data.x;
				world_x += offset_noise.generateNoise(world_x, world_z, 0.0f) * half_size;
				world_z += offset_noise.generateNoise(world_x, world_z, 1.0f) * half_size;

				float world_y = ground->mapWorldToHeight(world_x, world_z);
				world_y -= grass_size * Math::isometric_y_scale;

				const Math::Vector3 world_normal = ground->mapWorldToNormal(world_x, world_z);
				const float pitch = Math::atan2f(world_normal.y, world_normal.z);
				const float yaw = Math::atan2f(world_normal.y, world_normal.x);

				const unsigned int i = x + z * grass_count_side;

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
		vkCmdDraw(command_buffer, grass_count, 1, 0, 0);
	}

	void BuilderXZUniformNP::createVertexBuffers(void* vertices)
	{
		const unsigned int vertex_count = static_cast<unsigned int>(grass_count);

		const VkDeviceSize buffer_size = sizeof(VertexXZUniformNP) * grass_count;
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
