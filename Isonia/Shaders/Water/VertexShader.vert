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

const float QUAD_SIZE = 128.0;
void main()
{
    fragPosWorld = vec3(
        (gl_VertexIndex / 2) * QUAD_SIZE + push.x,
        push.y,
        (gl_VertexIndex % 2) * QUAD_SIZE + push.z
    );

    gl_Position = ubo.projection * ubo.view * vec4(fragPosWorld, 1);
}
