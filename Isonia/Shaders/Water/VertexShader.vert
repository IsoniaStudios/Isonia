#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec3 frag_position_world;
layout(location = 1) out vec4 frag_clip_position;

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

const float QUAD_SIZE = 128.0;
void main()
{
    frag_position_world = vec3(
        (gl_VertexIndex / 2) * QUAD_SIZE + push.x,
        push.y,
        (gl_VertexIndex % 2) * QUAD_SIZE + push.z
    );

    frag_clip_position = ubo.projection * ubo.view * vec4(frag_position_world, 1);
    gl_Position = frag_clip_position;
}
