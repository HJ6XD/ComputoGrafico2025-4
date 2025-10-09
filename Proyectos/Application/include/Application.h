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
	std::vector<float> geometry{
		//X		Y	  Z     W
		 -1.0f, 1.0f, 0.0f, 1.0f, //VERTICE 1
		-1.0f, -1.0f, 0.0f, 1.0f, //VERTICE 2
		 1.0f, -1.0f, 0.0f, 1.0f  //VERTICE 3
	};
	std::map<std::string, GLuint> ids;

	void GeometrySetUp();
	void ProgramSetUp();
public: 
	std::string leerArchivo(const std::string& nombreArchivo);
	void SetUp();
	void Update();
	void Draw();

};