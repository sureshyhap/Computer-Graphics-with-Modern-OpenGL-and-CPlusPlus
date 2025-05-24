#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <string>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

// IDs. Since we will only be drawing one triangle for now, make these global
GLuint VAO, VBO, shader;

// Normally we will create the shaders in an external file (which we will later).
// But for now, we put them here as strings and read them as strings

// Vertex Shader
// location = 0 defines the search index for the attribute pointer so that it can
// find this. You can also make it search for the name of an attribute ('pos' here).
// gl_Position is a built-in variable to shaders. You can't see it but imagine there's
// an: out vec4 gl_Position;
// 0.4 * to illustrate we can edit final values that go into gl_Position.
// We don't want to put 0.4 on z because that affects depth
static const char* vShader = "                                        \n\
#version 330                                                          \n\
                                                                      \n\
layout (location = 0) in vec3 pos;                                    \n\
                                                                      \n\
void main() {                                                         \n\
    gl_Position = vec4(0.4 * pos.x, 0.4 * pos.y, pos.z, 1.0);         \n\
}";

// Fragment Shader
static const char* fShader = "";

void createTriangle() {
    // Create triangle. A VAO will hold multiple VBOs and other types of buffers
    // to define how a triangle will be drawn. For now, just focus on vertices;
    // the location of the points on the triangle. Usually would have multiple
    // VBOs to a VAO (locations, colors, normals, ...). But for now, just one
    // VAO with one VBO inside it
    GLfloat vertices[] = {
        // Center of screen is (0.0f, 0.0f)
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // We want to create 1 array and store its ID into VAO
    // Defines space in graphics card memory for 1 vertex array
    glGenVertexArrays(1, &VAO);

    // Now any OpenGL functions we use that interact with vertex arrays/buffer
    // will take place within this VAO
    glBindVertexArray(VAO);

    // 1 VBO
    glGenBuffers(1, &VBO);

    // VBO has multiple targets that it can bind to. In this case we want
    // GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Some people indent the VBO code to signal that we are working within a VAO

    // Connect the vertex data to the VBO. Could also use: 9 * sizeof(GLfloat)
    // Static draw as opposed to dynamic draw because we won't be changing the
    // values that we put in the array. We will move the triangle around but we
    // will do that without changing the values of the points in the array
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 1) Location of the attribute we are creating this pointer for. Remember
    // layout (location = 0) ... from the Vertex Shader? That is this 0. We want to
    // point to that value in the shader
    // 2) Size of each value that will get passed in (x, y, and z here so 3)
    // 3) The types of those values
    // 4) If want to normalize values
    // 5) Stride length if say had vertex info then its colors, then another vertex
    // with its colors ... -> stride would equal 1 (Skip 1 line every time). We will
    // not be using this so equals 0 (tighly packed data)
    // 6) Offset - can start from second triple for instance. For when not drawing all
    // of object. Not using so equals 0.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // 0 from layout (location = 0). Connecting the first argument of glVertexAttribPointer
    // to this 0 (we are enabling that location)
    glEnableVertexAttribArray(0);

    // Turning off binding of VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Turning off binding of VAO
    glBindVertexArray(0);
}

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