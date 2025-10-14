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
		  1.0f, 0.0f, 1.0f,	1.0f,	//Color V2
		  0.0f, 0.0f, 1.0f, 1.0f,	//Color V3
		  0.0f, 0.0f, 1.0f, 1.0f	//Color V3
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

void Application::ProgramSetUp()
{
	std::string fragmentShader = leerArchivo("Shaders/FragmentShader.glsl");
	std::string vertexShader = leerArchivo("Shaders/VertexShader.glsl");
	ids["program"] = shaderfuncs.InitializeProgram(vertexShader, fragmentShader);
	ids["time"] = glGetUniformLocation(ids["programa"], "time");
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
	ProgramSetUp();
	GeometrySetUp();
}

void Application::Update()
{
	time += 0.01f;
	
}

void Application::Draw()
{
	//Seleccionar programa (shaders)
	glUseProgram(ids["program"]);

	//Pasar el resto de los parámetros para el programa
	glUniform1f(ids["time"], time);

	//Seleccionar la geometria 
	glBindVertexArray(ids["triangle"]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
