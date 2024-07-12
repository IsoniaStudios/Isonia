#version 450
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec2 frag_texture_coord;

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

layout (set = 0, binding = 4) uniform sampler2D texture_map;

void main()
{
	vec4 texture_value = texture(texture_map, frag_texture_coord);
	if (texture_value.a < 1)
		discard;
	out_color = texture_value;
}
