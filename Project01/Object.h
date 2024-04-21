#pragma once
#include <vector>
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <LoadShaders.h>
#include "GLID.h"

class Object
{
private:
	GLID ID;

	std::vector<glm::vec3> vertices;
	glm::vec3 position;
	glm::mat4 translation;
	glm::fquat rotation;
	glm::vec3 scale;


	glm::mat4 ObjectMatrix;

public:
	std::vector<Object*> childrens;
	Object();
	~Object();
	void Init(GLuint shaderProgram, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors, std::vector<unsigned>& indice);
	void AddChild(Object* child);

	void pushVertex(glm::vec3 vertex);
	void Init(GLuint shaderProgram);

	void setPosition(glm::vec3 newPose);
	void setRotation(glm::fquat rotation);
	void setScale(glm::vec3 scale);

	std::vector<Object*> childrens;
};

Object::Object()
{

	position = glm::vec3(0.0f, 0.0f, 0.0f);
	translation = glm::mat4(1.0f);
	rotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	mRotation = glm::mat4(1.0f);
	ObjectMatrix = glm::mat4(1.0f);
}

Object::~Object()
{
}

void Object::Init(GLuint shaderProgram, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors, std::vector<unsigned>& indice)
{
	EBOSize = indice.size();
	// Create and bind VAO
	glGenVertexArrays(1, &ID.VAO);

	glGenBuffers(1, &ID.V_VBO);

	glGenBuffers(1, &ID.C_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, ID.C_VBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(colors[0]), &colors[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ID.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indice.size() * sizeof(indice[0]), &indice[0], GL_STATIC_DRAW);
}

void Object::AddChild(Object* child)
{
	childrens.push_back(child);
}

void Object::setTranslate(glm::vec3 translation)
{
	this->translation = translation;
}

void Object::setRotation(glm::vec3 rotation)
{

	glBindVertexArray(ID.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, ID.V_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, ID.V_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, ID.C_VBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw triangles
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID.EBO);
	glDrawElements(GL_TRIANGLES, EBOSize, GL_UNSIGNED_INT, 0);

	ObjectMatrix = translation * mRotation * glm::scale(glm::mat4(1.0f), scale);

	GLint rotationLoc = glGetUniformLocation(ID.ShaderProgram, "rotation");
	glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, &ObjectMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void Object::setPosition(glm::vec3 newPose)
{
	this->position = newPose;
	this->translation = glm::translate(glm::mat4(1.0f), newPose);
	glm::vec4 t = glm::vec4(newPose, 1);
	glm::vec4 c1 = glm::vec4(1, 0, 0, 0);
	glm::vec4 c2 = glm::vec4(0, 1, 0, 0);
	glm::vec4 c3 = glm::vec4(0, 0, 1, 0);
	translation = glm::mat4(c1, c2, c3, t);
}