#shader vertex
#version 460 core

layout (location = 0) in vec3 position;	// right now this position is the position in local (object) space in right-handed coordinates.

layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform float u_TimePassed;

out vec2 v_TexCoord;	// v for varying, as OpenGL is interpolating this after vertex shader and before fragment shader.
out vec3 v_Normal;
out vec3 v_WorldPosition;
out vec3 v_Color;

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
    vec3 local_space_position = position;
    vec3 local_space_normal = normal;

    float ripple = sin(15.0 * local_space_position.y + 15.0 * u_TimePassed);
    float amplitude_max = 0.1;
    float amplitude = remap(ripple, -1.0, 1.0, 0.0, amplitude_max);
    local_space_position += amplitude * local_space_normal;
    

    gl_Position = u_MVP * vec4(local_space_position, 1.0);	// Note that position is right-handed, gl_Position is left-handed
    v_TexCoord = texCoord;
    v_Normal = (u_Model * vec4(local_space_normal, 0.0)).xyz;
    v_WorldPosition = (u_Model * vec4(local_space_position, 1.0)).xyz;

    vec3 peak_color = vec3(0.0, 0.0, 0.5);
    vec3 trough_color = vec3(0.1, 0.5, 0.8);
    v_Color = mix(peak_color, trough_color, smoothstep(0.0, amplitude_max, amplitude));
    //v_Color = mix(peak_color, trough_color, remap(amplitude, 0.0, amplitude_max, 0.0, 1.0));

}

#shader fragment
#version 460 core

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_WorldPosition;
in vec3 v_Color;

layout (location = 0) out vec4 final_color;

uniform samplerCube u_Cubemap;
uniform vec4 u_CameraWorldPosition;
uniform float u_Shininess;
uniform bool u_FresnelEffect;
uniform bool u_UseScreenSpaceDerivativeNormal;

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
    
    if (u_UseScreenSpaceDerivativeNormal)
    {
        normal = normalize(cross(dFdx(v_WorldPosition), dFdy(v_WorldPosition)));
    }

    vec3 material_albedo = v_Color;

    vec3 ambient_light = vec3(0.25);

    vec3 light_dir = normalize(vec3(1.0));     // pointing from the surface
    vec3 direct_light = vec3(1.0, 1.0, 0.9);
    vec3 lambertian_light = max(0.0, dot(normal, light_dir)) * direct_light;

    vec3 view_direction = normalize(u_CameraWorldPosition.xyz - v_WorldPosition);   // pointing from the surface
    float specular_factor = BlinnPhong(light_dir, normal, view_direction);

    // This Phong shading is hacky because right now we aren't accounting for the effect of the color of direct_light
    vec3 specular_light = vec3(specular_factor);

    vec3 cubemap_sampling_dir = reflect(-view_direction, normal);
    vec3 cubemap_sample = texture(u_Cubemap, cubemap_sampling_dir).xyz; // TODO: do we need to do sRGB-to-linear transformation? 
    vec3 ibl = cubemap_sample * 0.5;
    specular_light += ibl;

    if (u_FresnelEffect)
    {
        float fresnel_depth = 2.0;  // how grazed the angle needs to be to have reflection
        float fresnel_factor = pow(1.0 - max(0.0, dot(view_direction, normal)), fresnel_depth);
        specular_light *= fresnel_factor;
    }
    
    vec3 diffuse_lighting = ambient_light + lambertian_light;
    
    // Here I define diffuse lights to be lights that goes into the object (and then get out), and specular lights to be lights that directly reflect away, without interacting with albedo
    vec3 color = diffuse_lighting * material_albedo + specular_light;

    color = linearTosRGB(color);

	final_color = vec4(color, 1.0);
}