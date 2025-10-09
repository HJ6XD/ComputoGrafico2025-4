#include "Application.h"

void Application::GeometrySetUp()
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

void Application::ProgramSetUp()
{
	std::string fragmentShader = leerArchivo("Shaders/FragmentShader.glsl");
	std::string vertexShader = leerArchivo("Shaders/VertexShader.glsl");
	ids["program"] = shaderfuncs.InitializeProgram(vertexShader, fragmentShader);
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
	//std::cout << "update" << std::endl;
}

void Application::Draw()
{
	//Seleccionar programa (shaders)
	glUseProgram(ids["program"]);

	//Seleccionar la geometria 
	glBindVertexArray(ids["triangle"]);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}
