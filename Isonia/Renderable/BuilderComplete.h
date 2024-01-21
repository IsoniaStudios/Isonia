#pragma once

// internal
#include "VertexComplete.h"
#include "Builder.h"

// std
#include <stdexcept>

namespace Isonia::Renderable
{
	struct BuilderComplete
	{
		std::vector<VertexComplete> vertices{};
		std::vector<uint32_t> indices{};

		void LoadModel(const std::string& filepath)
		{
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warn, err;

			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
			{
				throw std::runtime_error(warn + err);
			}

			vertices.clear();
			indices.clear();

			std::unordered_map<VertexComplete, uint32_t> uniqueVertices{};
			for (const auto& shape : shapes)
			{
				for (const auto& index : shape.mesh.indices)
				{
					VertexComplete vertex{};

					if (index.vertex_index >= 0)
					{
						vertex.position = {
							attrib.vertices[3 * index.vertex_index + 0],
							attrib.vertices[3 * index.vertex_index + 1],
							attrib.vertices[3 * index.vertex_index + 2],
						};

						vertex.color = {
							attrib.colors[3 * index.vertex_index + 0],
							attrib.colors[3 * index.vertex_index + 1],
							attrib.colors[3 * index.vertex_index + 2],
						};
					}

					if (index.normal_index >= 0)
					{
						vertex.normal = {
							attrib.normals[3 * index.normal_index + 0],
							attrib.normals[3 * index.normal_index + 1],
							attrib.normals[3 * index.normal_index + 2],
						};
					}

					if (index.texcoord_index >= 0)
					{
						vertex.uv = {
							attrib.texcoords[2 * index.texcoord_index + 0],
							attrib.texcoords[2 * index.texcoord_index + 1],
						};
					}

					if (uniqueVertices.count(vertex) == 0)
					{
						uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
						vertices.push_back(vertex);
					}
					indices.push_back(uniqueVertices[vertex]);
				}
			}
		}
	};
}
