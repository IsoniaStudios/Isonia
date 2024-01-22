#version 450

layout(location = 0) in float amplitude;
layout(location = 1) in Float16 putch;
layout(location = 2) in Float16 yaw;

layout(location = 0) out vec3 fragPosWorld;
layout(location = 1) out vec3 fragNormalWorld;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  vec3 lightDirection;
} ubo;

void main()
{
	float VERTICES_PER_RUN = 20.0;
	float CLAMPED_VERTICES_PER_RUN = 17.0;

	float rowIndex = mod(gl_InstanceIndex, VERTICES_PER_RUN);
	float clampedIndex = clamp(rowIndex - 1.0, 0.0, CLAMPED_VERTICES_PER_RUN);

	fragPosWorld = vec3(floor(clampedIndex / 2.0), amplitude, mod(clampedIndex, 2.0));
	fragNormalWorld = vec3(0,1,0);

	gl_Position = ubo.projection * ubo.view * positionWorld;
}
