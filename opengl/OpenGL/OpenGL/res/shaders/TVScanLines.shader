#shader vertex
#version 460 core

layout (location = 0) in vec4 position;	// right now this position is the position in local (object) space in right-handed coordinates.
/*
Note that currently location 0 actually feeds in 2 floats.
It is predefined that OpenGL will set the 1st and 2nd components of the vec4 to these two floats respectively,
and set the 3rd and 4th components of the vec4 to 0.0f (This is based on the assumption that if Z is not provided, the vertex is meant to be in a plane) and
1.0f (implies a homogeneous position in space as opposed to a homogeneous direction vector, which would have set to 0.0f) respectively.
*/

layout (location = 1) in vec2 texCoord;

uniform mat4 u_MVP;

out vec2 v_TexCoord;	// v for varying, as OpenGL is interpolating this after vertex shader and before fragment shader.

void main()
{
    gl_Position = u_MVP * position;	// Note that position is right-handed, gl_Position is left-handed
    v_TexCoord = texCoord;
}

#shader fragment
#version 460 core

uniform float u_TimePassed;	// the u prefix is a naming convention for uniform variables
uniform sampler2D u_Texture;

in vec2 v_TexCoord;

layout (location = 0) out vec4 final_color;

float inverseLerp(float value, float minValue, float maxValue)
{
    return (value - minValue) / (maxValue - minValue);
}

float remap(float value, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(value, inMin, inMax);
    return mix(outMin, outMax, t);
}

void main()
{
    // Interpretation: remap(sin(y-axis value * frequency + time_passed * speed), -1.0, 1.0, darkness, 1.0);

    // Downwards Scan Lines:
    float t1 = remap(sin(v_TexCoord.y * 400.0 + u_TimePassed * 10.0), -1.0, 1.0, 0.9, 1.0);
    
    // Upwards Scan Lines:
    float t2 = remap(sin(v_TexCoord.y * 50.0 - u_TimePassed * 2.0), -1.0, 1.0, 0.9, 1.0);

    vec3 color = texture(u_Texture, v_TexCoord).rgb * t1 * t2;

    final_color = vec4(color, 1.0);
}