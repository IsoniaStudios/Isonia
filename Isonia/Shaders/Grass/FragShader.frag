#version 450
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec3 frag_normal_world;
layout (location = 1) in float frag_cloud_shadow;
layout (location = 2) in vec2 frag_texture_coord;

layout (location = 0) out vec4 out_color;

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

layout (set = 0, binding = 2) uniform sampler1D colors;

layout (set = 0, binding = 3) uniform sampler2D alpha_map;

void main()
{
	if (texture(alpha_map, frag_texture_coord).r < 1)
		discard;

	float directional_light = -dot(frag_normal_world, ubo.light_direction);
	float light_intensity = max(directional_light * frag_cloud_shadow, ubo.recording_time_elapsed_s.w);
	out_color = texture(colors, light_intensity);
}
