#pragma once
#include "glad.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>

class ShaderFuncs {
public:
	GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile);
	GLuint CreateProgram(const std::vector<GLuint>& shaderList);
	GLuint InitializeProgram(std::string strVertexShader, std::string strFragmentShader);
};