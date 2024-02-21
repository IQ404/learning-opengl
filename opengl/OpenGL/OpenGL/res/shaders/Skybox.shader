#shader vertex
#version 460 core

layout (location = 0) in vec3 local_cubemap_vertex_position;	// right now this position is the position in local (object) space in right-handed coordinates.

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_RotationOnlyViewMatrix;

out vec3 v_DirectionalTexCoord;	// v for varying, as OpenGL is interpolating this after vertex shader and before fragment shader.

void main()
{
    gl_Position = u_ProjectionMatrix * u_RotationOnlyViewMatrix * vec4(local_cubemap_vertex_position, 1.0);
    v_DirectionalTexCoord = local_cubemap_vertex_position;
}

#shader fragment
#version 460 core

in vec3 v_DirectionalTexCoord;

uniform samplerCube u_Cubemap;

layout (location = 0) out vec4 color;

void main()
{
    color = texture(u_Cubemap, v_DirectionalTexCoord);
}