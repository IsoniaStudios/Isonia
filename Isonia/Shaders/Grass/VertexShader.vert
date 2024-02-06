#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 position;
layout(location = 1) in float pitch;
layout(location = 2) in float yaw;
layout(location = 3) in float gain;

layout(location = 0) out vec3 fragNormalWorld;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  vec3 lightDirection;
} ubo;

void main()
{
    float xzLen = cos(pitch);
    fragNormalWorld = vec3(
        xzLen * cos(yaw),
        sin(pitch),
        xzLen * sin(-yaw)
    );
    
    gl_Position = vec4(position, 1);
}
