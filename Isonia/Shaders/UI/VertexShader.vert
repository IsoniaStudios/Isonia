#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 fragTexCoord;

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

void main()
{
    fragTexCoord = uv;
    gl_Position = vec4(position.xy, 0, 1);
}
