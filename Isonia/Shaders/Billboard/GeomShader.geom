#version 450

layout (points) in;

layout (triangle_strip, max_vertices = 4) out;

layout(location = 0) out vec2 frag_texture_coord;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 inverse_view;
  vec4 recording_time_elapsed_s;
  vec3 light_direction;
  vec2 sub_pixel_offset;
} ubo;

layout(set = 0, binding = 1) uniform GlobalClock {
  float time_s;
  float frame_time_s;
} clock;

const float PIXELS_PER_UNIT = 16.0;
const float SIZE = 32.0 / PIXELS_PER_UNIT;
const float HALF_SIZE = SIZE / 2.0;
const float Y_SCALE = 1.0 / cos(radians(-30.0));

void main()
{
	vec3 world_position = gl_in[0].gl_Position.xyz;

	//vec3 camera_up_world = vec3(ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]) * HALF_SIZE;
	vec3 camera_right_world = vec3(ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]) * HALF_SIZE;
	vec3 up_world = vec3(0.0, -Y_SCALE, 0.0) * HALF_SIZE;

	mat4 view_projection = ubo.projection * ubo.view;
	gl_Position = view_projection * vec4(world_position - camera_right_world + up_world, 1.0);
	frag_texture_coord = vec2(0.0, 0.0);
	EmitVertex();
  
	gl_Position = view_projection * vec4(world_position - camera_right_world - up_world, 1.0);
	frag_texture_coord = vec2(0.0, 1.0);
	EmitVertex();
  
	gl_Position = view_projection * vec4(world_position + camera_right_world + up_world, 1.0);
	frag_texture_coord = vec2(1.0, 0.0);
	EmitVertex();
  
	gl_Position = view_projection * vec4(world_position + camera_right_world - up_world, 1.0);
	frag_texture_coord = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}