#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 frag_clip_position;

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
  float y;
  float z;
} push;

const float QUAD_SIZE = 128.0;
void main()
{
    vec4 frag_position_world = vec4(
        (gl_VertexIndex / 2) * QUAD_SIZE + push.x,
        push.y,
        (gl_VertexIndex % 2) * QUAD_SIZE + push.z,
        1.0
    );

    frag_clip_position = ubo.projection * ubo.view * frag_position_world;
    gl_Position = frag_clip_position;
}
