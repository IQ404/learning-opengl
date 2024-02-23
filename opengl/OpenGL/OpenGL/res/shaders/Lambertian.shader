#shader vertex
#version 460 core

layout (location = 0) in vec4 position;	// right now this position is the position in local (object) space in right-handed coordinates.

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

uniform bool u_MixWithHemisphereLighting;
uniform bool u_LinearTosRGBCorrection;

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

vec3 linearTosRGB(vec3 color)
{
    vec3 lt = vec3(lessThanEqual(color.rgb, vec3(0.0031308)));
    
    vec3 v1 = color * 12.92;
    vec3 v2 = pow(color.xyz, vec3(0.41666)) * 1.055 - vec3(0.055);

    return mix(v2, v1, lt);
}

vec3 gammaCorrection_sRGB(vec3 color)
{
    return pow(color, vec3(1.0 / 2.2));
}

void main()
{
    vec3 normal = normalize(v_Normal);  // this needs to be normalized because it has been interpolated.

    vec3 material_albedo = vec3(0.214);

    vec3 sky_light = vec3(0.0, 0.3, 0.6);
    vec3 ground_light = vec3(0.6, 0.3, 0.1);

    float mixing_factor = 0.0;

    if (u_MixWithHemisphereLighting)
    {
        mixing_factor = 0.5;
    }
    
    float effective_normal_normalized = remap(normal.y, -1.0, 1.0, 0.0, 1.0);
    vec3 hemisphere_lighting = mix(ground_light, sky_light, effective_normal_normalized);

    vec3 light_dir = normalize(vec3(1.0));     // pointing from the surface
    vec3 direct_light = vec3(1.0, 1.0, 0.9);
    vec3 lambertian_lighting = max(0.0, dot(normal, light_dir)) * direct_light;

    vec3 lighting = hemisphere_lighting * mixing_factor + lambertian_lighting * (1.0 - mixing_factor);

    vec3 color = lighting * material_albedo;

    if (u_LinearTosRGBCorrection)
    {
        color = linearTosRGB(color);
    }

	final_color = vec4(color, 1.0);
}