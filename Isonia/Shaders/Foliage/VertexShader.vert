#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in float amplitude;
layout(location = 1) in float pitch;
layout(location = 2) in float yaw;

layout(location = 0) out vec3 fragPosWorld;
layout(location = 1) out vec3 fragNormalWorld;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  vec3 lightDirection;
} ubo;

layout(push_constant) uniform Push {
  float x;
  float z;
} push;

const float QUAD_SIZE = 0.25;
const int QUADS = 256;
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

    fragPosWorld = vec3(row * QUAD_SIZE + push.x, amplitude, col * QUAD_SIZE + push.z);

    float xzLen = cos(pitch);
    fragNormalWorld = vec3(
        xzLen * cos(yaw),
        sin(pitch),
        xzLen * sin(-yaw)
    );
    
    gl_Position = ubo.projection * ubo.view * vec4(fragPosWorld, 1);
}
