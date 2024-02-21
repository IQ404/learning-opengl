#shader vertex
#version 460 core

layout (location = 0) in vec4 position;	// right now this position is the position in local (object) space in right-handed coordinates.
/*
Note that currently location 0 actually feeds in 3 floats.
It is predefined that OpenGL will set the 1st, 2nd and 3rd components of the vec4 to these three floats respectively,
and set the 4th components of the vec4 to 1.0f (implies a homogeneous position in space as opposed to a homogeneous direction vector,
 which would have set to 0.0f) respectively.
*/

layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec2 v_TexCoord;	// v for varying, as OpenGL is interpolating this after vertex shader and before fragment shader.
out vec3 v_Normal;

void main()
{
    gl_Position = u_MVP * position;	// Note that position is right-handed, gl_Position is left-handed
    v_TexCoord = texCoord;
    v_Normal = (u_Model * vec4(normal, 0.0)).xyz;
}

#shader fragment
#version 460 core

uniform bool u_HemisphereLighting_on;

in vec2 v_TexCoord;
in vec3 v_Normal;

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
    vec3 normal = normalize(v_Normal);  // this needs to be normalized because it has been interpolated.

    vec3 material_albedo = vec3(0.5);

    vec3 ambient_light = vec3(0.5);

    vec3 lighting = vec3(0.0);

    vec3 sky_light = vec3(0.0, 0.3, 0.6);
    vec3 ground_light = vec3(0.6, 0.3, 0.1);

    if (u_HemisphereLighting_on)
    {
        float effective_normal_normalized = remap(normal.y, -1.0, 1.0, 0.0, 1.0);
        lighting = mix(ground_light, sky_light, effective_normal_normalized);
    }
    else
    {
        lighting = ambient_light;
    }

    vec3 color = lighting * material_albedo;

	final_color = vec4(color, 1.0);
    
    //final_color = vec4(normal, 1.0);
}