#include "glad.h"
#include "GLFW/glfw3.h"
#include "Application.h"
#include <string>

Application app;

void myCallBack(GLFWwindow* _window, int key, int scancode, int action, int code) {
    app.KeyCallBack(key, scancode, action, code);
}
int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    
    app.window = glfwCreateWindow(1024, 768, "Hello Application", NULL, NULL);
    glfwMakeContextCurrent(app.window);
    /* Make the window's context current */

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    app.SetUp();

    glfwSetKeyCallback(app.window, myCallBack);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(app.window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        app.Update();

        app.Draw();
        glfwSwapBuffers(app.window);
    }

    glfwTerminate();
    return 0;
}