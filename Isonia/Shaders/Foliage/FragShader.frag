#version 450
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec3 fragNormalWorld;
layout (location = 1) in vec2 fragTexCoord;

layout (location = 0) out vec4 outColor;

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

layout (set = 0, binding = 1) uniform sampler1D colorMap;

void main()
{
	float lightIntensity = max(-dot(fragNormalWorld, ubo.lightDirection), ubo.ambientLightColor.w);
	outColor = texture(colorMap, lightIntensity);
}
