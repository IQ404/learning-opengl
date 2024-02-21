#shader vertex
#version 460 core

layout (location = 0) in vec4 position;	// right now this position is the position in local (object) space in right-handed coordinates.

layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec2 v_TexCoord;	// v for varying, as OpenGL is interpolating this after vertex shader and before fragment shader.
out vec3 v_Normal;
out vec3 v_WorldPosition;

void main()
{
    gl_Position = u_MVP * position;	// Note that position is right-handed, gl_Position is left-handed
    v_TexCoord = texCoord;
    v_Normal = (u_Model * vec4(normal, 0.0)).xyz;
    v_WorldPosition = (u_Model * position).xyz;
}

#shader fragment
#version 460 core

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_WorldPosition;

layout (location = 0) out vec4 final_color;

uniform vec4 u_CameraWorldPosition;
uniform float u_Shininess;
uniform bool u_Blinn;

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

float Phong(vec3 light_dir, vec3 normal, vec3 view_direction)
{    
    vec3 reflection_direction = normalize(reflect(-light_dir, normal));   // pointing from the surface
    return pow(max(0.0, dot(view_direction, reflection_direction)), u_Shininess);
}

float BlinnPhong(vec3 light_dir, vec3 normal, vec3 view_direction)
{
    vec3 half_vector = normalize(light_dir + view_direction);
    float N_dot_H = max(0.0, dot(normal, half_vector));
    return pow(N_dot_H, u_Shininess);
}

void main()
{
    vec3 normal = normalize(v_Normal);  // this needs to be normalized because it has been interpolated.

    vec3 material_albedo = vec3(0.214);

    vec3 ambient_light = vec3(0.5);

    vec3 light_dir = normalize(vec3(1.0));     // pointing from the surface
    vec3 direct_light = vec3(1.0, 1.0, 0.9);
    vec3 lambertian_light = max(0.0, dot(normal, light_dir)) * direct_light;

    vec3 view_direction = normalize(u_CameraWorldPosition.xyz - v_WorldPosition);   // pointing from the surface
    float specular_factor = 0.0;
    if (u_Blinn)
    {
        specular_factor = BlinnPhong(light_dir, normal, view_direction);
    }
    else
    {
        specular_factor = Phong(light_dir, normal, view_direction);
    }
    // This Phong/Blinn-Phong shading is hacky because right now we aren't accounting for the effect of the color of direct_light and of material_albedo on specular_light
    vec3 specular_light = vec3(specular_factor);
    vec3 lighting = ambient_light + lambertian_light;
    vec3 color = lighting * material_albedo + specular_light;

    color = linearTosRGB(color);

	final_color = vec4(color, 1.0);
}