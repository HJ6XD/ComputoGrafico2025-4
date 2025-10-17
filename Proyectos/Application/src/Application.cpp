#include "Application.h"

void Application::GeometrySetUp()
{
	std::vector<float> geometry{
		//X		Y	  Z     W
		 -1.0f, -1.0f, 0.0f, 1.0f,	//VERTICE 1
		 -1.0f,  1.0f, 0.0f, 1.0f,	//VERTICE 2
		  1.0f, -1.0f, 0.0f, 1.0f,	//VERTICE 3
		  1.0f,  1.0f, 0.0f, 1.0f,	//VERTICE 4

		  1.0f, 0.0f, 0.0f,	1.0f,	//Color V1
		  1.0f, 1.0f, 0.0f,	1.0f,	//Color V2
		  0.0f, 0.0f, 1.0f, 1.0f,	//Color V3
		  0.0f, 1.0f, 0.0f, 1.0f	//Color V3
	};

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
		sizeof(float) * geometry.size(),
		&geometry[0],
		GL_STATIC_DRAW); //mandamos la geometria al buffer

	//Vertices
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	//Colores
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const void*)(sizeof(float) * 16));
	glEnableVertexAttribArray(1);
}

void Application::CubeSetUp()
{
	std::vector<float> cube{
		//Vertices		
		//Cara Izquierda
		//Triangulo 1
		-1.0f,  1.0f, -1.0f, 1.0f, //Vertice 1
		-1.0f,  1.0f,  1.0f, 1.0f, //Vertice 2
		-1.0f, -1.0f, -1.0f, 1.0f, //Vertice 3
		
		//Triangulo 2
		-1.0f, -1.0f,  1.0f, 1.0f, //Vertice 1
		-1.0f,  1.0f,  1.0f, 1.0f, //Vertice 24
		-1.0f, -1.0f, -1.0f, 1.0f, //Vertice 37302.

		
		//Cara Derecha
		//Triangulo 1
		 1.0f,  1.0f, -1.0f, 1.0f, //Vertice 1
		 1.0f,  1.0f,  1.0f, 1.0f, //Vertice 2
		 1.0f, -1.0f, -1.0f, 1.0f, //Vertice 3
		
		//Triangulo 2
		 1.0f, -1.0f,  1.0f, 1.0f, //Vertice 1
		 1.0f,  1.0f,  1.0f, 1.0f, //Vertice 2
		 1.0f, -1.0f, -1.0f, 1.0f, //Vertice 3
		
		//Cara Abajo
		//Triangulo 1
		 1.0f, -1.0f, -1.0f, 1.0f, //Vertice 1
		 1.0f, -1.0f,  1.0f, 1.0f, //Vertice 2
		-1.0f, -1.0f, -1.0f, 1.0f, //Vertice 3
		
		//Triangulo 2
		-1.0f, -1.0f,  1.0f, 1.0f, //Vertice 1
		 1.0f, -1.0f,  1.0f, 1.0f, //Vertice 2
		-1.0f, -1.0f, -1.0f, 1.0f, //Vertice 3
		
		//Cara Arriba
		//Triangulo 1
		 1.0f,  1.0f, -1.0f, 1.0f, //Vertice 1
		 1.0f,  1.0f,  1.0f, 1.0f, //Vertice 2
		-1.0f,  1.0f, -1.0f, 1.0f, //Vertice 3
		
		//Triangulo 2
		-1.0f,  1.0f,  1.0f, 1.0f, //Vertice 1
		 1.0f,  1.0f,  1.0f, 1.0f, //Vertice 2
		-1.0f,  1.0f, -1.0f, 1.0f, //Vertice 3
		
		//Cara Atras
		//Triangulo 1
		-1.0f,  1.0f, -1.0f, 1.0f, //Vertice 1
		 1.0f,  1.0f, -1.0f, 1.0f, //Vertice 2
		-1.0f, -1.0f, -1.0f, 1.0f, //Vertice 3
		
		//Triangulo 2
		 1.0f, -1.0f, -1.0f, 1.0f, //Vertice 1
		 1.0f,  1.0f, -1.0f, 1.0f, //Vertice 2
		-1.0f, -1.0f, -1.0f, 1.0f, //Vertice 3
		
		//Cara Alfrente
		//Triangulo 1
		-1.0f,  1.0f,  1.0f, 1.0f, //Vertice 1
		 1.0f,  1.0f,  1.0f, 1.0f, //Vertice 2
		-1.0f, -1.0f,  1.0f, 1.0f, //Vertice 3
		
		//Triangulo 2
		 1.0f, -1.0f,  1.0f, 1.0f, //Vertice 1
		 1.0f,  1.0f,  1.0f, 1.0f, //Vertice 2
		-1.0f, -1.0f,  1.0f, 1.0f, //Vertice 3
		


		//Colores
		 //Cara Izquierda
		//Color Triangulo 1
		1.0f, 0.0f,  0.0f, 1.0f, //Color Vertice 1
		1.0f, 0.0f,  0.0f, 1.0f, //Color Vertice 2
		1.0f, 0.0f,  0.0f, 1.0f, //Color Vertice 3

		//Color Triangulo 2
		1.0f, 0.0f,  0.0f, 1.0f, //Color Vertice 1
		1.0f, 0.0f,  0.0f, 1.0f, //Color Vertice 2
		1.0f, 0.0f,  0.0f, 1.0f, //Color Vertice 3

		//Cara Derecha
		//Color Triangulo 1
		0.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 1
		0.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 2
		0.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 3

		//Color Triangulo 2
		0.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 1
		0.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 2
		0.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 3

		//Cara Abajo
		//Color Triangulo 1
		0.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 1
		0.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 2
		0.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 3

		//Color Triangulo 2
		0.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 1
		0.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 2
		0.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 3

		//Cara Arriba
		//Color Triangulo 1
		1.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 1
		1.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 2
		1.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 3

		//Color Triangulo 2
		1.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 1
		1.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 2
		1.0f, 1.0f,  0.0f, 1.0f, //Color Vertice 3

		//Cara Atras
		//Color Triangulo 1
		1.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 1
		1.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 2
		1.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 3

		//Color Triangulo 2
		1.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 1
		1.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 2
		1.0f, 0.0f,  1.0f, 1.0f, //Color Vertice 3

		//Cara Alfrente
		//Color Triangulo 1
		0.0f, 1.0f,  1.0f, 1.0f, //Color Vertice 1
		0.0f, 1.0f,  1.0f, 1.0f, //Color Vertice 2
		0.0f, 1.0f,  1.0f, 1.0f, //Color Vertice 3

		//Color Triangulo 2
		0.0f, 1.0f,  1.0f, 1.0f, //Color Vertice 1
		0.0f, 1.0f,  1.0f, 1.0f, //Color Vertice 2
		0.0f, 1.0f,  1.0f, 1.0f, //Color Vertice 3
	};

	//Crear Vertex Array Object
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	ids["cubo"] = VAO;

	glBindVertexArray(VAO);

	//Crear Vertex Buffer Object (VBO) vertices
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Ojo, aun no ha reservado memoria
	//pasar areglo de vertices
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(float) * cube.size(),
		&cube[0],
		GL_STATIC_DRAW); //mandamos la geometria al buffer

	//Vertices
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//Colores
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const void*)(sizeof(float) * 144));
	glEnableVertexAttribArray(1);
}

void Application::ProgramSetUp1()
{
	std::string fragmentShader = leerArchivo("Shaders/FragmentShader.glsl");
	std::string vertexShader = leerArchivo("Shaders/VertexShader.glsl");
	ids["program1"] = shaderfuncs.InitializeProgram(vertexShader, fragmentShader);
	ids["time"] = glGetUniformLocation(ids["program"], "time");
	ids["horizontalSpeed"] = glGetUniformLocation(ids["program"], "horizontalSpeed");
}
void Application::ProgramSetUp2()
{
	std::string fragmentCamera = leerArchivo("Shaders/FragmentCamera.glsl");
	std::string vertexCamera = leerArchivo("Shaders/VertexCamera.glsl");
	ids["program2"] = shaderfuncs.InitializeProgram(vertexCamera, fragmentCamera);
	ids["time2"] = glGetUniformLocation(ids["program2"], "time");
	ids["camera"] = glGetUniformLocation(ids["program2"], "camera");
	ids["proyection"] = glGetUniformLocation(ids["program2"], "proyection");
	ids["model"] = glGetUniformLocation(ids["program2"], "model");
}

std::string Application::leerArchivo(const std::string& ruta) {
	std::ifstream archivo(ruta); // abre el archivo en modo texto
	if (!archivo) {
		std::cerr << "No se pudo abrir el archivo: " << ruta << std::endl;
		return "";
	}

	std::string contenido;
	std::string linea;

	while (std::getline(archivo, linea)) {
		contenido += linea + '\n'; // conserva los saltos de línea
	}

	return contenido;
}


void Application::SetUp()
{
	lastXMouse = 0; lastYMouse = 0;

	//ProgramSetUp1();
	ProgramSetUp2();
	CubeSetUp();

	glEnable(GL_DEPTH_TEST);

	proyection = glm::perspective(45.0f, 1024.0f/ 768.0f, 0.1f, 100.0f);
}

void Application::Update()
{
	


	if (moveingLeft) xRot -= 0.02;
	if (moveingRight) xRot += 0.02;
	time += 1;
	eye = glm::vec3(2.0f , 2.0f , 2.0f);
	center = glm::vec3(0.0f, 0.0f, 0.0f);
	camera = glm::lookAt(eye, center, glm::vec3(0.0f, 0.0f, 1.0f));
	
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f,1.0f,1.0f));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(yRot), glm::vec3(0.0f,1.0f,0.0f) );
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(xRot), glm::vec3(0.0f,0.0f,1.0f) );
	model = rotateZ* rotateY * translate * scale;
}
void Application::KeyCallBack(int key, int scancode, int action, int code) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	//Teclas para mover
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		moveingRight = true;
	}
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		moveingRight = false;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		moveingLeft = true;
	}
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		moveingLeft = false;
	}
}
void Application::CursorPosCallBack(GLFWwindow* window, double xpos, double ypos)
{
	double deltax = lastXMouse - xpos;
	double deltay = lastYMouse - ypos;
	lastXMouse = xpos; lastYMouse = ypos;
	xRot += deltax * sensitivity;
	yRot += deltay * sensitivity;
}
void Application::Draw()
{
	//Seleccionar programa (shaders)
	glUseProgram(ids["program2"]);

	//Pasar el resto de los parámetros para el programa
	glUniform1f(ids["time2"], time);
	glUniformMatrix4fv(ids["model"],1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(ids["camera"],1, GL_FALSE, &camera[0][0]);
	glUniformMatrix4fv(ids["proyection"],1, GL_FALSE, &proyection[0][0]);

	//Seleccionar la geometria 
	glBindVertexArray(ids["cubo"]);

	glDrawArrays(GL_TRIANGLES, 0, 36);
}
