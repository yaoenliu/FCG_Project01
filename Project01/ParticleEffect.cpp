#include "ParticleEffect.hpp"


glm::mat4 ParticleEffect::translationMatrix()
{
	return glm::translate(glm::mat4(1.0f), glm::vec3(translation.x * scale.x, translation.y * scale.y, translation.z * scale.z));
}

glm::mat4 ParticleEffect::rotationMatrix()
{
	return glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}


glm::mat4 ParticleEffect::scaleMatrix()
{
	return glm::scale(glm::mat4(1.0f), scale);
}

ParticleEffect::ParticleEffect(Shader* shader,float startTime , float lifeTime , glm::vec4 color, glm::vec3 translation,
	glm::vec3 rotation , glm::vec3 scale , float angle, float radius, float heightIncrement , int nrParticles)
{
	srand(time(NULL));
	startTime = startTime;
	lifeTime = lifeTime;
	isAction = true;
	partName = "";
	offset = glm::vec3(0.0f);
	color = glm::vec4(1.0f);
	translation = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
	scale = glm::vec3(0.01f);
	angle = angle;
	radius = radius;
	heightIncrement = heightIncrement;
	lastUsedParticle = 0;
	nrParticles = nrParticles;
	lastUpdate = 0;
	parentModel = glm::mat4(1.0f);
	this->shader = shader;
	activeTime = 0;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glm::mat4 models[2];
	for (int i = -1; i < 1; i++)
	{
		models[i + 1] = glm::mat4(1.0f);
		models[i + 1] = glm::translate(models[i + 1], glm::vec3(i * 2.0f + 1, 0.0f, 0));
	}


	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * 2, &models[0], GL_STATIC_DRAW);
	GLsizei vec4Size = sizeof(glm::vec4);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(1 + i);
		glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
		glVertexAttribDivisor(1 + i, 1);
	}

	for (size_t i = 0; i < nrParticles; i++)
	{
		particles.push_back(Particle());
	}
		
}

ParticleEffect::ParticleEffect(Shader* shader)
{
	srand(time(NULL));
	startTime = 0;
	lifeTime = 1.0f;
	isAction = true;
	partName = "";
	offset = glm::vec3(0.0f);
	color = glm::vec4(1.0f);
	translation = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
	scale = glm::vec3(0.01f);
	angle = 0.0f;
	radius = 2.0f;
	heightIncrement = 0.1f;
	lastUsedParticle = 0;
	nrParticles = 1000;
	lastUpdate = 0;
	parentModel = glm::mat4(1.0f);
	activeTime = 0;
	this->shader = shader;
	shader->use();
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glm::mat4 models[2];
	for (int i = -1; i < 1; i++)
	{
		models[i + 1] = glm::mat4(1.0f);
		models[i + 1] = glm::translate(models[i + 1], glm::vec3(i * 2.0f + 1, 0.0f, 0));
	}


	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * 2, &models[0], GL_STATIC_DRAW);
	GLsizei vec4Size = sizeof(glm::vec4);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(1 + i);
		glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
		glVertexAttribDivisor(1 + i, 1);
	}

	for (size_t i = 0; i < nrParticles; i++)
		particles.push_back(Particle());
}

size_t ParticleEffect::firstUnusedParticle()
{
	for (size_t i = lastUsedParticle; i < particles.size(); i++)
	{
		if (particles[i].life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	for (size_t i = 0; i < lastUsedParticle; i++)
	{
		if (particles[i].life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	lastUsedParticle = 0;
	return 0;
}

void ParticleEffect::respawnParticle(Particle& particle)
{
	float randomAngle = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 * PI;
	particle.offset = glm::vec3(0.0f);
	particle.color = color;
	particle.life = particleLifeTime;
	particle.radius = radius;
	particle.angle = randomAngle;
	particle.hiehgtIncrement = heightIncrement;
}

void ParticleEffect::draw()
{
	if (currentTime - startTime > lifeTime)isAction = false;
	else isAction = true;
	if (!isAction)return;
	this->update();
	shader->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const Particle& particle : particles)
	{
		if (particle.life <= 0.0f)continue;
		shader->setVec3("offset", particle.offset*glm::vec3(10.0f));
		shader->setVec4("color", particle.color);
		glm::mat4 particleModel = translationMatrix()*rotationMatrix()*scaleMatrix();
		shader->setMat4("model", parentModel*particleModel);
		glBindVertexArray(cubeVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36,2);
		glBindVertexArray(0);
	}
}

void ParticleEffect::update()
{
	//add new particles
	size_t nrNewParticles = 2;
	for (size_t i = 0; i < nrNewParticles; i++)
	{
		int unusedParticle = firstUnusedParticle();
		respawnParticle(particles[unusedParticle]);
	}

	//update all particles
	for (size_t i = 0; i < nrParticles; i++)
	{
		Particle& p = particles[i];
		if (p.life <= 0.0f)continue;
		p.life -= dt;
		p.angle += dt;
		if (p.angle > 2 * PI)p.angle -= 2 * PI;
		p.offset.x = cos(p.angle) * p.radius;
		p.offset.z = sin(p.angle) * p.radius;
		p.offset.y += p.hiehgtIncrement;
	}
}

