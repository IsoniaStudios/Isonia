#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 frag_texture_coord;

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

void main()
{
    frag_texture_coord = uv;
    gl_Position = vec4(position.xy, 0, 1);
}
