#pragma once
#include "shader.hpp"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

#define PI 3.14159265359




struct Particle
{
	glm::vec3 offset;
	glm::vec4 color;
	float life;
	float radius;
	float angle;
	float hiehgtIncrement;
	Particle(): offset(glm::vec3(0.0f)), color(glm::vec4(1.0f)),
		life(0.0f), radius(0.0f), angle(0.0f), hiehgtIncrement(0.0f) { }
};

class ParticleEffect
{
public:
	float startTime;
	float lifeTime;
	bool isAction;
	std::string partName;
	glm::vec3 offset;
	glm::vec4 color;
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::mat4 parentModel;
	float angle;
	float radius;
	float heightIncrement;
	std::vector<Particle> particles;
	size_t lastUsedParticle;
	size_t nrParticles;
	Shader* shader;
	GLuint cubeVAO, cubeVBO;
	float dt = 0.01;
	float lastUpdate;

	ParticleEffect(Shader* shader, float startTime, float lifeTime, glm::vec4 color, glm::vec3 translation,
		glm::vec3 rotation, glm::vec3 scale, float angle, float radius, float heightIncrement, int nrParticles);
	ParticleEffect(Shader* shader);
	glm::mat4 translationMatrix();
	glm::mat4 rotationMatrix();
	glm::mat4 scaleMatrix();
	size_t firstUnusedParticle();
	void respawnParticle(Particle& particle);
	void draw();
	void update();

	GLfloat quadVertices[36][6] = {
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	};
};
