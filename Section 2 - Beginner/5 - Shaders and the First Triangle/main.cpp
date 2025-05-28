#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstring>

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
static const char* fShader = "                                        \n\
#version 330                                                          \n\
                                                                      \n\
out vec4 color;                                                       \n\
                                                                      \n\
void main() {                                                         \n\
    color = vec4(1.0, 0.0, 0.0, 1.0);                                 \n\
}";

void createTriangle() {
    // Create triangle. A VAO will hold multiple VBOs and other types of buffers
    // to define how a triangle will be drawn. For now, just focus on vertices;
    // the location of the points on the triangle. Usually would have multiple
    // VBOs to a VAO (locations, colors, normals, ...). But for now, just one
    // VAO with one VBO inside it
    GLfloat vertices[] = {
        // Center of screen is (0.0f, 0.0f)
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
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

    // Connect the vertex data to the VBO. Could also use: 9 * sizeof(GLfloat).
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
    // 5) Stride length if say had vertex info, then its colors, then another vertex,
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

void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    // The individual shader
    GLuint theShader = glCreateShader(shaderType);

    // Array with one element (a C-string essentially) 
    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    // Length of the code string
    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    // 1) The shader we are modifying the code for
    // 2) Could have multiple strings
    // 3) The code itself (one element array of C-strings)
    // 4) The length of the code
    glShaderSource(theShader, 1, theCode, codeLength);

    // Actually compiles the shader code
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), nullptr, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
        return;
    }

    glAttachShader(theProgram, theShader);
}

void compileShaders() {
    // Shaders are on the graphics card. Get the ID here
    shader = glCreateProgram();

    if (!shader) {
        printf("Error creating shader program!\n");
        return;
    }

    // GL_VERTEX_SHADER is a built-in GLenum. So is GL_FRAGMENT_SHADER
    addShader(shader, vShader, GL_VERTEX_SHADER);
    addShader(shader, fShader, GL_FRAGMENT_SHADER);

    // Result of functions
    GLint result = 0;
    // Place to log the error
    GLchar eLog[1024] = { 0 };

    // Creates executables on the graphics card by linking the pieces
    glLinkProgram(shader);

    // Check if linked or not and store this in a GLint
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    
    if (!result) {
        // Want to know log for shader program and where we will store the errors
        glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
        printf("Error linking program: '%s'\n", eLog);
        return;
    }

    // Checks if shader program we've created is valid in the current context that
    // OpenGL is working in
    glValidateProgram(shader);

    // Check if validated or not
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
        printf("Error vaildating program: '%s'\n", eLog);
        return;
    }
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

    createTriangle();
    compileShaders();
    
    // Loop until window closed
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get and handle user input events
        glfwPollEvents();

        // Clear window
        // RGB values are between 0 and 1 not 0 and 255
        // Alpha value is how transparent it is
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Each pixel has color but more than that for graphical applications.
        // Has depth information in addtion for instance.
        // Here we clear just the color information
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader ID that was set up in compileShaders() for the pipeline.
        // This is needed because you can multiple shaders. Needs to be repeatedly done
        // again because can have multiple shaders that want to switch between
        glUseProgram(shader);

        // We are working with this particular VAO. We called this in createTriangle() so
        // that we could define the VAO by specifying vertices. Now we define the VAO that
        // we will be drawing
        glBindVertexArray(VAO);

        // Draw the points
        // 1) Mode we are using. GL_TRIANGLES means each 3 sets of coordinates will be
        // interpreted and drawn as a triangle
        // 2) Where in the array we should start (from the beginning here)
        // 3) Amount of points we want to draw
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Reset to no particular vertex array object
        glBindVertexArray(0);

        // Unassigning the shader set before. Again this is because you may want to change
        // shaders on each iteration of the loop (but we use the same shader everytime here)
        glUseProgram(0);

        // There are two scenes going on at once. The one that the user sees
        // and one that you are drawing to (can't be seen).
        // Swaps buffers so what you drew becomes what the user sees
        glfwSwapBuffers(mainWindow);
    }
    return 0;
}