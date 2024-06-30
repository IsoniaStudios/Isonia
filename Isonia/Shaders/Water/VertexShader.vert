#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in float amplitude;

layout(location = 0) out vec3 fragPosWorld;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  vec3 lightDirection;
} ubo;

layout(set = 0, binding = 1) uniform GlobalClock {
  float time;
  float frameTime;
} clock;

layout(push_constant) uniform Push {
  float x;
  float y;
  float z;
} push;

const float QUAD_SIZE = 4.0;
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

    fragPosWorld = vec3(col * QUAD_SIZE + push.x, 0, row * QUAD_SIZE + push.z);

    gl_Position = ubo.projection * ubo.view * vec4(fragPosWorld, 1);
}
