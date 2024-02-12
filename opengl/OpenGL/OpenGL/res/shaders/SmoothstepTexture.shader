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

uniform bool u_SmoothstepOn;	// the u prefix is a naming convention for uniform variables
uniform sampler2D u_Texture;

in vec2 v_TexCoord;

layout (location = 0) out vec4 color;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord);
	if (u_SmoothstepOn)
	{
		color = smoothstep(vec4(0.0), vec4(1.0), texColor);
	}
	else
	{
		color = texColor;
	}
}