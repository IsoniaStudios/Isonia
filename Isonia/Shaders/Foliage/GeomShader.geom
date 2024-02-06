#version 450

layout (points) in;

layout (location = 0) in vec3 fragNormalWorlds[];

layout (triangle_strip, max_vertices = 4) out;

layout(location = 0) out vec3 fragNormalWorld;
layout(location = 1) out vec2 fragTexCoord;

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

const float SIZE = 0.1;
const float yScale = 1.0 / cos(radians(-30.0));

void main() {
	vec3 worldPosition = gl_in[0].gl_Position.xyz;
	vec3 normal = fragNormalWorlds[0];

	vec3 cameraRightWorld = vec3(ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]) * SIZE;
	//vec3 cameraUpWorld = vec3(ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]) * SIZE;
	vec3 upWorld = vec3(0.0, yScale, 0.0) * SIZE;

	gl_Position = ubo.projection * ubo.view * vec4(worldPosition - cameraRightWorld - upWorld, 1.0);
	fragNormalWorld = normal;
	fragTexCoord = vec2(-1.0,  1.0);
	EmitVertex();
  
	gl_Position = ubo.projection * ubo.view * vec4(worldPosition - cameraRightWorld + upWorld, 1.0);
	fragNormalWorld = normal;
	fragTexCoord = vec2(-1.0, -1.0);
	EmitVertex();
  
	gl_Position = ubo.projection * ubo.view * vec4(worldPosition + cameraRightWorld - upWorld, 1.0);
	fragNormalWorld = normal;
	fragTexCoord = vec2( 1.0,  1.0);
	EmitVertex();
  
	gl_Position = ubo.projection * ubo.view * vec4(worldPosition + cameraRightWorld + upWorld, 1.0);
	fragNormalWorld = normal;
	fragTexCoord = vec2( 1.0, -1.0);
	EmitVertex();

	EndPrimitive();
}