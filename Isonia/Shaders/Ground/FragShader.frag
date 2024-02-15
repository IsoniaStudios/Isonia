#version 450
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec3 fragPosWorld;
layout (location = 1) in vec3 fragNormalWorld;

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

layout(push_constant) uniform Push {
  float x;
  float z;
} push;

layout (set = 0, binding = 2) uniform sampler1D colorMap;

layout (set = 0, binding = 5) uniform sampler2D cloudShadowMap;

const float CLOUD_HEIGHT = -100;

void main()
{
    vec2 windDirection = vec2(2, 1);

    // calculate the distance to travel along the light direction to reach CLOUD_HEIGHT
    float distanceToTravel = (CLOUD_HEIGHT - fragPosWorld.y) / -ubo.lightDirection.y;
    // calculate the position at this distance along the normal
    vec2 cloudShadowXZ = (fragPosWorld + distanceToTravel * ubo.lightDirection).xz;
    // scale to texcoord
    vec2 cloudShadowMapTexCoord = (cloudShadowXZ + clock.time * windDirection) / 32.0;
    // get the shadow intensity
    float fragCloudShadow = texture(cloudShadowMap, cloudShadowMapTexCoord).r;

	float directionalLight = -dot(fragNormalWorld, ubo.lightDirection);
	float lightIntensity = max(directionalLight * fragCloudShadow, ubo.ambientLightColor.w);
	outColor = texture(colorMap, lightIntensity);
}
