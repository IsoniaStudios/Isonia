#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 fragPosWorld;

layout(location = 0) out vec4 outColor;

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

layout (set = 0, binding = 6) uniform sampler1D colorMap;

void main()
{
	outColor = vec4(texture(colorMap, 0).rgb, 0.1);
}
