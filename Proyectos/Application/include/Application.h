#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <glad.h>
class Application {
private:
	std::vector<float> geometry{
		//X		Y	  Z     W
		 -1.0f, 1.0f, 0.0f, 1.0f, //VERTICE 1
		-1.0f, -1.0f, 0.0f, 1.0f, //VERTICE 2
		 1.0f, -1.0f, 0.0f, 1.0f  //VERTICE 3
	};
	std::map<std::string, GLuint> ids;
public: 
	void SetUp();
	void Update();
	void Draw();

};