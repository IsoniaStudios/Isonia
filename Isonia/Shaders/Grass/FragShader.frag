#version 450
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec3 fragNormalWorld;
layout (location = 1) in float fragCloudShadow;
layout (location = 2) in vec2 fragTexCoord;

layout (location = 0) out vec4 outColor;

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

layout (set = 0, binding = 2) uniform sampler1D colorMap;

layout (set = 0, binding = 3) uniform sampler2D alphaMap;

const float alphaMapSize = 9;

void main()
{
	vec2 scaledFragTexCoord = fragTexCoord / alphaMapSize;
	//outColor = texture(alphaMap, scaledFragTexCoord);
	//return;
	if (texture(alphaMap, scaledFragTexCoord).r < 1)
		discard;

	float directionalLight = -dot(fragNormalWorld, ubo.lightDirection);
	float lightIntensity = max(directionalLight * fragCloudShadow, ubo.ambientLightColor.w);
	outColor = texture(colorMap, lightIntensity);
}
