#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        printf("GLFW initialization failed!");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Core profile -> no backwards compatibility because we don't want to allow
    // deprecated features (throw error if anything use is deprecated)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", nullptr, nullptr);
    if (!mainWindow) {
        printf("GLFW window creation failed!");
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information (area in middle of window that will hold all
    // the OpenGL data as it's being passed to the window). Will be the size of
    // the OpenGL viewport (the part that's getting drawn to)
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    // Let GLEW know that this window is the one that the OpenGL context should
    // be tied to. Can change between windows with this
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        printf("GLEW initialization failed!");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Setup Viewport size
    // Top left corner
    glViewport(0, 0, bufferWidth, bufferHeight);
    
    // Loop until window closed
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get and handle user input events
        glfwPollEvents();

        // Clear window
        // RGB values are between 0 and 1 not 0 and 255
        // Alpha value is how transparent it is
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        // Each pixel has color but more than that for graphical applications.
        // Has depth information in addtion for instance.
        // Here we clear just the color information
        glClear(GL_COLOR_BUFFER_BIT);

        // There are two scenes going on at once. The one that the user sees
        // and one that you are drawing to (can't be seen).
        // Swaps buffers so what you drew becomes what the user sees
        glfwSwapBuffers(mainWindow);
    }
    return 0;
}