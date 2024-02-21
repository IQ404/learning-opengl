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

//uniform samplerCube u_Cubemap;
uniform vec4 u_CameraWorldPosition;
uniform float u_Shininess;
//uniform bool u_FresnelEffect;

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

float BlinnPhong(vec3 light_dir, vec3 normal, vec3 view_direction)
{
    vec3 half_vector = normalize(light_dir + view_direction);
    float N_dot_H = max(0.0, dot(normal, half_vector));
    return pow(N_dot_H, u_Shininess);
}

void main()
{
    vec3 normal = normalize(v_Normal);  // this needs to be normalized because it has been interpolated.

    vec3 material_albedo = vec3(0.214, 0.214, 0.214);

    vec3 skyLight = vec3(0.0, 0.3, 0.6);
    vec3 groundLight = vec3(0.6, 0.3, 0.1);

    //vec3 ambient_light = vec3(0.25);

    vec3 hemisphere_light = mix(groundLight, skyLight, remap(normal.y, -1.0, 1.0, 0.0, 1.0));

    vec3 light_dir = normalize(vec3(1.0));     // pointing from the surface
    vec3 direct_light = vec3(1.0, 1.0, 0.9);
    float lambertian_factor = max(0.0, dot(normal, light_dir));

    //float cel_shadow_factor = lambertian_factor * smoothstep(0.5, 0.505, lambertian_factor);
    
    //float cel_shadow_factor = mix(0.5, 1.0, step(0.65, lambertian_factor)) * step(0.5, lambertian_factor);
    float primary_shadow = smoothstep(0.5, 0.505, lambertian_factor);
    float secondary_shadow = smoothstep(0.65, 0.655, lambertian_factor) * 0.5 + 0.5;
    float cel_shadow_factor = primary_shadow * secondary_shadow;

    vec3 view_direction = normalize(u_CameraWorldPosition.xyz - v_WorldPosition);   // pointing from the surface

    float fresnel_depth = 2.0;  // how grazed the angle needs to be to have reflection
    float fresnel_factor = pow(1.0 - max(0.0, dot(view_direction, normal)), fresnel_depth);
    float cel_fresnel_depth = 0.7;
    float cel_fresnel_factor = fresnel_factor * step(cel_fresnel_depth, fresnel_factor);

    vec3 cel_diffuse_lighting = direct_light * cel_shadow_factor * 0.8 + hemisphere_light * 0.2 + hemisphere_light * cel_fresnel_factor;

    vec3 reflection_direction = normalize(reflect(-light_dir, normal));   // pointing from the surface
    float specular_factor = pow(max(0.0, dot(view_direction, reflection_direction)), u_Shininess);
    //specular_factor = BlinnPhong(light_dir, normal, view_direction);
    // This Phong shading is hacky because right now we aren't accounting for the color of direct_light
    vec3 specular_light = vec3(specular_factor);
    vec3 cel_specular_light = smoothstep(0.5, 0.51, specular_light);
    
    // Here I define diffuse lights to be lights that goes into the object (and then get out), and specular lights to be lights that directly reflect away, without interacting with albedo
    vec3 color = cel_diffuse_lighting * material_albedo + cel_specular_light;

    color = linearTosRGB(color);

	final_color = vec4(color, 1.0);
}