#pragma once
#include <map>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Represents the data used in creating components a house out of simple primitive shapes
// Also draws the components
class House
{
public:

	std::map<std::string, glm::mat4*> transformations; // a map of the model transformation matrices of each piece of the house

	~House();

	void generateTransformations();
	void drawWalls(GLuint& vaoID, int numIndices, GLuint& modelMatrixID, GLuint& textureID);
	void drawRoof(GLuint& vaoID, int numIndices, GLuint& modelMatrixID, GLuint& textureID);
	void drawGables(GLuint& vaoID, int numIndices, GLuint& modelMatrixID, GLuint& textureID);
	void drawShape(int numIndices, GLuint& modelMatrixID, glm::mat4* modelMatrix);
};

