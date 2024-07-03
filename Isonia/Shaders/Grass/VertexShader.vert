#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 position;
layout(location = 1) in float pitch;
layout(location = 2) in float yaw;
layout(location = 3) in float gain;

layout(location = 0) out vec3 fragNormalWorld;
layout(location = 1) out float fragCloudShadow;
layout(location = 2) out vec2 fragWindUV;

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

layout (set = 0, binding = 5) uniform sampler2D cloudShadowMap;
layout (set = 0, binding = 7) uniform sampler2D windMap;

const float CLOUD_HEIGHT = -100.0;

void main()
{
    float xzLen = cos(pitch);
    fragNormalWorld = vec3(
        xzLen * cos(yaw),
        sin(pitch),
        xzLen * sin(-yaw)
    );

    vec2 windDirection = vec2(2, 1);

    // calculate the distance to travel along the light direction to reach CLOUD_HEIGHT
    float distanceToTravel = (CLOUD_HEIGHT - position.y) / -ubo.lightDirection.y;
    // calculate the position at this distance along the normal
    vec2 cloudShadowXZ = (position + distanceToTravel * ubo.lightDirection).xz;
    // scale to texcoord
    vec2 cloudShadowMapTexCoord = (cloudShadowXZ + clock.time * windDirection) / 32.0;
    // get the shadow intensity
    fragCloudShadow = texture(cloudShadowMap, cloudShadowMapTexCoord).r;
    
    // set frag wind to middle
    fragWindUV = texture(windMap, cloudShadowMapTexCoord).xy;

    gl_Position = vec4(position, 1);
}
