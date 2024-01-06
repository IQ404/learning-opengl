#shader vertex
#version 460 core

layout (location = 0) in vec4 position;

void main()
{
   gl_Position = position;
}

#shader fragment
#version 460 core

uniform vec4 u_color;	// the u prefix is a naming convention for uniform variables
layout (location = 0) out vec4 color;

void main()
{
   color = u_color;
}