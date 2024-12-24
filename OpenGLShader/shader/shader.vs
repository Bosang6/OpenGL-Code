#version 330 core

uniform float movimento;
uniform float Sin;
uniform float Cos;

layout (location = 0) in vec3 aPos;

out vec4 vertexColor;


void main(){
    gl_Position = vec4(aPos.x * Cos - aPos.y * Sin, aPos.y * Cos + aPos.x * Sin, aPos.z, 1.0);
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}