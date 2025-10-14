#include "glad.h"
#include "GLFW/glfw3.h"
#include "Application.h"
#include <string>

void KeyCallBack(GLFWwindow* _window, int key, int scancode, int action, int code) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(_window, true);
    }
}

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

    glfwSetKeyCallback(window, KeyCallBack);

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