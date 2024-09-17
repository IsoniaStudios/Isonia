#version 450
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec3 frag_position_world;
layout (location = 1) in vec3 frag_normal_world;

layout (location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 inverse_view;
  vec4 recording_time_elapsed_s;
  vec3 light_direction;
  vec2 sub_pixel_offset;
} ubo;

layout(set = 0, binding = 1) uniform GlobalClock {
  float time_s;
  float frame_time_s;
} clock;

layout(push_constant) uniform Push {
  float x;
  float z;
} push;

layout (set = 1, binding = 0) uniform sampler1D colors;

layout (set = 2, binding = 0) uniform sampler2D cloud_shadow_map;

const float CLOUD_HEIGHT = -100;

void main()
{
    vec2 wind_direction = vec2(2, 1);

    // calculate the distance to travel along the light direction to reach CLOUD_HEIGHT
    float distance_to_travel = (CLOUD_HEIGHT - frag_position_world.y) / -ubo.light_direction.y;
    // calculate the position at this distance along the normal
    vec2 cloud_shadow_xz = (frag_position_world + distance_to_travel * ubo.light_direction).xz;
    // scale to texcoord
    vec2 cloud_shadow_map_texture_coord = (cloud_shadow_xz + clock.time_s * wind_direction) / 32.0;
    // get the shadow intensity
    float frag_cloud_shadow = texture(cloud_shadow_map, cloud_shadow_map_texture_coord).r;

	float directional_light = -dot(frag_normal_world, ubo.light_direction);
	float light_intensity = max(directional_light * frag_cloud_shadow, ubo.recording_time_elapsed_s.w);
	out_color = texture(colors, light_intensity);
}
