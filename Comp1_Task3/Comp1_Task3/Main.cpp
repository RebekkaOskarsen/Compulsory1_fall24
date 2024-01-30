#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <string>

using namespace std;   

//function to calculate the function f(x,y) = x*y^2
float f(float x, float y) {
    return x * y * y;
}

//function to create a xyz file with the function f(x,y) = x*y^2
void createXYZFile(const char* filename, int resolution) 
{
    ofstream file(filename);

    if (!file.is_open()) 
    {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }

   //define the area of the function
    float minX = -5.0f;
    float maxX = 5.0f;
    float minY = -5.0f;
    float maxY = 5.0f;

    //define the stepsize
    float stepX = (maxX - minX) / resolution;
    float stepY = (maxY - minY) / resolution;

    int numLines = 0;

   //loop through the area and calculate the function f(x,y) = x*y^2
    for (float x = minX; x <= maxX; x += stepX) 
    {
        for (float y = minY; y <= maxY; y += stepY) 
        {
            float z = f(x, y);
            file << x << " " << y << " " << z << endl;
            numLines++;
        }
    }

    file.close();

   
    ifstream readFile(filename);
    string fileContents;
    fileContents += to_string(numLines) + "\n";
    fileContents += string((istreambuf_iterator<char>(readFile)), istreambuf_iterator<char>());
    readFile.close();

    ofstream writeFile(filename);
    writeFile << fileContents;
    writeFile.close();
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) 
    {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL XYZ File", nullptr, nullptr);

    if (!window) 
    {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        cerr << "Failed to initialize Glad" << endl;
        return -1;
    }

    //create the xyz file
    createXYZFile("output.xyz", 5);

    glfwTerminate();
    return 0;
}
