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
	glm::vec3 translation;
	glm::fquat rotation;
	glm::vec3 scale;
	std::vector<Object*> childrens;

public:
	Object();
	~Object();
	void Draw();
	void AddChild(Object* child);
	void pushVertex(glm::vec3 vertex);
	void Init(GLuint shaderProgram);
};

Object::Object()
{
	translation = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
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
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
