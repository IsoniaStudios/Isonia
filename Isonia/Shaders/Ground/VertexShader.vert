#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in float amplitude;
layout(location = 1) in float putch;
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

const int VERTICES = 19;
const int VERTICES_COUNT = VERTICES * VERTICES + (VERTICES - 2) * (VERTICES - 1);

int calculateCol(int index, int strip)
{
    /*
    // 0:
    return index / 2;

    // 1:
    return 9 - ((index + 1) / 2);

    // 2:
    return 9 - index / 2;

    // 3:
    return abs(18 - ((index + 1) / 2));
    */
    return abs((((strip + 1) / 2) * (VERTICES * 2 - 1)) - ((index + (strip % 2)) / 2));
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

    fragPosWorld = vec3(row, amplitude, col);

    float red = fract(sin(fragPosWorld.x * 12.9898 + fragPosWorld.z * 78.233) * 43758.5453);
    float green = fract(sin(fragPosWorld.x * 54.9874 + fragPosWorld.z * 87.156) * 65498.231);
    float blue = fract(sin(fragPosWorld.x * 98.1234 + fragPosWorld.z * 45.678) * 87651.874);

    fragNormalWorld = vec3(red, green, blue);

    // Output the final vertex position
    gl_Position = ubo.projection * ubo.view * vec4(fragPosWorld, 1);
}
