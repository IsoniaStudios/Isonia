#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in float amplitude;
layout(location = 1) in float pitch;
layout(location = 2) in float yaw;

layout(location = 0) out vec3 frag_position_world;
layout(location = 1) out vec3 frag_normal_world;

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
  float z;
} push;

const float QUAD_SIZE = 1.0;
const int QUADS = 64;
const int VERTICES = QUADS + 1;
const int VERTICES_COUNT = VERTICES * VERTICES + (VERTICES - 2) * (VERTICES - 1);

int calculateCol(int index, int strip)
{
    return abs(((strip + 1) / 2) * (VERTICES * 2 - 1) - ((index + (strip % 2)) / 2));
}
int calculateRow(int index, int strip)
{
    return ((index + strip) % 2) + strip;
}
int calculateStrip(int index)
{
    return (index - 1) / (VERTICES * 2 - 1);
}

void main()
{
    int strip = calculateStrip(gl_VertexIndex);
    int row = calculateRow(gl_VertexIndex, strip);
    int col = calculateCol(gl_VertexIndex, strip);

    frag_position_world = vec3(col * QUAD_SIZE + push.x, amplitude, row * QUAD_SIZE + push.z);

    float xz_len = cos(pitch);
    frag_normal_world = vec3(
        xz_len * cos(yaw),
        sin(pitch),
        xz_len * sin(-yaw)
    );
    
    gl_Position = ubo.projection * ubo.view * vec4(frag_position_world, 1);
}
