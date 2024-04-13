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
	glm::mat4 mRotation;
	glm::vec3 scale;

	glm::mat4 ObjectMatrix;


public:
	Object();
	~Object();
	void Draw();
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

void Object::Init(GLuint shaderProgram)
{
	ID.ShaderProgram = shaderProgram;
	glGenVertexArrays(1, &ID.VAO);
	glBindVertexArray(ID.VAO);
	glGenBuffers(1, &ID.V_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, ID.V_VBO);
	glGenBuffers(1, &ID.C_VBO);
	glGenBuffers(1, &ID.EBO);
	glGenTextures(1, &ID.Texture);
	glGenBuffers(1, &ID.TextureBuffer);
}

void Object::pushVertex(glm::vec3 vertex)
{
	vertices.push_back(vertex);
}

void Object::Draw()
{
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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