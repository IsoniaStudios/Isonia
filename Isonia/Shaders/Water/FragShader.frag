#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec4 frag_clip_position;

layout(location = 0) out vec4 out_color;

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

layout(push_constant) uniform Push {
  float x;
  float y;
  float z;
} push;

layout (set = 1, binding = 0) uniform sampler2D color_map;
layout (set = 1, binding = 1) uniform sampler2D depth_map;
layout (set = 2, binding = 0) uniform sampler1D colors;

vec4 alphaBlend(vec4 src_color, vec4 dst_color)
{
    float alpha = src_color.a;
    vec3 blended_color = src_color.rgb * alpha + dst_color.rgb * (1.0 - alpha);
    return vec4(blended_color, 1.0);
}

void main()
{
    vec3 ndc = frag_clip_position.xyz / frag_clip_position.w;
    vec2 screen_uv = (ndc.xy * 0.5) + 0.5;
    float texture_distance = texture(depth_map, screen_uv).r;
    float water_distance = ndc.z;
    float water_depth = texture_distance - water_distance;
    float water_depth_world = water_depth / ubo.projection[2][2];

    if (water_depth_world < 0.25)
    {
        out_color = vec4(1.0);
        return;
    }

    float water_color_uv = water_depth_world * 0.1;
    vec4 water_color = vec4(texture(colors, water_color_uv).rgb, water_color_uv);
    vec4 under_water_color = texture(color_map, screen_uv);
    out_color = alphaBlend(water_color, under_water_color);
    //out_color = vec4(color.rgb, 1.0);
    //out_color = vec4(water_depth, 0, 0, 1.0);
    //out_color = vec4(color.rgb, water_depth);
}
