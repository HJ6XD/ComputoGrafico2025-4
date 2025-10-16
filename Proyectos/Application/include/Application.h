#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <glad.h>
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include <fstream>
#include <string>
#include "ShaderFuncs.h"
class Application {
private:
	ShaderFuncs shaderfuncs;

	std::map<std::string, GLuint> ids;

	void GeometrySetUp();
	void CubeSetUp();
	void ProgramSetUp1();
	void ProgramSetUp2();
	std::string leerArchivo(const std::string& nombreArchivo);
	glm::mat4 camera;
	glm::mat4 proyection;
	glm::mat4 model;
	glm::vec3 eye{ 0.0f, 0.0f,-2.0f };
	glm::vec3 center{ 0.1f, 0.1f,0.5f };
public:

	float time{ 0.0f };
	float horizontalSpeed = 0;
	float xRot, yRot;
	double lastXMouse, lastYMouse;
	bool moveingRight = false, moveingLeft = false;
	void KeyCallBack(int key, int scancode, int action, int code);
	void CursorPosCallBack(GLFWwindow* window, double xpos, double ypos);
	GLFWwindow* window;
	void SetUp();
	void Update();
	void Draw();

};