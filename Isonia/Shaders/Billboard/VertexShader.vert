#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 position;

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

void main()
{
    gl_Position = vec4(position, 1);
}
