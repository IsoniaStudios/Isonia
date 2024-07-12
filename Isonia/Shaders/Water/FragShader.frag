#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 frag_position_world;
layout(location = 1) in vec4 frag_clip_position;

layout(location = 0) out vec4 out_color;

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

layout(push_constant) uniform Push {
  float x;
  float y;
  float z;
} push;

layout (set = 0, binding = 6) uniform sampler1D color_map;
layout (set = 0, binding = 9) uniform sampler2D texture_map;

void main()
{
    vec3 ndc = frag_clip_position.xyz / frag_clip_position.w;
    vec2 screen_uv = (ndc.xy * 0.5) + 0.5;

    vec4 color = texture(texture_map, screen_uv);
    out_color = vec4(color.rgb, 1.0);
}
