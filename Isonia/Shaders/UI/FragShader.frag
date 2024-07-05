#version 450
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) in vec2 fragTexCoord;

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

layout (set = 0, binding = 8) uniform sampler2D textureMap;

void main()
{
	vec4 textureValue = texture(textureMap, fragTexCoord);
	if (textureValue.a < 1)
		discard;
	outColor = textureValue;
}
