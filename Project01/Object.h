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

	int EBOSize;

	glm::vec3 translation;
	glm::fquat rotation;
	glm::vec3 scale;
	glm::mat4 model;

public:
	std::vector<Object*> childrens;
	Object();
	~Object();
	void Init(GLuint shaderProgram, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors, std::vector<unsigned>& indice);
	void AddChild(Object* child);
	void setTranslate(glm::vec3 translation);
	void setRotation(glm::vec3 rotation);
	void Draw(glm::mat4 parentModel);
};

Object::Object()
{
	EBOSize = 0;
	translation = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	model = glm::mat4(1.0f);
}

Object::~Object()
{
}

void Object::Init(GLuint shaderProgram, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors, std::vector<unsigned>& indice)
{
	EBOSize = indice.size();
	// Create and bind VAO
	glGenVertexArrays(1, &ID.VAO);
	glBindVertexArray(ID.VAO);

	glGenBuffers(1, &ID.V_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, ID.V_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

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
	this->rotation = rotation;
}

void Object::Draw(glm::mat4 parentModel = glm::mat4(1.0f))
{
	//// Use the shader
	//glUseProgram(ID.ShaderProgram);

	// get this obj's model matrix
	model = parentModel * glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);

	// put matrix
	glUniformMatrix4fv(glGetUniformLocation(ID.ShaderProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));

	// Enable 'position' and 'color' attribute and assign data
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, ID.V_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, ID.C_VBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw triangles
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID.EBO);
	glDrawElements(GL_TRIANGLES, EBOSize, GL_UNSIGNED_INT, 0);

	// Draw children
	for (int i = 0; i < childrens.size(); i++)
		childrens[i]->Draw(model);
}
