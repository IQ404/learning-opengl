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

in vec2 v_UV;
layout (location = 0) out vec4 final_color;

void main()
{
    vec3 color = vec3(0.0);

    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 blue = vec3(0.0, 0.0, 1.0);
    vec3 white = vec3(1.0, 1.0, 1.0);
    vec3 black = vec3(0.0, 0.0, 0.0);

    //color = vec3(v_UV.x);
    //color = vec3(smoothstep(0.0, 1.0, v_UV.x));
    //color = vec3(step(0.75, v_UV.x));
    //color = mix(red, blue, v_UV.x);
    //color = mix(red, blue, smoothstep(0.0, 1.0, v_UV.x));

    //float line = step(0.05, abs(v_UV.y - 0.5)); // hard horizontal line
    //float line = smoothstep(0.0, 0.05, abs(v_UV.y - 0.5)); // soft horizontal line

    float upper_axis = smoothstep(0.0, 0.005, abs(v_UV.y - 0.666));  // return 0 if the pixel is on the line, 1 if >=0.005 away, in between can represent the thickness of the graph
    float lower_axis = smoothstep(0.0, 0.005, abs(v_UV.y - 0.333));

    float mix_function_value = v_UV.x;
    float smoothstep_function_value = smoothstep(0.0, 1.0, v_UV.x);
    float step_function_value = step(0.5, v_UV.x);

    float mix_function = smoothstep(0.0, 0.005, abs(v_UV.y - mix(0.0, 0.333, mix_function_value)));
    float smoothstep_function = smoothstep(0.0, 0.005, abs(v_UV.y - mix(0.333, 0.666, smoothstep_function_value)));
    float step_function = smoothstep(0.0, 0.005, abs(v_UV.y - mix(0.666, 1.0, step_function_value)));

    if (v_UV.y > 0.666)
    {
        color = mix(red, blue, step_function_value);
    }
    else if (v_UV.y > 0.333)
    {
        color = mix(red, blue, smoothstep_function_value);
    }
    else
    {
        color = mix(red, blue, mix_function_value);
    }

    color = mix(black, color, upper_axis);
    color = mix(black, color, lower_axis);

    color = mix(white, color, mix_function);
    color = mix(white, color, smoothstep_function);
    color = mix(white, color, step_function);

    final_color = vec4(color, 1.0);
}