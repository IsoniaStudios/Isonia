#version 450

layout (points) in;

layout (triangle_strip, max_vertices = 4) out;

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

const float PIXELS_PER_UNIT = 16.0;
const float SIZE = 32.0 / PIXELS_PER_UNIT;
const float HALF_SIZE = SIZE / 2.0;
const float Y_SCALE = 1.0 / cos(radians(-30.0));

void main()
{
	vec3 worldPosition = gl_in[0].gl_Position.xyz;

	vec3 cameraRightWorld = vec3(ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]) * HALF_SIZE;
	//vec3 cameraUpWorld = vec3(ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]) * HALF_SIZE;
	vec3 upWorld = vec3(0.0, -Y_SCALE, 0.0) * HALF_SIZE;

	gl_Position = ubo.projection * ubo.view * vec4(worldPosition - cameraRightWorld + upWorld, 1.0);
	fragTexCoord = vec2(0.0, 0.0);
	EmitVertex();
  
	gl_Position = ubo.projection * ubo.view * vec4(worldPosition - cameraRightWorld - upWorld, 1.0);
	fragTexCoord = vec2(0.0, 1.0);
	EmitVertex();
  
	gl_Position = ubo.projection * ubo.view * vec4(worldPosition + cameraRightWorld + upWorld, 1.0);
	fragTexCoord = vec2(1.0, 0.0);
	EmitVertex();
  
	gl_Position = ubo.projection * ubo.view * vec4(worldPosition + cameraRightWorld - upWorld, 1.0);
	fragTexCoord = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}