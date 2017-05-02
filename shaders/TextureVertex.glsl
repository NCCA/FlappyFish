#version 330 core

uniform mat4 MVP;

layout (location=0)in vec3 inVert;
layout (location=1)in vec2 inUV;

out vec2 vertUV;

void main()
{
    gl_Position = MVP*vec4(inVert, 1.0);
    vertUV=inUV.st;
}








