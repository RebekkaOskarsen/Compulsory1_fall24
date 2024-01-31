#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height); // Callback function for window resizing
void processInput(GLFWwindow* window); // Function to process input

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float function(float x) 
{
	return pow(cos(x), 2) + pow(sin(x), 3); // function
}

float calculateDerivative(float x, float h)
{
	return (function(x + h) - function(x)) / h; // derivative
}

vector<float> generateVertices(float xMin, float xMax, float h) // generate vertices for the graph
{
	vector<float> vertices; 
	for (float x = xMin; x <= xMax; x += h)
	{
		vertices.push_back(x);
		vertices.push_back(function(x));
		vertices.push_back(calculateDerivative(x, h));
	}
	return vertices;
}

// Function to write vertex data to a text file
void writeDataToFile(const char* filename, const vector<float>& vertices) 
{
	ofstream outFile(filename); // Open the file for writing

	if (!outFile.is_open()) 
	{
		cout << "Failed to open the file for writing." << endl;
		return;
	}

	// Write the number of points to the file
	outFile << vertices.size() / 3 << endl;

	// Write vertex data for each point to the file
	for (size_t i = 0; i < vertices.size(); i += 3) 
	{
		float x = vertices[i];
		float y = vertices[i + 1];
		float derivative = vertices[i + 2];

		outFile << x << " " << y << " " << derivative << endl;
	}

	outFile.close(); // Close the file
}

int main()
{
	//glfw: initialize and configure
//-------------------------------
	glfwInit(); // Initialize GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Set the OpenGL version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Set the OpenGL version to 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set the OpenGL profile to core

	//glfw window creation
	//--------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Window", NULL, NULL); // Create a window object
	if (window == NULL) // Check if the window creation failed
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate(); // Terminate GLFW
		return -1;
	}
	glfwMakeContextCurrent(window); // Make the window's context current
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Register the window resize callback function

	//glad: load all OpenGL function pointers
	//---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // Initialize GLAD
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	vector<float> vertices = generateVertices(-1.0f * acos(-1.0), 1.0f * acos(-1.0), 0.2f); // generate vertices for the graph
	const char* filename = "data-points.txt";
	writeDataToFile(filename, vertices);

	// Vertex shader source code
	const char* vertexShaderSource = 
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"out vec4 color;\n"
		"void main() {\n"
		"   gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);\n"
		"   color = vec4(aPos.z > 0.0f ? 0.0f : 1.0f, aPos.z > 0.0f ? 1.0f : 0.0f, 0.0f, 1.0f);\n"
		"}\0";

	// Fragment shader source code
	const char* fragmentShaderSource =
		"#version 330 core\n"
		"in vec4 color;\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"   FragColor = color;\n"
		"}\0";

	//build and compile our shader program
//------------------------------------
//vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create a vertex shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Attach the shader source code to the shader object
	glCompileShader(vertexShader); // Compile the shader
	//check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // Check if the shader compiled successfully
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); // Get the error message
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl; // Print the error message
	}

	//fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // Attach the shader source code to the shader object
	glCompileShader(fragmentShader); // Compile the shader
	//check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // Check if the shader compiled successfully
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog); // Get the error message
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl; // Print the error message
	}

	//link shaders
	unsigned int shaderProgram = glCreateProgram(); // Create a shader program object
	glAttachShader(shaderProgram, vertexShader); // Attach the vertex shader to the shader program
	glAttachShader(shaderProgram, fragmentShader); // Attach the fragment shader to the shader program
	glLinkProgram(shaderProgram); // Link the shader program
	//check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // Check if the shader program linked successfully
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog); // Get the error message
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl; // Print the error message
	}
	glDeleteShader(vertexShader); // Delete the vertex shader object
	glDeleteShader(fragmentShader); // Delete the fragment shader object

	//set up vertex data (and buffer(s)) and configure vertex attributes
	//------------------------------------------------------------------
	unsigned int VBO, VAO, EBO; // Create a vertex buffer object and a vertex array object
	glGenVertexArrays(1, &VAO); // Generate a vertex array object
	glGenBuffers(1, &VBO); // Generate a vertex buffer object
	glGenBuffers(1, &EBO); // Generate a element buffer object

	glBindVertexArray(VAO); // Create vertex array object (VAO)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);// Create vertex buffer object (VBO)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); 

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	

	//render loop
	//-----------
	while (!glfwWindowShouldClose(window)) // Check if the window should close
	{
		//input
		//-----
		processInput(window); // Process the input

		//render
		//------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT); // Clear the buffers

		glUseProgram(shaderProgram); // Use the shader program when we want to render an object
		glBindVertexArray(VAO); // Bind the vertex array object

		glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);

		//glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		//---------------------------------------------------------------------------------
		glfwSwapBuffers(window); // Swap the screen buffers
		glfwPollEvents(); // Check if any events are triggered (like keyboard input or mouse movement events)
	}

	//optional: de-allocate all resources once they've outlived their purpose
	//------------------------------------------------------------------------
	glDeleteProgram(shaderProgram); // Delete the shader program
	glDeleteBuffers(1, &VBO); // Delete the vertex buffer object 
	glDeleteVertexArrays(1, &VAO); // Delete the vertex array object

	//glfw: terminate, clearing all previously allocated GLFW resources
	//-----------------------------------------------------------------
	glfwTerminate(); // Terminate GLFW
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // Check if the escape key is pressed
		glfwSetWindowShouldClose(window, true); // Close the window
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
	{
		cout << "A is pressed" << endl;
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height); // Set the viewport size
	cout << "Window resized with " << width << "Height" << height << endl;

}
