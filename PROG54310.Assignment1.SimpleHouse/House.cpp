#include "House.h"

House::~House()
{
	// Delete allocated memory of matrix pointers
	for (const auto& transformation : transformations)
	{
		if (transformation.second != nullptr)
		{
			delete transformation.second;
		}
	}
}

// Generates the transformation matrices of the house's component parts (walls, roof, gables) 
// and stores them in a map so they can be accessed at draw time and sent to the vertex shader
void House::generateTransformations()
{
	glm::mat4 identityMatrix(1.0);
	glm::mat4 houseScaleMatrix = glm::scale(glm::vec3(1, 1, 1.2));

	//front wall and gable
	glm::mat4 frontTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0, 0, 0.5));
	glm::mat4* frontTransformMatrix = new glm::mat4(houseScaleMatrix * frontTranslateMatrix);
	transformations["Front"] = frontTransformMatrix;

	//back wall and gable
	glm::mat4 backTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0, 0, -0.5));
	glm::mat4 backRotateMatrix = glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0));
	glm::mat4* backTransformMatrix = new glm::mat4(houseScaleMatrix * backTranslateMatrix * backRotateMatrix * identityMatrix);
	transformations["Back"] = backTransformMatrix;

	//left wall
	glm::mat4 lwTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5, 0, 0));
	glm::mat4 lwRotateMatrix = glm::rotate(glm::radians(-90.0f), glm::vec3(0, 1, 0));
	glm::mat4* lwTransformMatrix = new glm::mat4(houseScaleMatrix * lwTranslateMatrix * lwRotateMatrix * identityMatrix);
	transformations["Left Wall"] = lwTransformMatrix;

	//right wall
	glm::mat4 rwTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 0, 0));
	glm::mat4 rwRotateMatrix = glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0));
	glm::mat4* rwTransformMatrix = new glm::mat4(houseScaleMatrix * rwTranslateMatrix * rwRotateMatrix * identityMatrix);
	transformations["Right Wall"] = rwTransformMatrix;

	//left roof
	glm::mat4 lrScaleMatrix = glm::scale(glm::vec3(1, 1.1, 1));
	glm::mat4 lrTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5, 0, 0));
	glm::mat4 lrRotateMatrix = glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));
	glm::mat4 lrRotateMatrix2 = glm::rotate(glm::radians(45.0f), glm::vec3(0, 0, 1));
	glm::mat4 lrTranslateMatrix2 = glm::translate(identityMatrix, glm::vec3(0, 1, 0));
	glm::mat4* lrTransformMatrix = new glm::mat4(houseScaleMatrix * lrTranslateMatrix2 * lrRotateMatrix2 * lrRotateMatrix * lrTranslateMatrix * lrScaleMatrix * identityMatrix);
	transformations["Left Roof"] = lrTransformMatrix;

	//right roof
	glm::mat4 rrScaleMatrix = glm::scale(glm::vec3(1, 1.1, 1));
	glm::mat4 rrTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 0, 0));
	glm::mat4 rrRotateMatrix = glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));
	glm::mat4 rrRotateMatrix2 = glm::rotate(glm::radians(-45.0f), glm::vec3(0, 0, 1));
	glm::mat4 rrTranslateMatrix2 = glm::translate(identityMatrix, glm::vec3(0, 1, 0));
	glm::mat4* rrTransformMatrix = new glm::mat4(houseScaleMatrix *  rrTranslateMatrix2 * rrRotateMatrix2 * rrRotateMatrix * rrTranslateMatrix * rrScaleMatrix * identityMatrix);
	transformations["Right Roof"] = rrTransformMatrix;
}

// Draws the four walls of the house using the given shape primitive and texture
void House::drawWalls(GLuint& vaoID, int numIndices, GLuint& modelMatrixID, GLuint& textureID)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindVertexArray(vaoID);

	drawShape(numIndices, modelMatrixID, transformations["Front"]);
	drawShape(numIndices, modelMatrixID, transformations["Back"]);
	drawShape(numIndices, modelMatrixID, transformations["Left Wall"]);
	drawShape(numIndices, modelMatrixID, transformations["Right Wall"]);

}

// Draws the two roof pieces of the house using the given shape primitive and texture
void House::drawRoof(GLuint& vaoID, int numIndices, GLuint& modelMatrixID, GLuint& textureID)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindVertexArray(vaoID);

	drawShape(numIndices, modelMatrixID, transformations["Left Roof"]);
	drawShape(numIndices, modelMatrixID, transformations["Right Roof"]);

}

// Draws the two gables of the house using the given shape primitive and texture
void House::drawGables(GLuint& vaoID, int numIndices, GLuint& modelMatrixID, GLuint& textureID)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindVertexArray(vaoID);

	drawShape(numIndices, modelMatrixID, transformations["Front"]);
	drawShape(numIndices, modelMatrixID, transformations["Back"]);
}

// Transforms and draws shape that is defined in the currently binded VAO
void House::drawShape(int numIndices, GLuint& modelMatrixID, glm::mat4* modelMatrix)
{
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, (GLfloat*)modelMatrix);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}
