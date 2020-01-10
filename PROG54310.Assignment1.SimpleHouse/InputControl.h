#pragma once

#ifndef INPUT_CONTROLS
#define INPUTCONTROLS_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

void initialize(GLFWwindow* win, int width, int height);

//This function will be used to calculate matrices based on our inputs
void calculateMatrices();

glm::mat4 getViewMatrix();

glm::mat4 getProjectionMatrix();

float getDeltaTime();

#endif // !INPUT_CONTROLS
