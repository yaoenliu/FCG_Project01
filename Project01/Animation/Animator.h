#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include "model.h"
#include "modelState.h"
#include "Animation.h"


class Animator
{
private:
	Model* model;
	Shader* shader;
	int curIndex;
	float playTime, lastUpdate;

public:
	vector<Animation> animations;
	bool loopAll, loopThis, play;
	Animator(Model& model, Shader& shader);
	void update(float curTime);
	void next();
	void replay();
};

Animator::Animator(Model& model, Shader& shader) : model(&model), shader(&shader)
{
	lastUpdate = 0;
	playTime = 0;
	curIndex = 0;
	play = 0;
	loopAll = 1;
	loopThis = 0;
}

void Animator::update(float curTime)
{
	if (animations.empty())
		return;
	if (!play)
	{
		lastUpdate = curTime;
		return;
	}

	float deltaTime = curTime - lastUpdate;
	if (deltaTime < 0)                      // open windows too long
	{
		lastUpdate = curTime;
		return;
	}
	playTime += deltaTime;

	Animation& curAnimation = animations[curIndex];
	if (playTime > curAnimation.duration)   // next animation
	{
		playTime = 0;
		if (!loopThis)
			curIndex++;
	}
	if (curIndex >= animations.size())      // out of stack
	{
		playTime = 0;
		if (loopAll)
		{
			curIndex = 0;
		}
		else
		{
			curIndex = 0;
			play = 0;
			return;
		}
	}

	// render it
	modelState curState = curAnimation.update(playTime);
	model->loadModelState(curState);
	model->Draw();

	lastUpdate = curTime;
}

void Animator::next()
{
	curIndex++;
}

void Animator::replay()
{
	curIndex = 0;
	playTime = 0;
}