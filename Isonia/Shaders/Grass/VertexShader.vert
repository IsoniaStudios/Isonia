#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 position;
layout(location = 1) in float pitch;
layout(location = 2) in float yaw;
layout(location = 3) in float gain;

layout(location = 0) out vec3 frag_normal_world;
layout(location = 1) out float frag_cloud_shadow;
layout(location = 2) out vec2 frag_wind_texture_coord;

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

layout (set = 2, binding = 0) uniform sampler2D cloud_shadow_map;
layout (set = 2, binding = 1) uniform sampler2D wind_map;

const float CLOUD_HEIGHT = -100.0;
const float ALPHA_MAP_SIZE = 9.0;

void main()
{
    float xz_len = cos(pitch);
    frag_normal_world = vec3(
        xz_len * cos(yaw),
        sin(pitch),
        xz_len * sin(-yaw)
    );

    vec2 wind_direction = vec2(2, 1);

    // calculate the distance to travel along the light direction to reach CLOUD_HEIGHT
    float distance_to_travel = (CLOUD_HEIGHT - position.y) / -ubo.light_direction.y;
    // calculate the position at this distance along the normal
    vec2 cloud_shadow_xz = (position + distance_to_travel * ubo.light_direction).xz;
    // scale to texcoord
    vec2 cloud_shadow_map_texture_coord = (cloud_shadow_xz + clock.time_s * wind_direction) / 32.0;
    // get the shadow intensity
    frag_cloud_shadow = texture(cloud_shadow_map, cloud_shadow_map_texture_coord).r;
    
    // set frag wind to middle
    vec2 wind_map_texture_coord = position.xz / 64.0 + clock.time_s * wind_direction / 16.0;
    frag_wind_texture_coord = floor((1.0 + texture(wind_map, wind_map_texture_coord).xy) * 0.5 * ALPHA_MAP_SIZE) + 0.0001; // why do i need this epsilon

    gl_Position = vec4(position, 1);
}
