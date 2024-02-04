#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragPosWorld;
layout(location = 1) out vec3 fragNormalWorld;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  vec3 lightDirection;
} ubo;

layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;

void main()
{
	vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);  

	fragPosWorld = positionWorld.xyz;

	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);

	gl_Position = ubo.projection * ubo.view * positionWorld;
}
