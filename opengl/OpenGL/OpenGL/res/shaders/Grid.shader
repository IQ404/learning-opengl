#shader vertex
#version 460 core

layout (location = 0) in vec4 position;	// right now this position is the position in local (object) space in right-handed coordinates.
/*
Note that currently location 0 actually feeds in 2 floats.
It is predefined that OpenGL will set the 1st and 2nd components of the vec4 to these two floats respectively,
and set the 3rd and 4th components of the vec4 to 0.0f (This is based on the assumption that if Z is not provided, the vertex is meant to be in a plane) and
1.0f (implies a homogeneous position in space as opposed to a homogeneous direction vector, which would have set to 0.0f) respectively.
*/

layout (location = 1) in vec2 uv;

uniform mat4 u_MVP;
out vec2 v_UV;

void main()
{
    gl_Position = u_MVP * position;	// Note that position is right-handed, gl_Position is left-handed
    v_UV = uv;
}

#shader fragment
#version 460 core

//uniform vec2 window_resolution;   // width, height

in vec2 v_UV;
layout (location = 0) out vec4 final_color;

vec3 red = vec3(1.0, 0.0, 0.0);
vec3 blue = vec3(0.0, 0.0, 1.0);
vec3 green = vec3(0.0, 1.0, 0.0);
vec3 yellow = vec3(1.0, 1.0, 0.0);
vec3 light_blue = vec3(0.0, 1.0, 1.0);
vec3 purple = vec3(0.91, 0.0, 1.0);
vec3 pink = vec3(1.0, 0.0, 0.745);
vec3 white = vec3(1.0, 1.0, 1.0);
vec3 black = vec3(0.0, 0.0, 0.0);

void main()
{
    // Background Color:

    vec3 color = vec3(0.75);

    // Grid:

    float cells_per_dimension = 10.0;
    //float pixels_per_cell = 100.0;

    vec2 uv_origin_centered = v_UV - 0.5;
    vec2 uv_in_cell = fract(uv_origin_centered * cells_per_dimension);  // note that the negative part is also returned as x - floor(x) where floor(x) <= x. Hence, after this fract, every cell's local orgin is at bottom-left.
    //vec2 uv_in_cell = fract(uv_orgin_centered * window_resolution / pixels_per_cell);

    vec2 absolute_centered_uv_in_cell = abs(uv_in_cell - 0.5);
    float at_layer = max(absolute_centered_uv_in_cell.x, absolute_centered_uv_in_cell.y);
    float at_layer_with_color_normalized_flipped = 1.0 - 2.0 * at_layer;

    float grid = smoothstep(0.0, 0.05, at_layer_with_color_normalized_flipped);
    float x_axis = smoothstep(0.0, 0.002, abs(v_UV.y - 0.5));   // we chose a much smaller thickness because the drawing of axes is done per quad, not per-cell.
    float y_axis = smoothstep(0.0, 0.002, abs(v_UV.x - 0.5));

    // Lines:

    vec2 pos = uv_origin_centered * cells_per_dimension;
    // if real-time window_resolution is implemented:
    //vec2 pos = uv_origin_centered * resolution / pixels_per_cell;
    
    float y_equals_x = pos.x;
    float y_abs = abs(pos.x);
    float y_floor = floor(pos.x);
    float y_ceil = ceil(pos.x);
    // I'm not drawing round() here because it would be a mess (it's already is)
    float y_mod = mod(pos.x, 3.14); // 3.14 is just a random choice
    float y_fract = fract(pos.x);

    float function_y_equals_x = smoothstep(0.0, 0.055, abs(pos.y - y_equals_x));    // the deviation along y-component
    float function_abs = smoothstep(0.0, 0.055, abs(pos.y - y_abs));
    float function_floor = smoothstep(0.0, 0.055, abs(pos.y - y_floor));
    float function_ceil = smoothstep(0.0, 0.055, abs(pos.y - y_ceil));
    float function_mod = smoothstep(0.0, 0.055, abs(pos.y - y_mod));
    float function_fract = smoothstep(0.0, 0.055, abs(pos.y - y_fract));

    // Drawing:

    color = mix(black, color, grid);
    color = mix(blue, color, x_axis);
    color = mix(blue, color, y_axis);
    color = mix(yellow, color, function_y_equals_x);
    color = mix(red, color, function_abs);
    color = mix(green, color, function_floor);
    color = mix(light_blue, color, function_ceil);
    color = mix(purple, color, function_mod);
    color = mix(pink, color, function_fract);

    final_color = vec4(color, 1.0);
}