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

uniform vec2 u_WindowResolution;   // width, height
uniform float u_TimePassed;
uniform float u_SignedDistance;
uniform bool u_Soft;
uniform float u_MovableCircleX;

in vec2 v_UV;
layout (location = 0) out vec4 final_color;

vec3 yellow = vec3(1.0, 1.0, 0.5);
vec3 blue = vec3(0.25, 0.25, 1.0);
vec3 red = vec3(1.0, 0.25, 0.25);
vec3 green = vec3(0.25, 1.0, 0.25);
vec3 purple = vec3(1.0, 0.25, 1.0);

float inverseLerp(float value, float minValue, float maxValue)
{
    return (value - minValue) / (maxValue - minValue);
}

float remap(float value, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(value, inMin, inMax);
    return mix(outMin, outMax, t);
}

vec3 BackgroundColor()
{
    float distance_from_center = length(abs(v_UV - 0.5));
    float color_distance = smoothstep(0.3, 1.0, distance_from_center);  // make the full-white area larger
    float vignette = 1.0 - color_distance;
    vignette = remap(vignette, 0.0, 1.0, 0.3, 1.0); // just make vignette brighter

    return vec3(vignette);
}

vec3 DrawGrid(vec3 destination_color, vec3 line_color, float pixels_per_cell, float grid_thickness_in_pixels)
{
    vec2 uv_origin_centered = v_UV - 0.5;
    vec2 uv_in_cell = fract(uv_origin_centered * u_WindowResolution / pixels_per_cell);  // note that the negative part is also returned as x - floor(x) where floor(x) <= x. Hence, after this fract, every cell's local orgin is at bottom-left.
    vec2 absolute_centered_uv_in_cell = abs(uv_in_cell - 0.5);
    float at_layer = max(absolute_centered_uv_in_cell.x, absolute_centered_uv_in_cell.y);
    float at_layer_absolute_pixel_distance_to_grid = (0.5 - at_layer) * pixels_per_cell;
    float grid = smoothstep(0.0, grid_thickness_in_pixels, at_layer_absolute_pixel_distance_to_grid);
    vec3 destination_color_new = mix(line_color, destination_color, grid);

    return destination_color_new;
}

float SdfCircle(vec2 fragment_coordinates_in_pixel, vec2 circle_center_coordinates_in_pixel, float radius_in_pixel)
{
    return length(fragment_coordinates_in_pixel - circle_center_coordinates_in_pixel) - radius_in_pixel;
}

float SdfLineSegment(vec2 p, vec2 a, vec2 b)
{
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);

    return length(pa - ba * h);
}

float SdfBox(vec2 p, vec2 b)
{
    vec2 d = abs(p) - b;
    
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float SdfHexagon( in vec2 p, in float r )
{
    const vec3 k = vec3(-0.866025404, 0.5, 0.577350269);
    p = abs(p);
    p -= 2.0*min(dot(k.xy,p),0.0)*k.xy;
    p -= vec2(clamp(p.x, -k.z*r, k.z*r), r);
    
    return length(p)*sign(p.y);
}

mat2 Rotation2D(float radians)
{
    float sine = sin(radians);
    float cosine = cos(radians);

    return mat2(
        cosine, -sine,
        sine, cosine
        );
}

vec3 SmoothstepAntialiasing(vec3 destination_color, float sdf, vec3 sdf_color, vec3 edge_color, float edge_thickness, float edge_shadow_thickness)
{
    vec3 destination_color_new = mix(edge_color, destination_color, smoothstep(0.0, edge_thickness, sdf));  // edge
    destination_color_new = mix(sdf_color, destination_color_new, smoothstep(-edge_shadow_thickness, 0.0, sdf));    // shadow casted by edge

    return destination_color_new;
}

float SoftMax(float a, float b, float k) {
  return log(exp(k * a) + exp(k * b)) / k;
}

float SoftMin(float a, float b, float k) {
  return -SoftMax(-a, -b, k);
}

float SdfIntersection(float sdf1, float sdf2, bool soft, float hardness)
{
    if (soft)
    {
        return SoftMax(sdf1, sdf2, hardness);
    }
    return max(sdf1, sdf2);
}

float SdfSubtraction(float sdf1, float sdf2, bool soft, float hardness)
{
    if (soft)
    {
        return SoftMax(-sdf1, sdf2, hardness);
    }
    return max(-sdf1, sdf2);
}

float SdfUnion(float sdf1, float sdf2, bool soft, float hardness)
{
    if (soft)
    {
        return SoftMin(sdf1, sdf2, hardness);
    }
    return min(sdf1, sdf2);
}

float softMinValue(float sdf1, float sdf2, float k)
{
    //return remap(sdf1 - sdf2, -1.0 / k, 1.0 / k, 0.0, 1.0);
    return exp(-sdf2 * k) / (exp(-sdf1 * k) + exp(-sdf2 * k));
}

void main()
{
    vec2 centered_coordinates = v_UV - 0.5;
    vec2 pixel_coordinates = centered_coordinates * u_WindowResolution;

    // Background:

    vec3 color = BackgroundColor();

    // Grid:

    color = DrawGrid(color, vec3(0.5), 10.0, 1.0);
    color = DrawGrid(color, vec3(0.0), 100.0, 3.0);

    // SDFs:

    //float signed_distance_line = SdfLineSegment(pixel_coordinates, vec2(-800.0, 200.0), vec2(0.0, -400.0));
    ////color = mix(red, color, step(3.0, signed_distance_line));
    //color = SmoothstepAntialiasing(color, signed_distance_line, red, red, 4.0, 6.0);

    float signed_distance_circle = SdfCircle(pixel_coordinates, vec2(0.0, 0.0), 100.0);

    vec2 screen_space_position = vec2(0.0, 0.0 + 400.0 * sin(u_TimePassed));
    vec2 sdf_space_position = vec2(0.0);
    sdf_space_position = pixel_coordinates - screen_space_position; // first do translation, then do rotation
    sdf_space_position = Rotation2D(u_TimePassed) * sdf_space_position;
    float signed_distance_box = SdfBox(sdf_space_position, vec2(153.0, 153.0));
    
    float sdf_merged = SdfUnion(signed_distance_circle, signed_distance_box, true, 0.03);
    
    //vec3 sdf_color = mix(red, blue, smoothstep(0.0, 1.0, softMinValue(signed_distance_circle, signed_distance_box, 0.01)));

    float signed_distance_circle_A = SdfCircle(pixel_coordinates, vec2(0.0, 0.0), 200.0);
    float signed_distance_circle_B = SdfCircle(pixel_coordinates, vec2(u_MovableCircleX, 0.0), 200.0);
    float sdf_merged_C = SdfUnion(signed_distance_circle_A, signed_distance_circle_B, u_Soft, 0.03);

    if ((u_SignedDistance - 0.5 < sdf_merged_C) && (sdf_merged_C < u_SignedDistance + 0.5))
    {

        if (u_SignedDistance > 0.0)
        {
            color = vec3(0.0);
        }
        else
        {
            color = vec3(1.0);
        }
    }
    else
    {
        vec3 sdf_color = red;
        if (u_Soft)
        {
            sdf_color = mix(red, blue, smoothstep(0.0, 1.0, softMinValue(signed_distance_circle_A, signed_distance_circle_B, 0.01)));
        }
        color = SmoothstepAntialiasing(color, sdf_merged_C, sdf_color, sdf_color * 0.214, 2.0, 6.0);
    }

    // ---

    final_color = vec4(color, 1.0);
}