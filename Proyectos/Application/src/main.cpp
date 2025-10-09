#include "glad.h"
#include "GLFW/glfw3.h"
#include "Application.h"
#include <string>
int main(void)
{
    GLFWwindow* window;
    Application app;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1024, 768, "Hello Application", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    app.SetUp();
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        app.Update();

        app.Draw();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}