#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "ApplicationDX12.h"

ApplicationDX12 app;

void MiCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    app.keyCallback(key, scancode, action, mods);
}

int main(void)
{

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    app.window = glfwCreateWindow(app.Width, app.Height, "Hello D3D11", nullptr, nullptr);
    if (!app.window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(app.window);


    app.setup();

    //seccion de asignacion de callbacks
    glfwSetKeyCallback(app.window, MiCallback);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(app.window))
    {
        /* Render here */

        /* Poll for and process events */
        glfwPollEvents();
        app.update();
        //app.clearColorBuffer( 1.0, 1.0, 0.0, 1.0 );
        app.draw();
        /* Swap front and back buffers */
        //glfwSwapBuffers(app.window);
    }

    glfwTerminate();
    return 0;
}

