#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "Application.h"

Application app;

void MiCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    app.keyCallback(key, scancode, action, mods);
}

int main(void)
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    app.window = glfwCreateWindow(app.WINDOW_WIDTH, app.WINDOW_HEIGHT, "Conejo que Gira JAJAJAJ", nullptr, nullptr);
    if (!app.window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(app.window);
    glfwSetKeyCallback(app.window, MiCallback);

    app.setup();

    while (!glfwWindowShouldClose(app.window))
    {
        glfwPollEvents();
        app.update();
        app.draw();
    }

    glfwTerminate();
    return 0;
}
