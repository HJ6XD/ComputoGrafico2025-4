#version 460 core

in vec4 vertexColor;
out vec4 outColor;
uniform float time;

void main(){
	vec4 newColor = vertexColor;
	newColor.x = 1;
	newColor.y = 1;
	newColor.z = 0;
	/*newColor.x = sin(time);
	newColor.y = sin(time);
	newColor.z = sin(time);*/
	outColor = newColor;
}