#version 430 core
layout (depth_greater) out float gl_FragDepth;
out vec4 color;

uniform vec4 renderingColor;
uniform float test;

void main()
{
    //color = vec4(0,0.9,0.8,0);
    color=renderingColor;
    //gl_FragDepth = gl_FragCoord.z + 0.1f;
}
