#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>

#include "model.hpp"

#include "modelState.h"

class keyFrame
{
public:
	modelState state;
	float time;

    keyFrame(modelState state, float time) :state(state), time(time)
    {

    }
};

class animation
{
private:
    modelState interpolate(modelState& start, modelState& end, float progression)
    {
        modelState state;
        if (start.isJoint && end.isJoint)
        {
            state.isJoint = 1;
            state.translation = glm::mix(start.translation, end.translation, progression);
            state.rotation = glm::slerp(start.rotation, end.rotation, progression);
            state.scale = glm::mix(start.scale, end.scale, progression);
        }

        if (start.children.size() != end.children.size() || start.isJoint xor end.isJoint)
        {
            std::cout << "modelState problem";
            return state;
        }

        for (int i = 0; i < start.children.size(); i++)
        {
            state.children.push_back(interpolate(start.children[i], end.children[i], progression));
        }
        return state;
    }

public:
	std::vector<keyFrame> keyFrames;
    float duration;

    animation(float duration) : duration(duration)
	{

	}

    modelState update(float aniTime)
	{
        keyFrame startFrame = keyFrames.back();
        for (auto& frame : keyFrames) {
            if (frame.time > aniTime) {
                keyFrame endFrame = frame;

                float progression = (aniTime - startFrame.time) / (endFrame.time - startFrame.time);

                return interpolate(startFrame.state, endFrame.state, progression);
            }
            startFrame = frame;
        }
        return modelState();
	}
};

class animator
{
private:
    Model model;
    Shader shader;
    int curIndex;
    float playTime, lastUpdate;

public:
    vector<animation> animations;
    bool loopAll, loopThis, play;

    animator(Model& model, Shader& shader) : model(model), shader(shader)
    {
        lastUpdate = 0;
        playTime = 0;
        curIndex = 0;
        play = 0;
        loopAll = 1;
        loopThis = 0;
    }

    void update(float curTime)
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

        animation& curAnimation = animations[curIndex];
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
        model.rootMesh->loadModelState(curState);
        model.Draw(shader);

        lastUpdate = curTime;
    }

    void next()
    {
        curIndex++;
    }

    void replay()
    {
        curIndex = 0;
        playTime = 0;
    }
};

