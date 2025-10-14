#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <glad.h>
#include <fstream>
#include <string>
#include "ShaderFuncs.h"
class Application {
private:
	ShaderFuncs shaderfuncs;
	
	std::map<std::string, GLuint> ids;

	void GeometrySetUp();
	void ProgramSetUp();
	float time{ 0.0f };
	std::string leerArchivo(const std::string& nombreArchivo);
	;
public: 
	void SetUp();
	void Update();
	void Draw();

};