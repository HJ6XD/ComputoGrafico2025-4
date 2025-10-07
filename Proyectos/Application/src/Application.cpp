#include "Application.h"

void Application::SetUp()
{
	//Crear Vertex Array Object
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	ids["triangle"] = VAO;

	glBindVertexArray(VAO);

	//Crear Vertex Buffer Object (VBO) vertices
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Ojo, aun no ha reservado memoria
	//pasar areglo de vertices
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * geometry.size(),
		&geometry[0],
		GL_STATIC_DRAW); //mandamos la geometria al buffer

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void Application::Update()
{
	std::cout << "update" << std::endl;
}

void Application::Draw()
{
	std::cout << "draw" << std::endl;
}
