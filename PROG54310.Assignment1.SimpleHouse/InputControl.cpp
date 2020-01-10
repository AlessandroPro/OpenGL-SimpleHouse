#include "InputControl.h"

#include <glm/gtc/matrix_transform.hpp>

#define HALF_PI 1.57f

using namespace glm;

float horAngle = 0.0f;
float verAngle = 0.0f;
float initialFOV = 45.0f;
float dt = 0.0f;

vec3 position = vec3(0, 0, -4); 
vec3 focus = vec3(0, 0, 0);

float speed = 2.0f; //2 units per second .. for keyboard input
float mouseSpeed = 0.003f;

GLFWwindow* _window;
int windowWidth;
int windowHeight;
float halfWidth;
float halfHeight;
float aspectRatio;

mat4 viewMatrix;
mat4 projectionMatrix;

mat4 getViewMatrix()
{
	return viewMatrix;
}

mat4 getProjectionMatrix()
{
	return projectionMatrix;
}

float getDeltaTime()
{
	return dt;
}

//Call the below function from main.cpp before the do-while loop
void initialize(GLFWwindow* win, int width, int height)
{
	_window = win;

	windowWidth = width;
	windowHeight = height;

	halfWidth = windowWidth * 0.5f;
	halfHeight = windowHeight * 0.5f;

	aspectRatio = windowWidth / windowHeight;
	glfwSetCursorPos(_window, halfWidth, halfHeight);
}

void calculateMatrices()
{
	//glfwGetTime() only runs once, the first time this function is called
	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	dt = float(currentTime - lastTime);
	double xPos, yPos;

	glfwGetCursorPos(_window, &xPos, &yPos);

	//Compute new angles
	horAngle += mouseSpeed * float(windowWidth * 0.5f - xPos);
	//our new horizontal angle is mouse speed times our difference in xPos and half of the window width

	verAngle += mouseSpeed * float(windowHeight * 0.5f - yPos);

	//horizontal angle: THETA
	//vertical angle: FI

	//direction in vector3-> convert from spherical coords to cartesian
	vec3 direction(
		cos(verAngle) * sin(horAngle),
		sin(verAngle),
		cos(verAngle) * cos(horAngle));

	//Right vector
	vec3 rightVector(
		sin(horAngle - HALF_PI),
		0,
		cos(horAngle - HALF_PI)
	);

	//Up vector:: perpendicular to our right and direction vectors
	vec3 upVector = cross(rightVector, direction);

	//Move forward
	if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		position += direction * dt * speed;
	}

	//Move backward
	if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		position -= direction * dt * speed;
	}

	//Pan right
	if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		position += rightVector * dt * speed;
	}

	//Pan left
	if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		position -= rightVector * dt * speed;
	}

	//Perspective Projection Matrix
	projectionMatrix = perspective(initialFOV, aspectRatio * 1.0f, 0.1f, 100.0f);

	//Camera matrix (view matrix)
	focus = position + direction;
	viewMatrix = lookAt(position, focus, upVector);

	lastTime = currentTime;

	//reset the mouse position to the center of the window
	glfwSetCursorPos(_window, halfWidth, halfHeight);
}

