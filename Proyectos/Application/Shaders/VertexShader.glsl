#version 460 core
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;

out vec4 vertexColor;
uniform float time;
uniform float horizontalSpeed;

void main(){
	vertexColor = vColor;
	vec4 newPosition = vPosition;
	newPosition.x = newPosition.x + horizontalSpeed;

	gl_Position = newPosition; // Equivale a hacer return gl_Position
}