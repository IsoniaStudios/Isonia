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

const int VERTICES = 4;
const int VERTICES_COUNT = 2 * VERTICES * VERTICES - 4;

void main()
{
    // Calculate the row and column indices using gl_VertexIndex
    int row = gl_VertexIndex / (2 * VERTICES - 2);
    int col = gl_VertexIndex % (2 * VERTICES - 2);

    // Calculate the position of the vertex in the rectangle
    float x = float(col / 2);
    float y = float(row);

    // Adjust the position to form a rectangle
    x *= 2.0 / (VERTICES - 1.0);
    y *= 2.0 / (VERTICES - 1.0);

    // Alternate between even and odd rows to create a triangle strip
    if (row % 2 == 1) {
        x += 1.0 / (VERTICES - 1.0);
    }

    fragPosWorld = vec3(x, amplitude, y);
    fragNormalWorld = vec3(x / 2.0, amplitude, y / 2.0);

    // Output the final vertex position
    gl_Position = ubo.projection * ubo.view * vec4(fragPosWorld, 1);

    /*
    // Calculate the current strip and vertex indices
    int stripIndex = int(gl_VertexIndex / VERTICES_PER_STRIP);
    int vertexIndex = int(gl_VertexIndex % VERTICES_PER_STRIP);

    // Calculate x and z positions based on strip and vertex indices
    float x = float(vertexIndex % 2 + stripIndex);
    float z = float(vertexIndex / 2);

	fragPosWorld = vec3(x, amplitude, z);
	fragNormalWorld = vec3(x / 2.0, amplitude, z / 2.0);

	gl_Position = ubo.projection * ubo.view * vec4(fragPosWorld, 1);
    */
}
