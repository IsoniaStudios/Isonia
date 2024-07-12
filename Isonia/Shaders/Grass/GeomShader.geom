#version 450

layout (points) in;

layout (location = 0) in vec3 frag_normal_worlds[];
layout (location = 1) in float frag_cloud_shadows[];
layout (location = 2) in vec2 frag_wind_texture_coords[];

layout (triangle_strip, max_vertices = 4) out;

layout(location = 0) out vec3 frag_normal_world;
layout(location = 1) out float frag_cloud_shadow;
layout(location = 2) out vec2 frag_texture_coord;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 inverse_view;
  vec4 recording_time_elapsed_s;
  vec3 light_direction;
} ubo;

layout(set = 0, binding = 1) uniform GlobalClock {
  float time_s;
  float frame_time_s;
} clock;

const float PIXELS_PER_UNIT = 16.0;
const float SIZE = 16.0 / PIXELS_PER_UNIT;
const float HALF_SIZE = SIZE / 2.0;
const float Y_SCALE = 1.0 / cos(radians(-30.0));

const float ALPHA_MAP_SIZE = 9.0;

void main()
{
	vec3 world_position = gl_in[0].gl_Position.xyz;
	vec3 normal = frag_normal_worlds[0];
	float cloud_shadow = frag_cloud_shadows[0];
	vec2 uv = frag_wind_texture_coords[0];

	//vec3 camera_up_world = vec3(ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]) * HALF_SIZE;
	vec3 camera_right_world = vec3(ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]) * HALF_SIZE;
	vec3 up_world = vec3(0.0, -Y_SCALE * HALF_SIZE, 0.0);

	mat4 view_projection = ubo.projection * ubo.view;
	gl_Position = view_projection * vec4(world_position - camera_right_world + up_world, 1.0);
	frag_normal_world = normal;
	frag_cloud_shadow = cloud_shadow;
	frag_texture_coord = (vec2(0.0, 0.0) + uv) / ALPHA_MAP_SIZE;
	EmitVertex();
  
	gl_Position = view_projection * vec4(world_position - camera_right_world - up_world, 1.0);
	frag_normal_world = normal;
	frag_cloud_shadow = cloud_shadow;
	frag_texture_coord = (vec2(0.0, 1.0) + uv) / ALPHA_MAP_SIZE;
	EmitVertex();
  
	gl_Position = view_projection * vec4(world_position + camera_right_world + up_world, 1.0);
	frag_normal_world = normal;
	frag_cloud_shadow = cloud_shadow;
	frag_texture_coord = (vec2(1.0, 0.0) + uv) / ALPHA_MAP_SIZE;
	EmitVertex();
  
	gl_Position = view_projection * vec4(world_position + camera_right_world - up_world, 1.0);
	frag_normal_world = normal;
	frag_cloud_shadow = cloud_shadow;
	frag_texture_coord = (vec2(1.0, 1.0) + uv) / ALPHA_MAP_SIZE;
	EmitVertex();

	EndPrimitive();
}