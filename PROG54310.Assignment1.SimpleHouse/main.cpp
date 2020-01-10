// Alessandro Profenna
// PROG54310 Assignment 1

#include <iostream>
#include <GL/glew.h>	
#include <GLFW/glfw3.h>	
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "InputControl.h"
#include "shader.hpp"
#include "House.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

bool InitializeWindow();
GLuint LoadTexture(const char* imagePath);
void setupVertexArray(GLuint& vaoID, GLuint& vboID, GLuint& eboID, float vertices[], GLuint indices[], int vertSize, int indicesSize);
void cleanup(GLuint& vaoID, GLuint& vboID, GLuint& eboID);

GLFWwindow* window;

// The following are two primitive shapes that can be combined together to form 3D geometry using transformations.
// Basically, this program stores vertex data for only two primitives in memory, a square and a triangle.
// These primitives are drawn multiple times with different model transformations and textures to form a 3D house

// SQUARE
float verticesSquare[] =
{
	//positions				//colors			//texture coords
	//x, y, z				//r,g,b				//s,t

	0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f, //top right
	0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f, //bottom-right
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, //bottom-left
	-0.5f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	0.0f, 1.0f  //top left
};

GLuint indicesSquare[] =
{
	0, 1, 3, //first tri
	1, 2, 3  //second tri
};

// TRIANGLE
float verticesTri[] =
{
	//positions				//colors			//texture coords
	//x, y, z				//r,g,b				//s,t

	0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 0.0f, // right
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f, // left
	0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,	0.5f, 1.0f  // top 
};

GLuint indicesTri[] =
{
	0, 1, 2 
};

// VAO, VBO, and EBO ids for the square and triangle
GLuint vaoSquareID;
GLuint vaoTriangleID;

GLuint vboSquareID;
GLuint vboTriangleID;

GLuint eboSquareID;
GLuint eboTriangleID;


int main()
{
	if (!InitializeWindow())
	{
		return -1;
	}

	// Set up the VAO, VBO, and EBO for the square and triangle verticies
	setupVertexArray(vaoSquareID, vboSquareID, eboSquareID, verticesSquare, indicesSquare, sizeof(verticesSquare), sizeof(indicesSquare));
	setupVertexArray(vaoTriangleID, vboTriangleID, eboTriangleID, verticesTri, indicesTri, sizeof(verticesTri), sizeof(indicesTri));

	// Disable cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Load the three texture used for rendering a house
	GLuint wallTexture = LoadTexture("crate.jpg");
	GLuint gableTexture = LoadTexture("gable.jpg");
	GLuint roofTexture = LoadTexture("roofTiles.png");

	// Load shaders
	GLuint programID = LoadShaders("texVS.vertexShader", "texFS.fragmentShader");
	glUseProgram(programID);

	// Define uniform variables to be used in the shaders
	glUniform1i(glGetUniformLocation(programID, "myTexture"), 0);
	GLuint vpMatrixID = glGetUniformLocation(programID, "vpMatrix"); // View-Projection Matrix
	GLuint mMatrixID = glGetUniformLocation(programID, "mMatrix");   // Model Matrix
	GLuint timeID = glGetUniformLocation(programID, "time");   // Model Matrix

	// Create a House object and generate the transformation matrices for each of it's components (walls, roof, gables)
	House simpleHouse;
	simpleHouse.generateTransformations();

	int numSquareIndices = sizeof(indicesSquare) / sizeof(indicesSquare[0]);
	int numTriIndices = sizeof(indicesTri) / sizeof(indicesTri[0]);

	// time variables used to update the uniform time variable in the fragment shader
	float time = 0;
	int sign = 1;

	// RENDER LOOP //
	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update the Projection and View Matrices based on user input
		calculateMatrices();
		glm::mat4 vpMatrix = getProjectionMatrix() * getViewMatrix();
		glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);

		//Update the time effect used in the fragment shader
		if (time > 1.0)
		{
			time = 1.0;
			sign = -1;
		}
		else if (time < 0.0)
		{
			time = 0.0;
			sign = 1;
		}
		time += (sign * getDeltaTime() * 0.5f);
		glUniform1f(timeID, time);

		// Draw the walls, roof, and gables of the simple house using the squae and triangle primitives
		simpleHouse.drawWalls(vaoSquareID, numSquareIndices, mMatrixID, wallTexture);
		simpleHouse.drawRoof(vaoSquareID, numSquareIndices, mMatrixID, roofTexture);
		simpleHouse.drawGables(vaoTriangleID, numTriIndices, mMatrixID, gableTexture);

		//swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	// Cleanup
	cleanup(vaoSquareID, vboSquareID, eboSquareID);
	cleanup(vaoTriangleID, vboTriangleID, eboTriangleID);
	glfwTerminate();
	getchar();
	return 0;
}

bool InitializeWindow()
{
	if (!glfwInit())
	{
		//Failed to initialize GLFW
		fprintf(stderr, "Failed to initialize GLFW\n");
		return false;
	}

	//multi-sampling anti-aliasing -- 4X MSAA
	glfwWindowHint(GLFW_SAMPLES, 4);

	//Targetting OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//define the profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window size(in pixels), window name
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Simple House", NULL, NULL);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to openGLFW window. Check if your GPU supports OpenGL 3.3");
		glfwTerminate(); //terminates any open glfw windows, and frees up allocated resources
		return false;
	}

	initialize(window, WINDOW_WIDTH, WINDOW_HEIGHT);

	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true; //USE this or you won't see anything drawn
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();//terminates any open glfw windows, and frees up any allocated resources
		getchar(); //to keep the console window forced open if the console window shuts on you
		return false;
	}
	//Sets a background color to the window as white
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	return true;
}

// Sets up the VAO, VBO, and EBO for the given vertex and index arrays, assuming each vertex has position, colour, and st attributes
void setupVertexArray(GLuint& vaoID, GLuint& vboID, GLuint& eboID, float vertices[], GLuint indices[], int vertSize, int indicesSize)
{
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboID);
	glGenBuffers(1, &eboID);

	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Colour attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture Coords attribute

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

// Loads a texture and returns its ID
GLuint LoadTexture(const char* imagePath)
{
	//Load texture;
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	//set wrap parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//texture width, texture height, number of color channels
	int width, height, nrChannels;

	//optional: to flip a texture verticaly on load
	stbi_set_flip_vertically_on_load(true);

	unsigned char* texData = stbi_load(imagePath, &width, &height, &nrChannels, 0);

	if (texData == NULL)
	{
		fprintf(stderr, "Can't load texture...\n");
		return -1;
	}

	//Pass the texture data to opengl
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);

	glGenerateMipmap(GL_TEXTURE_2D); //generate mipmaps

	//Once the texture data is passed to opengl, we don't need it anymore
	stbi_image_free(texData);

	return texID;
}

// Cleans up VAO, VBO, and EBO memory
void cleanup(GLuint& vaoID, GLuint& vboID, GLuint& eboID)
{
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vboID);
	glDeleteBuffers(1, &eboID);
}
