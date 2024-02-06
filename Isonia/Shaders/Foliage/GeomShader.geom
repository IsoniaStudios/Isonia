#version 450

layout (points) in;

layout (location = 0) in vec3 fragPosWorlds[];
layout (location = 1) in vec3 fragNormalWorlds[];

layout (triangle_strip, max_vertices = 4) out;

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
  float x;
  float z;
} push;

const float SIZE = 0.1;

void main() {
	vec4 position = gl_in[0].gl_Position;
  
	gl_Position = ubo.projection * ubo.view * (position + vec4(-SIZE,  SIZE, 0.0, 0.0));
	fragPosWorld = gl_Position.xyz;
	fragNormalWorld = vec3(0, -1, 0);
	//texcoord = vec2( -1.0, 1.0 );
	EmitVertex();
  
	gl_Position = ubo.projection * ubo.view * (position + vec4(-SIZE, -SIZE, 0.0, 0.0));
	fragPosWorld = gl_Position.xyz;
	fragNormalWorld = vec3(0, -1, 0);
	//texcoord = vec2( -1.0, -1.0 );
	EmitVertex();
  
	gl_Position = ubo.projection * ubo.view * (position + vec4( SIZE,  SIZE, 0.0, 0.0));
	fragPosWorld = gl_Position.xyz;
	fragNormalWorld = vec3(0, -1, 0);
	//texcoord = vec2( 1.0, 1.0 );
	EmitVertex();
  
	gl_Position = ubo.projection * ubo.view * (position + vec4( SIZE, -SIZE, 0.0, 0.0));
	fragPosWorld = gl_Position.xyz;
	fragNormalWorld = vec3(0, -1, 0);
	//texcoord = vec2( 1.0, -1.0 );
	EmitVertex();

	EndPrimitive();
}