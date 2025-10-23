#version 460 core

in vec4 vertexColor;
out vec4 outColor;
uniform float time;

void main(){
	vec4 newColor = vertexColor;
	newColor.x = 0;
	newColor.y = 0;
	newColor.z = 0;
	newColor.x = sin(time * 2.0) * 0.5 + 0.5;
	newColor.y = sin(time * 2.0 + 2.0) * 0.5 + 0.5;
	newColor.z = sin(time * 2.0 + 4.0) * 0.5 + 0.5;
	outColor = newColor;
	//outColor = vertexColor;
}