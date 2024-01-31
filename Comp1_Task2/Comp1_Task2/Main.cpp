#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aColor;

    out vec3 color;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        color = aColor;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 color;
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(color, 1.0);
    }
)";

void generateSpiralData(const char* filename)
{
    ofstream outputFile(filename);

    if (!outputFile.is_open())
    {
        cerr << "Error opening output file" << endl;
        exit(EXIT_FAILURE);
    }

    const float spiralRadius = 0.5f;
    const float spiralHeight = 2.0f;
    const int numPoints = 50;
    const float step = 2 * acos(-1.0) / numPoints;

    for (int i = 0; i < numPoints; ++i)
    {
        float t = i * step;
        float x = spiralRadius * cos(t);
        float y = spiralRadius * sin(t);
        float z = (spiralHeight / (2 * acos(-1.0))) * t;

        outputFile << x << " " << y << " " << z << endl;
    }

    outputFile.close();

}

int main()
{
    const char* spiralDataFile = "spiral_data.txt";

    // Generate spiral data
    generateSpiralData(spiralDataFile);

    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFW window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Spiral Window", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Shader program
    // --------------
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compile errors...

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compile errors...

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for shader linking errors...

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Generate spiral data
    // --------------------
    const float spiralRadius = 0.5f;
    const float spiralHeight = 2.0f;
    const int numPoints = 300;
    const float step = 2 * acos(-1.0) / numPoints;

    float vertices[numPoints * 3 * 2];  // Each point has a position (x, y, z) and a color (r, g, b)
    int vertexIndex = 0;

    for (int i = 0; i < numPoints; ++i)
    {
        float t = i * step;
        float x = spiralRadius * cos(t);
        float y = spiralRadius * sin(t);
        float z = (spiralHeight / (2 * acos(-1.0))) * t;

        // Position
        vertices[vertexIndex++] = x;
        vertices[vertexIndex++] = y;
        vertices[vertexIndex++] = z;

        // Color
        vertices[vertexIndex++] = static_cast<float>(i) / numPoints;  // Red
        vertices[vertexIndex++] = 0.0f;                                 // Green
        vertices[vertexIndex++] = 1.0f;                                 // Blue
    }

    // Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
    // --------------------------------------------------------
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, numPoints);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    // -------
    glDeleteVertexArrays(1, &VAO);

    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
